#include "Animation.cpp"
#include <LocoNet.h>


// void notifyThrottleAddress(uint8_t UserData, TH_STATE State, uint16_t Address, uint8_t Slot)    {Serial.print(F("LN: Address: "));     Serial.println(Address);}
// void notifyThrottleSlotStatus(uint8_t UserData, uint8_t Status)                                 {Serial.print(F("LN: Slot Status: ")); Serial.println(Status);}
// // void notifyThrottleError(uint8_t UserData, TH_ERROR Error)                                      {Serial.print(F("LN: Error: "));       Serial.println(Throttle.getErrorStr(Error));}
// // void notifyThrottleState(uint8_t UserData, TH_STATE PrevState, TH_STATE State)                  {Serial.print(F("LN: State: "));       Serial.println(Throttle.getStateStr(State));}
// void notifyThrottleSpeed(uint8_t UserData, TH_STATE State, uint8_t Speed)                       {Serial.print(F("LN: Speed: "));       Serial.println(Speed);}
// void notifyThrottleDirection(uint8_t UserData, TH_STATE State, uint8_t Direction)               {Serial.print(F("LN: Direction: "));   Serial.println(Direction);}
// void notifyThrottleFunction(uint8_t UserData, uint8_t Function, uint8_t Value) 					{Serial.print(F("LN: Function: "));    Serial.print(Function);
// 	Serial.print(F("\tLN: Value: ")); Serial.println(Value);
// 	// if (Value != 0) {
// 	// 	running[Function] = Value;
// 	// 	if (!continuous[Function]) Throttle.setFunction(Function, 0);
// 	// }
// }

class LNInterface {

private:

	Animation **functions;
	bool *continuous;
	bool *running;

	lnMsg *LnPacket;
	LocoNetThrottleClass Throttle;

	// used for printing and processing LocoNet packets
	void lnUpdate() {
		LnPacket = LocoNet.receive();
		if (LnPacket) {
			// First print out the packet in HEX
			Serial.print(F("RX: "));
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

public:

	void notifyThrottleAddress(uint8_t UserData, TH_STATE State, uint16_t Address, uint8_t Slot)	{Serial.print(F("LN: Address: "));     Serial.println(Address);}
	void notifyThrottleSlotStatus(uint8_t UserData, uint8_t Status)									{Serial.print(F("LN: Slot Status: ")); Serial.println(Status);}
	void notifyThrottleError(uint8_t UserData, TH_ERROR Error)										{Serial.print(F("LN: Error: "));       Serial.println(Throttle.getErrorStr(Error));}
	void notifyThrottleState(uint8_t UserData, TH_STATE PrevState, TH_STATE State)					{Serial.print(F("LN: State: "));       Serial.println(Throttle.getStateStr(State));}
	void notifyThrottleSpeed(uint8_t UserData, TH_STATE State, uint8_t Speed)						{Serial.print(F("LN: Speed: "));       Serial.println(Speed);}
	void notifyThrottleDirection(uint8_t UserData, TH_STATE State, uint8_t Direction)				{Serial.print(F("LN: Direction: "));   Serial.println(Direction);}
	void notifyThrottleFunction(uint8_t UserData, uint8_t Function, uint8_t Value)					{Serial.print(F("LN: Function: "));    Serial.print(Function);
	Serial.print(F("\tLN: Value: ")); Serial.println(Value);
		// if (Value != 0) {
		// 	running[Function] = Value;
		// 	if (!continuous[Function]) Throttle.setFunction(Function, 0);
		// }
	}

LNInterface(uint8_t tx_pin, uint16_t loco_address) {
		LocoNet.init(tx_pin);
		Throttle.init(0, 0, loco_address);

		// attempt to initialize onto existing address
		Serial.println("LN: Stealing Address");
		Throttle.stealAddress(loco_address);
		lnUpdate();     // repeated to process possible errors,
		lnUpdate();     // do not remove duplicate line

		// if address does not exist, create one
		if (Throttle.getState() == TH_ST_FREE) {
			Serial.println(F("LN: Setting Address"));
			Throttle.setAddress(loco_address);
		}
		lnUpdate();

		Serial.println(F("LN: Initialized LN part."));

		functions = new Animation*[29] {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};
		continuous = new bool[29] {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
		running = new bool[29] {false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false};
		Serial.println(F("Initialized arrays."));
	}

	void map(Animation *ani, uint8_t fn, bool cts) {
		functions[fn] = ani;
		continuous[fn] = cts;
	}

	void update() {
		lnUpdate();
		// for (int i = 0; i < 29; ++i) {
		// 	if (running[i]) running[i] = functions[i]->runAnimation();
		// }
	}
};