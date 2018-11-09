#pragma once
#include <eathread/eathread.h>
#include <EASTL/string.h>
#include <EASTL/vector.h>
#include <EASTL/list.h>
#include <EASTL/functional.h>
#include <atomic>
#ifdef DEBUG
#include <fstream>
#endif

#define STDLISTTYPE
#ifdef STDLISTTYPE
#include <list> // eastl::splice doesn't seems to work on non-copiable object e.g. std::atomic
#endif

class System;
class TaskList;
class TaskPacket {
public:
#define JOBARGS std::ofstream& a0
#define JOBSIGNATURE void(JOBARGS)
    typedef eastl::function<JOBSIGNATURE> JobType;
    unsigned int _priority; // 0 = lowest priority
    float _budget; // in ms
    JobType _task;
    TaskList* _parent;
    //std::atomic<int>& _tljobDone;
    
    TaskPacket(JobType&, float, TaskList*);
    TaskPacket(JobType&&, float, TaskList*);
    float operator()(JOBARGS); // return time spend
};

class TaskList {
public:
#ifdef STDLISTTYPE
    std::list<TaskList*> _succesor;
#else
    eastl::list<TaskList*> _succesor;
#endif
    enum PacketType {
        None,
        SyncPoint,
        Tick,
        Event
    };
    TaskList(PacketType, System*);
    TaskList(const TaskList&) = delete;
    TaskList(TaskList&&) = delete;
    PacketType _type;
    std::atomic<size_t> _jobsDone;
    std::atomic<size_t> _unsatisfiedDeps;
    System* _affiliatedSystem;
    // check if wrong threadPreferance
    int _threadIdPreferance; // -1 = don't care
#ifdef STDLISTTYPE
    std::list<TaskPacket> _taskPackets;
    std::list<TaskList>::const_iterator _tasklistIterator;
#else
    eastl::list<TaskPacket> _taskPackets;
    eastl::list<TaskList>::const_iterator _tasklistIterator;
#endif
    void makeTaskPacket();
    inline bool shouldStart();
    void notifyEnd();
    void tryFinish();
};

class System {
public:
#ifdef STDLISTTYPE
    std::list<TaskList> _pendingTasks;
    std::list<TaskList> _jobList;
#else
    eastl::list<TaskList> _pendingTasks;
    eastl::list<TaskList> _jobList;
#endif
    TaskList _startEndTickPoint;
    eastl::string _name;
    System(const eastl::string& name);
    void autoAddTask(TaskList*);
};

class SystemBalancer {
private:
    class TaskConsumer {
#ifdef DEBUG
        std::ofstream _threadLogFile;
        int _id;
#endif
        std::thread _internal;
    public:
        TaskConsumer();
        ~TaskConsumer();
        bool _running;
        eastl::list<TaskPacket>* _execQueue;
        eastl::list<TaskPacket>* _backlogQueue;
        eastl::list<TaskPacket> _jobBuffer0;
        eastl::list<TaskPacket> _jobBuffer1;
        static void jobExec(TaskConsumer*);
        void join();
    };
    TaskList _startEndTickPoint;
    eastl::vector<TaskConsumer> _cpus;
    eastl::vector<System*> _systemList;
public:
    float _budgetPerSystem; // in ms
    TaskConsumer& selectCpu();
    SystemBalancer();
    ~SystemBalancer();
    void registerSystem(System*);
    void run();
    void exec(System*);
};
