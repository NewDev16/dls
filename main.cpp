#include "scheduler.h"

int main(int argc, char *argv[])
{
    if(argc != 3){
        std::cerr << "To few commandline arguments!!!" << std::endl;
    }

    Scheduler scheduler;
    scheduler.setFilename(std::string(argv[1]));
    scheduler.createTaskGraph();
    scheduler.determineStaticBLevels();
    scheduler.scheduleInitialTask();


    return 0;
}
