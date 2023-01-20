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

#include <ttracker.h>
#include <cstring>
#include <fmt/core.h>
#include <algorithm>

#define xstr(a) str(a)
#define str(a) #a
#define VERSION xstr(TTRACKER_VERSION)

namespace ttracker{

void TTracker::parseArgs(int argc, char **argv)
{
    if ((argc == 1) || strcmp(argv[1], "--help") == 0) {
        usage();
        return;
    }
    std::vector<std::string> args(argc - 1);
    for (int i = 1; i < argc; ++i) {
        args[i - 1] = argv[i];
    }

    if (! strcmp(argv[1], "create")) {
        createTask(args);
    } else if (! strcmp(argv[1], "start")) {
        continueTask(args);
    } else if (! strcmp(argv[1], "pause")) {
        pause();
    } else if (! strcmp(argv[1], "summary")) {
        summary(args);
    } else if (! strcmp(argv[1], "task")) {
        task();
    } else if (! strcmp(argv[1], "--version")) {
        version();
    } else if (! strcmp(argv[1], "clear")) {
        clear(args);
    } else {
        usage();
    }
}

void TTracker::usage()
{
    auto usageStr =
    "TTracker: Tool for keeping track of time spent on various tasks.\n"
    "version: " VERSION
    "\nUsage:\n\n"
    "  ttracker create [taskname]\n"
    "    Add a new task.\n\n"
    "  ttracker start [taskname]\n"
    "    Start or continue working on task. If no name is set, continue on"
    " previous task.\n\n"
    "  ttracker pause\n"
    "    Pause working on current task\n\n"
    "  ttracker summary [--since time] [taskname]\n"
    "    Get summary of task. Available values for since: d (days), h (hours)\n"
    "    If no task name is provided, print summary of all tasks.\n\n"
    "  ttracker task\n"
    "    print current task.\n\n"
    "  ttracker clear\n"
    "    Remove all tasks.\n";

    std::cout << usageStr << "\n";
}

void TTracker::continueTask(const std::vector<std::string> &argv)
{
    std::string task;
    if (argv.size() >= 2) {
        task = argv[1];
        if (!database->checkExists(task)) {
            std::cout << "No task \"" << task << "\". Create it? (y/N):";
            std::string ans;
            getline(std::cin, ans);
            if (ans == "Y" || ans == "y") {
                createTask(argv);
            } else {
                std::cout << "No task created.\n";
                return;
            }
        }
    } else {
        task = database->currentTask();
    }
    database->startTask(task);
    std::cout << "Started task \"" << task << "\"\n";
}

void TTracker::createTask(const std::vector<std::string> &argv)
{
    if (argv.size() <= 1) {
        std::cout << "No task name defined!\n";
    }
    database->createTask(argv[1]);
    std::cout << "Created task \"" << argv[1] << "\"\n";
}

void TTracker::makeSummary(std::string task, time_t since, bool printNoExists)
{
    if (!database->checkExists(task)) {
        if (printNoExists) {
            std::cout << "No task named \"" << task << "\"\n";
        }
        return;
    }
    auto timeSpent = database->getTime(task, since);

    if (timeSpent > 60*60) {
        std::cout << "Task \"" << task << "\" worked on for " <<
            fmt::format("{:.2}", tc->secondsToHours(timeSpent)) << " hours.\n";
    } else {
        std::cout << "Task \"" << task << "\" worked on for " <<
            fmt::format("{:.2}", tc->secondsToMinutes(timeSpent)) << " minutes.\n";
    }
}

time_t TTracker::parseTime(std::string str)
{
    time_t now = tc->getTime();
    if (str.find('d') != std::string::npos) {
        auto pos = str.find('d');
        return now - tc->daysToSeconds(std::stof(str.substr(0, pos), 0));
    } else if (str.find('h') != std::string::npos) {
        auto pos = str.find('h');
        return now - tc->hoursToSeconds(std::stof(str.substr(0, pos), 0));
    }

    throw TTrackerException("Wrong timeformat!");
}

void TTracker::task()
{
    auto current = database->currentTask();
    if (current.empty()) {
        std::cout << "No task started.\n";
    } else {
        std::cout << "Current task is \"" << current <<
            "\". State: " << (database->checkWorking(current) ? "working.\n" : "paused.\n");
    }
}

void TTracker::clear(const std::vector<std::string> &argv)
{
    if (argv.size() == 1) {
        std::cout << "This will clear all tasks. Are you sure? (y/N): ";
        std::string ans;
        getline(std::cin, ans);
        if (ans == "Y" || ans == "y") {
            database->clear();
            std::cout << "All tasks cleard!\n";
        }
    }
}

time_t TTracker::getSince(const std::vector<std::string> &argv)
{
    auto timeit = std::find(std::begin(argv), std::end(argv), "--since");

    if (timeit == std::end(argv) || (timeit + 1) == std::end(argv)) {
        return 0;
    }
    return parseTime(*(timeit + 1));
}

void TTracker::version() const
{
    std::cout << "Version: " << VERSION << std::endl;
}

void TTracker::pause()
{
    if (database->checkWorking(database->currentTask())) {
        std::cout << "Paused task \"" << database->pauseTask() << "\"\n";
    } else {
        std::cout << "Task \"" << database->currentTask() << "\" is paused.\n";
    }
}

void TTracker::summary(const std::vector<std::string> &argv)
{
    time_t since = getSince(argv);

    if (argv.size() == 1 || argv.size() == 3) {
        for (auto task : database->allTasks(since)) {
            makeSummary(task, since);
        }
    } else if (argv.size() == 2) {
        makeSummary(argv[1], since);
    } else {
        for (auto &str : argv) {
            makeSummary(str, since, false);
        }
    }
}

}