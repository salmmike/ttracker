#ifndef TTRACKER_H
#define TTRACKER_H

#include <string>
#include <ttracker_database.h>
#include <ttracker_timecalc.h>
#include <iostream>
#include <memory>


namespace ttracker {

class TTracker
{
public:
    TTracker() {
        database = std::make_unique<TTrackerDatabase>();
        tc = std::make_unique<TTrackerTimecalc>();
    };

    ~TTracker() {};

    void parseArgs(int argc, char** argv);

private:

    std::unique_ptr<TTrackerDatabase> database;
    std::unique_ptr<TTrackerTimecalc> tc;

    /*
    Print usage.
    */
    void usage();

    /*
    Start working on taskName. If empty, continue previous task.
    */
    void startWorking(int argc, char** argv);

    /*
    Pause working current task.
    */
    void pause();

    /*
    Create a new task.
    */
    void createTask(int argc, char** argv);

    /*
    Continue on task.
    */
    void continueTask(int argc, char** argv);


    void summary(int argc, char** argv);

    void makeSummary(std::string task, time_t since);

    time_t parseTime(std::string str);

    void task();

};



}


#endif