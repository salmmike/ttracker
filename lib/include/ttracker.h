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
    void startWorking(const std::vector<std::string> &argv);

    /*
    Pause working current task.
    */
    void pause();

    /*
    Create a new task.
    */
    void createTask(const std::vector<std::string> &argv);

    /*
    Continue on task.
    */
    void continueTask(const std::vector<std::string> &argv);


    void summary(const std::vector<std::string> &argv);

    void makeSummary(std::string task, time_t since, bool printNoExists=true);

    time_t parseTime(std::string str);

    void task();

    void version() const;

    void clear(const std::vector<std::string> &argv);

    time_t getSince(const std::vector<std::string> &argv);

};



}


#endif