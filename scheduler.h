#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <string>
#include <fstream>
#include <regex>
#include "task.h"
#include <iostream>

class Scheduler{
public:
    Scheduler();

    inline void setFilename(const std::string& fn){filename = fn;}

    void createTaskGraph();

    void determineStaticBLevels();

    void scheduleInitialTask();

private:
    int nrOfTasks;
    int nrOfProcessors;

    std::string filename;
    std::vector<Task> tasks;

    std::vector<Task*> readyList;

    std::vector<Processor> processors;

    Task* getSpecificTask(int taskIdx);

    void updateReadyList(Task& rmTask);

    void calcDl();

};


#endif //SCHEDULER_H
