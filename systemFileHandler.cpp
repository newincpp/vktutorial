#include <iostream>
#include "systemFileHandler.h"

SystemFileHandler::SystemFileHandler() : System("fileHandler") {
    TaskList& el = _jobList.emplace_back(TaskList::PacketType::Event, this);
    el._tasklistIterator = _jobList.end()--;
    TaskList& tl = _pendingTasks.emplace_back(TaskList::PacketType::Tick, this);
    tl._tasklistIterator = _pendingTasks.end()--;
    
    // should begin to watch then poll events
    el._succesor.emplace_back(&tl);
    tl._unsatisfiedDeps++;
    
    el._taskPackets.emplace_back([this](std::ofstream& logfile){ 
#ifdef DEBUG
		logfile << "watching" << std::endl; 
#endif
		_fh.watch("/tmp/lol"); }, 0.1f, &el);
    tl._taskPackets.emplace_back([this](std::ofstream& logfile){ 
#ifdef DEBUG
		logfile << "pollEvent" << std::endl; 
#endif
		_fh.pollEvent(); }, 0.1f, &tl);

    // this is creating the concept of "tick" by design
    // would be cool to encapsulate this into easy to understand function
    _startEndTickPoint._succesor.emplace_back(&tl);
    tl._succesor.emplace_back(&_startEndTickPoint);
    el._jobsDone = el._taskPackets.size();
    tl._jobsDone = tl._taskPackets.size();
}
