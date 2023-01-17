#include <iostream>
#include <ttracker.h>
#include <ttracker_exception.h>

int main(int argc, char* argv[])
{
    ttracker::TTracker t;
    try {
        t.parseArgs(argc, argv);
    } catch (ttracker::TTrackerException &e) {
        std::cout << e.what() << "\n";
    }
    return 0;
}