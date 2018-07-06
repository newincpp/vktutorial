#pragma once
#include <eathread/eathread.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/list.h>
#include <EASTL/functional.h>

class System {
public:
    eastl::list<eastl::function<void(void)>> _jobListTick;
    eastl::list<eastl::function<void(void)>> _jobListEvent;
    eastl::string _name;
    System(const eastl::string& name);
};

class SystemBalancer {
	class TaskConsumer {
			std::thread _internal;
		public:
			TaskConsumer(unsigned int);
			bool _running;
			eastl::list<eastl::function<void(void)>> _todoQueue;
			static void jobExec(unsigned int, bool*, decltype(_todoQueue)*);
			void join();
	};
	eastl::vector<TaskConsumer> _cpus;
	eastl::vector<System*> _systemList;
	public:
	float _budgetPerSystem; // in ms
	TaskConsumer& selectCpu();
	SystemBalancer();
	~SystemBalancer();
	void registerSystem(System*);
	void run();
	void execEventWithBudget(System*);
	void execTick(System*);
};
