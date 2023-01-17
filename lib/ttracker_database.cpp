#include <ttracker_database.h>
#include <ttracker_timecalc.h>
#include <cstdlib>
#include <fstream>

namespace ttracker {

TTrackerDatabase::TTrackerDatabase()
{
    std::string home(getenv("HOME"));
    directory = home + "/." + name + "/";
    if (!std::filesystem::exists(directory)) {
        try {
            std::filesystem::create_directory(directory);
        } catch (std::filesystem::filesystem_error &err) {
            std::cerr << err.what() << "\n";
            throw TTrackerException(err.what());
        }
    }

    currentTaskFile = std::filesystem::path(
        directory.string() + "current_task.txt");

    databaseFile = std::filesystem::path(
        directory.string() + "data.db"
    );

    db = std::make_unique<DatabaseDriver>(databaseFile);
}

TTrackerDatabase::~TTrackerDatabase()
{
}

std::string TTrackerDatabase::currentTask()
{
    std::ifstream ifs(currentTaskFile, std::fstream::in);
    std::string task((std::istreambuf_iterator<char>(ifs)),
                      std::istreambuf_iterator<char>());
    ifs.close();

    return task;
}

void TTrackerDatabase::createTask(std::string name)
{
    db->createTask(name);
}

void TTrackerDatabase::startTask(std::string name)
{
    std::string arg = name;
    if (name.empty()) {
        arg = currentTask();
    }

    if (!db->exists(arg)) {
        throw TTrackerException("start task No task named " + arg);
    }

    writeTaskFile(arg);
    dbWriteTaskStart(arg);
}

std::string TTrackerDatabase::pauseTask()
{
    auto task = currentTask();
    if (task.empty()) {
        throw TTrackerException("No task was started!");
    }

    dbWriteTaskPause(task);

    return task;
}

bool TTrackerDatabase::checkExists(const std::string &name)
{
    if (name.empty()) {
        return false;
    }
    return db->exists(name);
}

void TTrackerDatabase::setComment(std::string task, const std::string &comment)
{
    if (task.empty()) {
        task = currentTask();
    }

    db->setComment(task, comment);
}

std::string TTrackerDatabase::getComment(std::string task)
{
    if (task.empty()) {
        task = currentTask();
    }
    return db->getComment(task);
}

time_t TTrackerDatabase::getTime(const std::string &name, time_t since)
{
    if (name.empty()) {
        return TTrackerTimecalc().parseTime(db->getTimeField(currentTask()), since);
    } else {
        return TTrackerTimecalc().parseTime(db->getTimeField(name), since);
    }
}

void TTrackerDatabase::clear()
{
    db->clear();
    writeTaskFile("");
}

std::vector<std::string> TTrackerDatabase::allTasks(time_t since)
{
    return db->getTasks(since);
}

void TTrackerDatabase::writeTaskFile(const std::string &name) const
{
    std::ofstream ofs;
    ofs.open(currentTaskFile);
    ofs << name;
    ofs.close();
}

void TTrackerDatabase::dbWriteTaskStart(const std::string &name)
{
    std::string task = name;
    if (name.empty()) {
        std::cout << name << " is empty!" << std::endl;
        task = currentTask();
    }
    if (task.empty()) {
        throw TTrackerException("dbWriteTaskStart No task named " + task);
    }
    if (!db->working(task)) {
        db->addStart(task);
    } else {
        throw TTrackerException(task + " is already started.\n");
    }
}

void TTrackerDatabase::dbWriteTaskPause(const std::string &name)
{
    if (db->working(name)) {
        db->addEnd(name);
    }
}


}