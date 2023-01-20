/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Author: Mike Salmela
 */


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
    } else if (name != currentTask()) {
        if (db->working(currentTask())) {
            std::cout << "Pausing task \"" + currentTask() + "\"\n";
            dbWriteTaskPause(currentTask());
        }
    }

    if (!db->exists(arg)) {
        throw TTrackerException("No task named " + arg);
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

bool TTrackerDatabase::checkWorking(const std::string &name)
{
    return db->working(name);
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
        task = currentTask();
    }
    if (task.empty()) {
        throw TTrackerException("No task named " + task);
    }
    if (!db->working(task)) {
        db->addStart(task);
    } else {
        throw TTrackerException(task + " is already started.");
    }
}

void TTrackerDatabase::dbWriteTaskPause(const std::string &name)
{
    if (db->working(name)) {
        db->addEnd(name);
    }
}


}