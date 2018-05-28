#include "LNInterface.h"
#define FUNC_AMNT 9

LNInterface::FunctionNode* LNInterface::functions_root = NULL;
LNInterface::FunctionNode** LNInterface::functions_map = NULL;
LocoNetThrottleClass LNInterface::Throttle;
bool LNInterface::is_disabled = false;

// used for printing and processing LocoNet packets
void LNInterface::lnUpdate()
{
	LnPacket = LocoNet.receive();
	if (LnPacket) {
		// First print out the packet in HEX
		Serial.print(F(" LN: RX: "));
		uint8_t msgLen = getLnMsgSize(LnPacket);
		for (uint8_t x = 0; x < msgLen; x++)
		{
			uint8_t val = LnPacket->data[x];
			// Print a leading 0 if less than 16 to make 2 HEX digits
			if (val < 16)
				Serial.print('0');

			Serial.print(val, HEX);
			Serial.print(' ');
		}

		// If this packet was not a Switch or Sensor Message then print a new line, and process it as a Throttle Message
		if (!LocoNet.processSwitchSensorMessage(LnPacket)) {
			Serial.println();
			Throttle.processMessage(LnPacket);
		}
	}
}

LNInterface::LNInterface(uint8_t tx_pin, uint16_t loco_address, uint8_t frequency)
{
	LocoNet.init(tx_pin);
	Throttle.init(0, 0, loco_address);

	// attempt to initialize onto existing address
	Serial.println(" LNI: Stealing Address");
	Throttle.stealAddress(loco_address);
	lnUpdate();     // repeated to process possible errors,
	lnUpdate();     // do not remove duplicate line

	// if address does not exist, create one
	if (Throttle.getState() == TH_ST_FREE) {
		Serial.println(F(" LNI: Setting Address"));
		Throttle.setAddress(loco_address);
	}
	lnUpdate();

	Serial.println(F(" LNI: Initialized LN part."));

	functions_map = new FunctionNode*[FUNC_AMNT];
	for (int i = 0; i < FUNC_AMNT; ++i) {
		functions_map[i] = NULL;
	}
	buttons_root = NULL;

	this->frequency = frequency;
	target_time = 0;

	Serial.println(F(" LNI: Self Initialized"));
}

void LNInterface::map(Animation *ani, uint8_t fn_num, bool cts)
{
	if (fn_num >= FUNC_AMNT) {
		Serial.println(F("Function past limit"));
		return;
	}

	if (!functions_root) {
		functions_root = new FunctionNode(ani, fn_num, cts);
		functions_map[fn_num] = functions_root;
	} else {
		FunctionNode *traversal = functions_map[fn_num];
		if (traversal) {
			Serial.println(F("Found duplicate"));
			traversal->ani = ani;
			traversal->cts = cts;
		} else {
			traversal = functions_root;
			while (traversal->next) traversal = traversal->next;
			traversal->next = new FunctionNode(ani, fn_num, cts);
			functions_map[fn_num] = traversal->next;
		}
	}

	Serial.println(F(" LNI: Mapped an animation"));
}

void LNInterface::mapButton(uint8_t pin_num, uint8_t fn_num)
{
	if (fn_num >= FUNC_AMNT) {
		Serial.println(F("Function past limit"));
		return;
	}

	pinMode(pin_num, INPUT_PULLUP);
	if (buttons_root) {
		ButtonNode *traversal = buttons_root;
		while (traversal->next) traversal = traversal->next;
		traversal->next = new ButtonNode(pin_num, fn_num);
	} else {
		buttons_root = new ButtonNode(pin_num, fn_num);
	}
}

void LNInterface::printRunningSpeeds() {
	Serial.print(F(" LNI: Printing Speeds:"));
	lnUpdate();
	lnUpdate();
	long max = 0, x0 = 0, x1 = 0;
	for (FunctionNode *traversal = functions_root; traversal; traversal = traversal->next) {
		Throttle.setFunction(traversal->func_num, 1);
		this->update();
		Throttle.setFunction(traversal->func_num, 0);
		while (traversal->running) {
			x0 = millis();
			this->update();
			x1 = millis();
			max = max(max, x1 - x0);
			Serial.println(max);
		}
		Serial.print(traversal->func_num); Serial.print(" - Max: "); Serial.println(max);
		max = 0;
	}
}

void LNInterface::update()
{
	lnUpdate();
	if (is_disabled) return;

	if (frequency != 0) {
		if (target_time < millis()) {
			target_time = millis() + frequency;
		} else {
			return;
		}
	}

	if (functions_root) {
		FunctionNode *traversal = functions_root;
		do {
			if (traversal->running && !(traversal->running = traversal->ani->runAnimation()) && Throttle.getFunction(traversal->func_num) != 0) {
				traversal->running = true;
			}
		} while (traversal = traversal->next);
	}
	if (buttons_root) {
		ButtonNode *traversal = buttons_root;
		do {
			if (digitalRead(traversal->pin)) {
				functions_map[traversal->fn]->running = true;
			}
		} while (traversal = traversal->next);
	}
}

static void LNInterface::updateFunction(uint8_t Function, uint8_t Value)
{
	if (Function < FUNC_AMNT - 2 && Value != 0 && functions_map[Function]) {
		// Serial.print(Function); Serial.print('\t'); Serial.print(functions_map[Function]->func_num); Serial.print('\t'); Serial.println(functions_map[Function]->cts);
		functions_map[Function]->running = true;
		if (!functions_map[Function]->cts) Throttle.setFunction(Function, 0);
	}
}

static void LNInterface::updateOnOff(uint16_t Direction) {
	Serial.println(F("Disabling function triggered."));
	if (Direction) {
		for (FunctionNode *traversal = functions_root; traversal; traversal = traversal->next) traversal->ani->killPWM();
		Serial.println(F("Disabled function."));
		is_disabled = true;
	} else {
		is_disabled = false;
	}
}


void notifyThrottleAddress(uint8_t UserData, TH_STATE State, uint16_t Address, uint8_t Slot)
{
	Serial.print(F(" LN: Address: "));		Serial.println(Address);
}

void notifyThrottleSlotStatus(uint8_t UserData, uint8_t Status)
{
	Serial.print(F(" LN: Slot Status: "));	Serial.println(Status);
}

void notifyThrottleError(uint8_t UserData, TH_ERROR Error)
{
	Serial.println(F(" LN: ERROR"));			//Serial.println(Throttle.getErrorStr(Error));
}

void notifyThrottleState(uint8_t UserData, TH_STATE PrevState, TH_STATE State)
{
	Serial.println(F(" LN: State Changed"));			//Serial.println(Throttle.getStateStr(State));
}

void notifyThrottleSpeed(uint8_t UserData, TH_STATE State, uint8_t Speed)
{
	Serial.print(F(" LN: Speed: "));			Serial.println(Speed);
}

void notifyThrottleDirection(uint8_t UserData, TH_STATE State, uint8_t Direction)
{
	Serial.print(F(" LN: Direction: "));		Serial.println(Direction);
	LNInterface::updateOnOff(Direction);
}

void notifyThrottleFunction(uint8_t UserData, uint8_t Function, uint8_t Value)
{
	Serial.print(F(" LN: Function: "));		Serial.print(Function);		Serial.print(F("\tLN: Value: "));	Serial.println(Value);
	LNInterface::updateFunction(Function, Value);
}