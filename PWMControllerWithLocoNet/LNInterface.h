#include "Animation.h"
#include <LocoNet.h>

#ifndef LNINTERFACE_H
#define LNINTERFACE_H

class LNInterface
{
	private:
		struct FunctionNode {
			Animation *ani;
			uint8_t func_num;
			bool cts;
			volatile bool running;
			FunctionNode *next;
			FunctionNode(Animation *a, uint8_t fn, bool c) : ani(a), cts(c), func_num(fn), running(false), next(NULL) {}
		};
		static FunctionNode *functions_root;	// used for running all declared function animations quickly
		static FunctionNode **functions_map;	// used for finding and starting a function quickly (on LN function press)

		struct ButtonNode {
			uint8_t pin;
			uint8_t fn;
			ButtonNode *next;
			ButtonNode(uint8_t p, uint8_t f) : pin(p), fn(f), next(NULL) {}
		};
		ButtonNode *buttons_root;

		static bool is_disabled;
		uint8_t period;
		unsigned long target_time;

		lnMsg *LnPacket;
		static LocoNetThrottleClass Throttle;
		void lnUpdate();

	public:
		LNInterface(uint8_t tx_pin, uint16_t loco_address, uint8_t period);			// 16 ms is a good period
		void mapAnimation(Animation *ani, uint8_t fn, bool cts);
		void mapButton(uint8_t pin_num, uint8_t fn_num);
		void printRunningSpeeds();													// deprecated; requires Serial
		void update();																// use in main loop

		static void updateFunction(uint8_t Function, uint8_t Value);				// do not use locally
		static void updateOnOff(uint16_t Direction);								// do not use locally
};

// callback functions from LocoNet; do not attempt to use locally
// void notifyThrottleAddress(uint8_t UserData, TH_STATE State, uint16_t Address, uint8_t Slot);
// void notifyThrottleSlotStatus(uint8_t UserData, uint8_t Status);
// void notifyThrottleError(uint8_t UserData, TH_ERROR Error);
// void notifyThrottleState(uint8_t UserData, TH_STATE PrevState, TH_STATE State);
// void notifyThrottleSpeed(uint8_t UserData, TH_STATE State, uint8_t Speed);
void notifyThrottleDirection(uint8_t UserData, TH_STATE State, uint8_t Direction);
void notifyThrottleFunction(uint8_t UserData, uint8_t Function, uint8_t Value);

#endif // LNINTERFACE_H