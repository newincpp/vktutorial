#include "systemBalancer.h"
#include <chrono>
#include <iostream>

System::System(const eastl::string& name_) : _name(name_) {
}

SystemBalancer::TaskConsumer::TaskConsumer(unsigned int id_) : _running(true), _internal(SystemBalancer::TaskConsumer::jobExec, id_, &_running, &_todoQueue) {
}

void SystemBalancer::TaskConsumer::jobExec(unsigned int id_, bool* running_, eastl::list<eastl::function<void(void)>>* taskQueue) {
	while (*running_) {
		for (eastl::list<eastl::function<void(void)>>::iterator ctask = taskQueue->begin(); ctask != taskQueue->end(); ++ctask) {
			(*ctask)();
			std::cout << "id: " << id_ << '\n';
			taskQueue->erase(ctask);
		}
	}
}

void SystemBalancer::TaskConsumer::join() {
	_internal.join();
}

SystemBalancer::SystemBalancer() : _budgetPerSystem(2.0) {
	std::cout << "System Balancer initialising\n";
	unsigned int concurentThreadsSupported = std::thread::hardware_concurrency();
	std::cout << "supported Threads: " << concurentThreadsSupported << '\n';

	_cpus.reserve(concurentThreadsSupported);
	for (unsigned int i = 0; i < concurentThreadsSupported; ++i) {
		_cpus.emplace_back(i);
	}
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
}

SystemBalancer::TaskConsumer& SystemBalancer::selectCpu() {
	SystemBalancer::TaskConsumer* best = &_cpus[0];
	for (SystemBalancer::TaskConsumer& c : _cpus) {
		if (c._todoQueue.size() < best->_todoQueue.size()) {
			best = &c;
		}
	}
	return *best;
}

void SystemBalancer::run() {
	while (_systemList.size()) {
		for (System* s : _systemList){
			std::cout << "executing: " << s->_name.c_str() << '\n';
			execEventWithBudget(s); // execute as much event as possible until the budget is exeeded
			execTick(s); // execute next tick
		}
	}
}

void SystemBalancer::execEventWithBudget(System* s_) {
	std::chrono::time_point<std::chrono::high_resolution_clock> startTime = std::chrono::high_resolution_clock::now();
	for (eastl::function<void(void)>& event : s_->_jobListEvent) {
		selectCpu()._todoQueue.push_back(event);

		std::chrono::time_point<std::chrono::high_resolution_clock> currentTime = std::chrono::high_resolution_clock::now();
		float time = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - startTime).count();
		if (time > _budgetPerSystem) {
			break;
		}
	}
}

void SystemBalancer::execTick(System* s_) {
	for (eastl::function<void(void)> job : s_->_jobListTick) {
		selectCpu()._todoQueue.push_back(job);
	}
}
