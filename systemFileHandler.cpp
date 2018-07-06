#include <iostream>
#include "systemFileHandler.h"

SystemFileHandler::SystemFileHandler() : System("fileHandler") {
    _jobListEvent.push_back([this](){ _fh.watch("/tmp/lol"); });
    _jobListTick.push_back([this](){ std::cout << "pollEvent"; _fh.pollEvent(); });
}
