#include <Adafruit_PWMServoDriver.h>
#include "PWMTask.cpp"

class PWMHandler {
  private:
    uint16_t min_val;
    uint16_t max_val;
    uint16_t offset;  
    uint8_t pin;
    Adafruit_PWMServoDriver *driver;
    PWMTask *root_task;
    PWMTask *current_task;
    
  protected:
    PWMHandler(uint16_t min_val, uint16_t max_val, uint16_t offset, uint8_t pin, Adafruit_PWMServoDriver *driver) {
      if (max_val > 4095) max_val = 4095;
      if (offset > max_val - min_val || min_val > max_val || pin > 15) return;
      this->min_val = min_val;
      this->max_val = max_val;
      this->offset = offset;
      this->pin = pin;
      this->driver = driver;
    }
    
  public:
    setOffsetPWM(uint16_t base_val) {
      base_val = min_val + offset + base_val;
      driver->setPWM(pin, 0, (base_val < max_val) ? base_val : max_val);
    }

    void appendTask(PWMTask *task) {
      if (!root_task) {
        root_task = task;
        current_task = task;
      } else {
        root_task->appendTask(task);
      }
    }

    void runTasks() {
      if (!current_task) {
        return;
      }
      setOffsetPWM(current_task->getPWMVal());
      if (current_task->isDone()) {
        current_task = current_task->getNext();
      }
    }

    void resetTask() {
      current_task = root_task;
      root_task->resetAll();
    }
};

class ServoHandler: public PWMHandler {
  public:
    ServoHandler(uint16_t offset, uint8_t pin, Adafruit_PWMServoDriver *driver) : PWMHandler(130, 640, offset, pin, driver) {}
};

