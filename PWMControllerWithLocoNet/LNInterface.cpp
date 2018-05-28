#include "LNInterface.h"
#define FUNC_AMNT 9

LNInterface::FunctionNode* LNInterface::functions_root = NULL;	// linked list
LNInterface::FunctionNode** LNInterface::functions_map = NULL;	// array of pointers pointing to elements in linked list above
LocoNetThrottleClass LNInterface::Throttle;
bool LNInterface::is_disabled = false;

// used for processing and printing LocoNet packets
void LNInterface::lnUpdate()
{
	LnPacket = LocoNet.receive();
	if (LnPacket) {
		// print out packet
		Serial.print(F(" LN: RX: "));
		uint8_t msgLen = getLnMsgSize(LnPacket);
		for (uint8_t x = 0; x < msgLen; x++)
		{
			uint8_t val = LnPacket->data[x];
			if (val < 16)
				Serial.print('0');

			Serial.print(val, HEX);
			Serial.print(' ');
		}

		// if this packet was not a Switch or Sensor Message then print a new line, and process it as a Throttle Message
		if (!LocoNet.processSwitchSensorMessage(LnPacket)) {
			Serial.println();
			Throttle.processMessage(LnPacket);
		}
	}
}


// constructor
// when the period is 0, animations will run as fast as possible and animations will run slower if more are running at the same time
// use 16 as a good period to get 62.5 updates per second
LNInterface::LNInterface(uint8_t tx_pin, uint16_t loco_address, uint8_t period)
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

	// initialize class variables

	functions_map = new FunctionNode*[FUNC_AMNT];
	for (int i = 0; i < FUNC_AMNT; ++i) {
		functions_map[i] = NULL;
	}
	buttons_root = NULL;

	this->period = period;
	target_time = 0;

	Serial.println(F(" LNI: Self Initialized"));
}


// add an animation to trigger on a specific decoder function
// if cts, then the animation will run repeatedly until button is depressed
// else the animation will only run once
void LNInterface::mapAnimation(Animation *ani, uint8_t fn_num, bool cts)
{
	if (fn_num >= FUNC_AMNT) {
		Serial.println(F(" LNI: Function past limit"));
		return;
	}

	// add animation to the linked list of FunctionNodes and create a pointer to it in the array
	if (!functions_root) {
		functions_root = new FunctionNode(ani, fn_num, cts);
		functions_map[fn_num] = functions_root;
	} else {
		FunctionNode *traversal = functions_map[fn_num];
		if (traversal) {
			Serial.println(F(" LNI: Found duplicate, updating"));
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

// add a physical button input on a certain pin to trigger a decoder function
// it is possible to map a button to a function that doesn't have an animation, though it is not useful in this case
void LNInterface::mapButton(uint8_t pin_num, uint8_t fn_num)
{
	if (fn_num >= FUNC_AMNT) {
		Serial.println(F(" LNI: Function past limit"));
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

// DEPRECATED - avoid using this function since it's possibly faulty and confusing
// used for finding the run speed of functions
// use it when the LNInterface update period is set to 0,
// begin serial outpit, run function,
// set clock speed to the largest printed value minus 51
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
			// Serial.println(max);
		}
		Serial.print(traversal->func_num); Serial.print(" - Max: "); Serial.println(max);
		max = 0;
	}
}

// update method that causes everything to happen; place this function in loop() method
void LNInterface::update()
{
	lnUpdate();													// check LocoNet, and run all callback functions
	if (is_disabled) return;									// if disabled, don't do anything

	if (period != 0) {											// handle the correct period
		if (target_time < millis()) {
			target_time = millis() + period;
		} else {
			return;
		}
	}

	if (functions_root) {										// run through all functions, and update running / newly running ones
		FunctionNode *traversal = functions_root;
		do {
			/* 
			* disambiguation of next line
			* if the function was running, run it
			* if the function isn't running anymore, check LocoNet
			* if LocoNet says the function is enabled, run it
			*/
			if (traversal->running && !(traversal->running = traversal->ani->run()) && Throttle.getFunction(traversal->func_num) != 0) {
				traversal->running = true;
			}
		} while (traversal = traversal->next);
	}

	if (buttons_root) {											// run through all buttons and update LocoNet accordingly
		ButtonNode *traversal = buttons_root;
		do {
			if (digitalRead(traversal->pin)) {
				Throttle.setFunction(traversal->fn, 1);
				// Serial.println(traversal->fn);
			}
		} while (traversal = traversal->next);
	}
}

// set a function's state to running if a function is turned on
static void LNInterface::updateFunction(uint8_t Function, uint8_t Value)
{
	if (Value != 0 && functions_map[Function]) {
		functions_map[Function]->running = true;
		if (!functions_map[Function]->cts) Throttle.setFunction(Function, 0);
	}
}

// turn everything off if the direction is reverse
static void LNInterface::updateOnOff(uint16_t Direction) {
	if (Direction) {
		for (FunctionNode *traversal = functions_root; traversal; traversal = traversal->next) traversal->ani->killPWM();
		Serial.println(F(" LN: Disabled."));
		is_disabled = true;
	} else {
		is_disabled = false;
	}
}


// LocoNet callback functions, do not use

// void notifyThrottleAddress(uint8_t UserData, TH_STATE State, uint16_t Address, uint8_t Slot)
// {
// 	Serial.print(F(" LN: Address: "));		Serial.println(Address);
// }

// void notifyThrottleSlotStatus(uint8_t UserData, uint8_t Status)
// {
// 	Serial.print(F(" LN: Slot Status: "));	Serial.println(Status);
// }

// void notifyThrottleError(uint8_t UserData, TH_ERROR Error)
// {
// 	Serial.println(F(" LN: ERROR"));			//Serial.println(Throttle.getErrorStr(Error));
// }

// void notifyThrottleState(uint8_t UserData, TH_STATE PrevState, TH_STATE State)
// {
// 	Serial.println(F(" LN: State Changed"));			//Serial.println(Throttle.getStateStr(State));
// }

// void notifyThrottleSpeed(uint8_t UserData, TH_STATE State, uint8_t Speed)
// {
// 	Serial.print(F(" LN: Speed: "));			Serial.println(Speed);
// }

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