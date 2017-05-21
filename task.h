#ifndef TASK_H
#define TASK_H

#include <vector>

enum State{
    NOT_READY,
    READY,
    Scheduled,
};

struct Successor{
    int taskIdx = -1;
    int communicationTime = -1;
};

struct Processor{
    int procIdx = -1;
    int startTime = 0;
    int endTime = 0;
    int currentTaskIdx = -1;
    //State state = READY;
};

class Task{
public:
    Task();

    inline void setTaskIdx(int idx){taskIdx = idx;}
    inline int getTaskIdx(){return taskIdx;}

    inline void setComputationTime(int time){computationTime = time;}
    inline int getComputationTime(){return computationTime;}

    inline void setNumberOfPredecessors(int nr){numberOfPredecessors = nr;}
    inline int getNumberOfPredecessors(){return numberOfPredecessors;}

    inline void setStaticBLevel(int sl){staticBLevel = sl;}
    inline int getStaticBLevel(){return staticBLevel;}


    inline void addSuccessorIdx(Successor succ){ successor.push_back(succ);}
    inline std::vector<Successor> getSuccessors(){return successor;}

    inline void addPredecessor(int pred){predecessor.push_back(pred);}
    inline std::vector<int> getPredecessors(){return predecessor;}

    inline void setProcessor(Processor proc){processor = proc;}
    inline Processor getProcessor(){return processor;}

    inline void setStartTime(int time){startTime = time;}
    inline int getStartTime(){return startTime;}

    inline void setEndTime(int time){endTime = time;}
    inline int getEndTime(){return endTime;}

    inline void changeState(State s){state = s;}
    inline State getState(){return state;}

    inline void setDl(int dl){dynamicLevel = dl;}
    inline int getDl(){return dynamicLevel;}

private:
    int taskIdx;
    int computationTime;
    int numberOfPredecessors;

    State state;

    int staticBLevel;

    Processor processor;
    int startTime;
    int endTime;

    int dynamicLevel;

    std::vector<Successor> successor;
    std::vector<int> predecessor;

};

#endif //TASK_H
