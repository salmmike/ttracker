#include <ttracker.h>
#include <cstring>
#include <fmt/core.h>

namespace ttracker{

void TTracker::parseArgs(int argc, char **argv)
{
    if ((argc == 1) || strcmp(argv[1], "--help") == 0) {
        usage();
        return;
    }

    if (! strcmp(argv[1], "create")) {
        createTask(argc, argv);
    } else if (! strcmp(argv[1], "start")) {
        continueTask(argc, argv);
    } else if (! strcmp(argv[1], "pause")) {
        pause();
    } else if (! strcmp(argv[1], "summary")) {
        summary(argc, argv);
    } else if (! strcmp(argv[1], "task")) {
        task();
    } else {
        usage();
    }
}

void TTracker::usage()
{
    auto usageStr =
    "TTracker: Tool for keeping track of time spent on various tasks.\n"
    "Usage:\n"
    "ttracker create [taskname]\n"
    "   Add a new task.\n\n"
    "ttracker start [taskname]\n"
    "   Start or continue working on task. If no name is set, continue on"
    " previous task.\n\n"
    "ttracker pause\n"
    "   Pause working on current task\n\n"
    "ttracker summary [--since time] [taskname]\n"
    "   Get summary of task. Available values for since: d (days), h (hours)\n"
    "   If no task name is provided, print summary of all tasks.\n\n"
    "ttracker task\n"
    "   print current task.\n";

    std::cout << usageStr;
}

void TTracker::continueTask(int argc, char** argv)
{
    std::string task;
    if (argc > 2) {
        task = argv[2];
        if (!database->checkExists(task)) {
            std::cout << "No task " << task << ". Create it first!\n";
            return;
        }
    } else {
        task = database->currentTask();
    }
    database->startTask(task);
    std::cout << "Started task \"" << task << "\"\n";
}

void TTracker::createTask(int argc, char** argv)
{
    if (argc <= 2) {
        std::cout << "No task name defined!\n";
    }
    database->createTask(argv[2]);
    std::cout << "Created task " << argv[2] << "\n";
}

void TTracker::makeSummary(std::string task, time_t since)
{
    if (!database->checkExists(task)) {
        std::cout << "No task named \"" << task << "\"\n";
        return;
    }
    auto timeSpent = database->getTime(task, since);
    if (timeSpent < 30) {
        return;
    }
    if (timeSpent > 60*60) {
        std::cout << "\"" << task << "\" worked on for " <<
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

void TTracker::pause()
{
    if (database->checkWorking(database->currentTask())) {
        std::cout << "Paused task \"" << database->pauseTask() << "\"\n";
    } else {
        std::cout << "Task \"" << database->currentTask() << "\" is paused.\n";
    }
}

void TTracker::summary(int argc, char** argv)
{
    if ( (argc == 2) || (!strcmp(argv[2], "--since")) ) {
        time_t since = 0;
        if (argc == 4) {
            since = parseTime(argv[3]);
        }

        auto tasks = database->allTasks(since);
        for (auto task : tasks) {
            makeSummary(task, since);
        }
    } else {
        if (argc >= 5) {
            makeSummary(argv[4], parseTime(argv[3]));
        } else if (argc == 3) {
            makeSummary(argv[2], 0);
        } else {
            usage();
        }
    }
}

}