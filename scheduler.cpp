#include "scheduler.h"

Scheduler::Scheduler()
    :nrOfTasks(0), nrOfProcessors(0), tasks{}, processors{}
{

}

void Scheduler::createTaskGraph(){
    const std::regex PROCESSING_REGEX("\\s+(\\d+)\\s+(\\d+)");
    const std::regex TASK_REGEX("\\s+(\\d+)\\s+(\\d+)\\s+(\\d+)");
    const std::regex PREDECESSOR_REGEX("\\s+(\\d+)\\s+(\\d+)");

    std::smatch matchResult;

    std::string line;
    std::ifstream file;
    file.open(filename);

    std::getline(file, line);
    std::regex_match(line, matchResult, PROCESSING_REGEX);

    nrOfTasks = std::stoi(matchResult.str(1));
    nrOfProcessors = std::stoi(matchResult.str(2));

    for (int idx = 0; idx < nrOfProcessors; ++idx) {
        processors.push_back(Processor());
        processors.back().procIdx = idx;
        std::cout << "Processor index: " << processors.back().procIdx << std::endl;
    }


    while(std::getline(file,line)){
        if(std::regex_match(line,matchResult, TASK_REGEX)){
            Task t;
            t.setTaskIdx(std::stoi(matchResult.str(1)));
            t.setComputationTime(std::stoi(matchResult.str(2)));
            t.setNumberOfPredecessors(std::stoi(matchResult.str(3)));

            if(t.getNumberOfPredecessors()!=0){
                for (int idx = 0; idx < t.getNumberOfPredecessors(); idx++) {
                    std::getline(file,line);
                    if (std::regex_match(line,matchResult,PREDECESSOR_REGEX)) {
                        int predecessorIdx = std::stoi(matchResult.str(1));
                        t.addPredecessor(predecessorIdx);

                        //provide successor to predecessor task
                        Task* tempT = getSpecificTask(predecessorIdx);
                        Successor tempSucc;
                        tempSucc.taskIdx = t.getTaskIdx();
                        tempSucc.communicationTime = std::stoi(matchResult.str(2));
                        tempT->addSuccessorIdx(tempSucc);
                    }
                }
            }
            tasks.push_back(t);
        }
    }

    for(auto t:tasks){
        std::cout <<"Task idx "<< t.getTaskIdx()+1<< ": "<< t.getSuccessors().size() << std::endl;
    }
    std::cout << std::endl;
}

Task* Scheduler::getSpecificTask(int taskIdx){
    Task* toi;
    for(auto it = tasks.begin(); it !=tasks.end(); it++){
        if(it->getTaskIdx() == taskIdx){
            toi = &(*it);
            break;
        }
    }

    return toi;
}

void Scheduler::determineStaticBLevels(){
    for (int idx = nrOfTasks-1; idx >= 0; idx--) {
        int tempSL = tasks.at(idx).getComputationTime();
        tasks.at(idx).setStaticBLevel(tempSL);
        if(!tasks.at(idx).getSuccessors().empty()){
            for(auto succ: tasks.at(idx).getSuccessors()){
                int succSL = getSpecificTask(succ.taskIdx)->getStaticBLevel();
                if(tempSL + succSL > tasks.at(idx).getStaticBLevel()){
                    tasks.at(idx).setStaticBLevel(tempSL + succSL);
                }
            }
        }
    }
    for(auto t: tasks){
        std::cout << "Task index: " << t.getTaskIdx()+1 << "; sl:" << t.getStaticBLevel() << "; CompTime: " << t.getComputationTime()<< std::endl;
    }
}

void Scheduler::scheduleInitialTask(){
    int dl = 0;

    for(auto it = tasks.begin(); it!=tasks.end();it++){
        if(it->getPredecessors().empty()){
            readyList.push_back(&(*it));
            readyList.back()->changeState(State::READY);
            readyList.back()->setDl(readyList.back()->getStaticBLevel());
        }
    }
    if(readyList.size()>1){
        std::cout << "Before reordering: " << std::endl;

        for(auto task: readyList){
            std::cout << "Task index: " << task->getTaskIdx()+1 << "; dl: " << task->getDl() << std::endl;
        }

        std::sort(readyList.begin(), readyList.end(),[](Task* t1, Task* t2){
            return t1->getDl() > t2->getDl();
        });

        std::cout << "After reordering: " << std::endl;

        for(auto task: readyList){
            std::cout << "Task index: " << task->getTaskIdx()+1 << "; dl: " << task->getDl() << std::endl;
        }
    }

    processors.at(0).startTime = 0;
    processors.at(0).endTime = readyList.at(0)->getComputationTime();
    processors.at(0).currentTaskIdx = readyList.at(0)->getTaskIdx();
    readyList.at(0)->changeState(State::Scheduled);
    readyList.at(0)->setProcessor(processors.at(0));

    updateReadyList(*readyList.at(0));

    calcDl();
}

void Scheduler::updateReadyList(Task& rmTask){
    readyList.erase(std::remove_if(readyList.begin(), readyList.end(), [&rmTask](Task* task){
        return task->getTaskIdx() == rmTask.getTaskIdx();
    }));
    for(auto taskItr = tasks.begin(); taskItr!=tasks.end(); taskItr++){
        State state = taskItr->getState();
        if (state==State::NOT_READY) {
            bool allSuccAreScheduled = true;
            for (auto pred: taskItr->getPredecessors()) {
                if(tasks.at(pred).getState() != State::Scheduled){
                    allSuccAreScheduled = false;
                }
            }
            if (allSuccAreScheduled){
                readyList.push_back(&(*taskItr));
                readyList.back()->changeState(State::READY);
            }
        }
    }
    std::cout <<"Current readyList size: "<< readyList.size()<< std::endl;
}

void Scheduler::calcDl(){
    for(auto rdyTask:readyList){
        int earliestStartTime = 1000000;

        for(auto processor: processors){

            int maxTime = 0;

            for(auto pred:rdyTask->getPredecessors()){
                int tempTime = tasks.at(pred).getProcessor().endTime;
                if(tasks.at(pred).getProcessor().procIdx != processor.procIdx){
                    auto itr = std::find_if(tasks.at(pred).getSuccessors().begin(), tasks.at(pred).getSuccessors().end(),[rdyTask](Successor& succ){
                       return rdyTask->getTaskIdx() == succ.taskIdx;
                    });
                    if(itr != std::end(tasks.at(pred).getSuccessors())){
                        tempTime+=itr->communicationTime;
                    }
                }
                if( tempTime > maxTime){
                    maxTime = tempTime;
                }
            }
            if(maxTime < earliestStartTime){
                earliestStartTime = maxTime;
            }
        }
        //TODO entsprechender Processor mit abspeichern. Am besten in einem Pair oder Tuble
        rdyTask->setDl(rdyTask->getStaticBLevel()-earliestStartTime);
    }

    std::sort(readyList.begin(), readyList.end(), [](Task* t1, Task* t2){
        return t1->getDl() > t2->getDl();
    });

    for(auto task:readyList){
        std::cout << "Task idx: " << task->getTaskIdx()+1 << "; dl: " << task->getDl() << std::endl;
    }
}
