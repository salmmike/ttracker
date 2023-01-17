#ifndef TTRACKER_EXCEPTION_H
#define TTRACKER_EXCEPTION_H

#include <string>
#include <iostream>

namespace ttracker{

class TTrackerException: public std::exception
{
public:
    TTrackerException(std::string msg): message(msg) {};
    const char* what() {
        return message.c_str();
    }

private:
    const std::string message;
};

}

#endif