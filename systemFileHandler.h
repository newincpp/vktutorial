#pragma once
#include "systemBalancer.h"
#include "fileHandler.h"

class SystemFileHandler : public System {
private:
    FileHandler _fh;
public:
    SystemFileHandler();
};
