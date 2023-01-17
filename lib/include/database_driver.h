#ifndef DATABASE_DRIVER_H
#define DATABASE_DRIVER_H

#include <string>
#include <sqlite3.h>
#include <ttracker_exception.h>
#include <vector>

namespace ttracker{

class DatabaseDriver
{
public:
    DatabaseDriver(std::string path);
    ~DatabaseDriver() {};

    void createTask(const std::string &task);
    void setComment(const std::string &task, const std::string &comment);
    std::string getComment(const std::string &task);
    void addStart(const std::string &task);
    void addEnd(const std::string &task);
    bool exists(const std::string &task);
    bool working(const std::string &task);

    void clear();
    std::string getTimeField(const std::string &task);

    std::vector<std::string> getTasks(time_t since=0);

private:
    const std::string path;
    sqlite3* db {nullptr};

    void setPrevT(const std::string &task);
    void open();
    void close();
    std::string time();
    void makeTable();
};

}

#endif
