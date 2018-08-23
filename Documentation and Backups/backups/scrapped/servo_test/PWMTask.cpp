#include <Timer.h>

#include <stdlib.h>
#include <stdint.h>

class PWMTask {
  private:
    bool (*trigger)(void);
    uint16_t current;
    uint16_t target;
    uint16_t step_size;

    Timer t;
    long wait_time;
    bool done;
    
    PWMTask *next;

    void setDone() {done = true;}
  public:
    PWMTask(bool (*trigger)(), uint16_t target, uint16_t step_size, long wait_time) {
      this->trigger = trigger;
      this->target = target;
      this->step_size = step_size;
      this->wait_time = wait_time;
      done = false;
    }
    void startAt(uint16_t current) {
      this->current = current;
    }
    uint16_t getPWMVal() {
      if ((trigger || trigger()) && target != current) {
          if (abs(target - current) <= abs(step_size)) {
            current = target;
            t.after(wait_time, trigger);
          } else {
            current += step_size * (current < target) ? 1 : -1;
          }
        }
      return current;
    }
    bool isDone() {
      return done;
    }
    PWMTask *getNext() {
      return next;
    }
    void appendTask(PWMTask *task) {
      if (next) next->appendTask(task);
      else next = task;
    }
    void resetAll() {
      current = 0;
      done = false;
      if (next) next->resetAll();
    }
};

