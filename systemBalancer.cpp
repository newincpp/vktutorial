#include "systemBalancer.h"
#include <fstream>
#include <string>
#include <chrono>
#include <iostream>

#ifdef DEBUG
#include <string>
#endif

TaskPacket::TaskPacket(eastl::function<JOBSIGNATURE>& task_, float budget_, TaskList* tl_) : _task(task_), _budget(budget_), _priority(0), _parent(tl_) {
#ifdef DEDUG
    if (_budget > 10) {
        std::cout << "WARNING: your budget is very high for one task alone\n";
    }
#endif
}

TaskPacket::TaskPacket(eastl::function<JOBSIGNATURE>&& task_, float budget_, TaskList* tl_) : _task(std::forward<eastl::function<JOBSIGNATURE>>(task_)), _budget(budget_),  _priority(0), _parent(tl_) {
}

float TaskPacket::operator()(JOBARGS) {
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
    
    //std::cout << "jobdone " << _parent->_jobsDone << "\n";
    if (_parent->_jobsDone != 0) {
        _task(a0);
        _parent->_jobsDone--;
    } else {
        _parent->tryFinish();
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - startTime).count();
#ifdef DEBUG
    if (time > _budget) {
        std::cout << "WARNING: task out of budget (" << time << ")! please profile!\n";
    }
#endif
    return time;
}

TaskList::TaskList(PacketType t_, System* af_) : _threadIdPreferance(-1), _type(t_), _affiliatedSystem(af_), _tasklistIterator() {
    std::cout << "TaskListCtor; af:" << (size_t)(_affiliatedSystem) << '\n';
}

bool TaskList::shouldStart() {
    return _unsatisfiedDeps == 0;
}

void TaskList::notifyEnd() {
    if (_unsatisfiedDeps > 0) {
	    _unsatisfiedDeps--;
    }
    if (_unsatisfiedDeps == 0) {
        _jobsDone = _succesor.size();
        //System* af = (*_succesor.begin())->_affiliatedSystem;
        System* af = _affiliatedSystem;
        if (af) { // _affiliatedSystem will be null in the case of _startEndTickPoint
            if (_tasklistIterator != af->_pendingTasks.end()) {
                std::cout << "splice !\n";
                af->_jobList.splice(af->_jobList.end(), af->_pendingTasks, _tasklistIterator);
                _tasklistIterator = af->_jobList.end()--;
            } else {
                std::cout << "wrong tasklistIterator from System: " << af->_name.c_str() << std::endl;
            }
        }
    }
}

void TaskList::tryFinish() {
    //std::cout << "try finishing\n";
    if (_jobsDone > 0) {
        return;
    }
    //std::cout << "finished, try running successor\n";
    for (TaskList* s : _succesor) {
        s->notifyEnd();
    }
    if (_type == TaskList::PacketType::Event) {
        //std::cout << "trying to erase from: " << _affiliatedSystem->_name.c_str() << "\n";
        // assuming the task has been executed and so is in _joblist
        //_affiliatedSystem->_jobList.erase(_tasklistIterator);
    }
}

System::System(const eastl::string& name_) : _name(name_), _startEndTickPoint(TaskList::PacketType::SyncPoint, nullptr) {
    std::cout << "systemCtor: " << _name.c_str() << '\n';
}

static int __g_countThread = 0;

SystemBalancer::TaskConsumer::TaskConsumer() : _running(true), 
#ifdef DEBUG
_threadLogFile(std::string("/tmp/ThreadLogLP") + std::to_string(__g_countThread) + std::string(".log")), 
#endif
_execQueue(&_jobBuffer0), _backlogQueue(&_jobBuffer1), _internal(SystemBalancer::TaskConsumer::jobExec, this) {
#ifdef DEBUG
    std::cout << "ThreadConsumerInit" << __g_countThread << std::endl;
	if (!_threadLogFile.good()) {
		std::cerr << "thread: " << _id << "do not have any log file\n";
	}
    _id = __g_countThread;
#endif
    __g_countThread++;
}

SystemBalancer::TaskConsumer::~TaskConsumer() {
    std::cout << "TaskConsumer Deletion" << std::endl;
}

void SystemBalancer::TaskConsumer::jobExec(TaskConsumer* ts_) {
	std::ofstream log;
	while (ts_->_running) {
		//for (eastl::list<TaskPacket::JobType>::iterator ctask = ts_->_execQueue->begin(); ctask != ts_->_execQueue->end(); ++ctask) {
			//(*ctask)(ts_->_threadLogFile);
		//for (TaskPacket::JobType& ctask : *(ts_->_execQueue)) {
		for (TaskPacket& ctask : *(ts_->_execQueue)) {
#ifdef DEBUG
			ctask(ts_->_threadLogFile);
#else
			ctask(log);
#endif
		}
        // TODO: should lock main thread on any access to this cpu
		ts_->_execQueue->clear();
		std::swap(ts_->_execQueue, ts_->_backlogQueue);
		//std::cout << "thread: " << ts_->_id << "cleared\n";
		// should unlock it
	}
}

void SystemBalancer::TaskConsumer::join() {
	_internal.join();
}

SystemBalancer::SystemBalancer() : _budgetPerSystem(2.0), _cpus(std::thread::hardware_concurrency()), _startEndTickPoint(TaskList::PacketType::SyncPoint, nullptr) {
	std::cout << "System Balancer initialising\n";
	unsigned int concurentThreadsSupported = std::thread::hardware_concurrency();
	std::cout << "supported Threads: " << concurentThreadsSupported << '\n';
}

SystemBalancer::~SystemBalancer() {
	for (TaskConsumer& t : _cpus) {
		t._running = false;
	}
	for (TaskConsumer& t : _cpus) {
		t.join();
	}
}

void SystemBalancer::registerSystem(System* s_) {
	_systemList.push_back(s_);
	//_startEndTickPoint._succesor.emplace_back(&(*s_->_pendingTasks.begin()));
	_startEndTickPoint._succesor.emplace_back(&(s_->_startEndTickPoint));
	s_->_startEndTickPoint._succesor.emplace_back(&_startEndTickPoint);
    
    std::cout << "registering system" << s_->_name.c_str() << "\n";
    for (TaskList* t : s_->_startEndTickPoint._succesor) {
        std::cout << "\tSuccessor; af:" << (size_t)(t->_affiliatedSystem) << " succesor: " << t->_succesor.size();
        if (t->_type == TaskList::PacketType::SyncPoint) {
            std::cout << " type: SyncPoint\n";
        } if (t->_type == TaskList::PacketType::Event) {
            std::cout << " type: Event\n";
        } if (t->_type == TaskList::PacketType::Tick) {
            std::cout << " type: Tick\n";
        }
    }
}

SystemBalancer::TaskConsumer& SystemBalancer::selectCpu() {
	SystemBalancer::TaskConsumer* best = &_cpus[0];
	for (SystemBalancer::TaskConsumer& c : _cpus) {
		if ((c._backlogQueue->size() + c._execQueue->size()) < (best->_backlogQueue->size() + best->_execQueue->size())) {
			best = &c;
		}
	}
	return *best;
}

void SystemBalancer::run() {
	while (_systemList.size()) {
		_startEndTickPoint._jobsDone = 0;
		_startEndTickPoint.tryFinish();
		for (System* s : _systemList){
			exec(s); // execute next tick
		}
	}
}

void SystemBalancer::exec(System* s_) {
    for (TaskList& jobs : s_->_jobList) {
        for (TaskPacket& event : jobs._taskPackets) {
            selectCpu()._backlogQueue->push_back(event);
        }
    }
}
