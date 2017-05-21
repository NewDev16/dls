#include "task.h"

Task::Task()
    :state(State::NOT_READY), taskIdx(-1),
      computationTime(-1), numberOfPredecessors(-1),
      staticBLevel(-1), processor(), startTime(-1),
      endTime(-1)
{

}
