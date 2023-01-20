#ifndef TTRACKER_DATABASE_H
#define TTRACKER_DATABASE_H


#include <string>
#include <filesystem>
#include <memory>
#include <iostream>
#include <database_driver.h>
#include <vector>

#define xstr(a) str(a)
#define str(a) #a
#define TTRACKER_NAME xstr(TTRACKER_NAME_DEF)

namespace ttracker {

class TTrackerDatabase
{
public:
    /*
    Initializes TTracker database files if they don't exists.
    */
    TTrackerDatabase();
    ~TTrackerDatabase();

    /*
    Get current task.
    */
    std::string currentTask();

    /*
    Create a new task with name.
    */
    void createTask(std::string name);

    /*
    Starts task with name. If name is empty, continue current task.
    */
    void startTask(std::string name="");

    /*
    Pauses current task. Return paused task name
    */
    std::string pauseTask();

    /*
    Check that task with name exists.
    */
    bool checkExists(const std::string &name);

    /*
    Append to task comment.
    */
    void setComment(std::string task="", const std::string &comment="");
    std::string getComment(std::string task="");

    /*
    Get time spent on task
    */
    time_t getTime(const std::string &name, time_t since=0);

    /*
    Remove all data.
    */
    void clear();

    bool checkWorking(const std::string &name);

    std::vector<std::string> allTasks(time_t since=0);

private:
    const std::string name {TTRACKER_NAME};
    std::filesystem::path directory;
    std::filesystem::path currentTaskFile;
    std::filesystem::path databaseFile;

    void writeTaskFile(const std::string &name) const;
    void dbWriteTaskStart(const std::string &name);
    void dbWriteTaskPause(const std::string &name);

    std::unique_ptr<DatabaseDriver> db;

};

}

#endif