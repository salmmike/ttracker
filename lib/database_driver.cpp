#include <database_driver.h>
#include <ttracker_timecalc.h>
#include <chrono>
#include <algorithm>
#include <sstream>

namespace ttracker{

static int getBoolean(void* input, int argc, char** argv, char** column)
{
    (void) column;
    bool* res = static_cast<bool*>(input);
    if (argc) {
        *res = (argv[0][0] == '1' || argv[0][0] == 1);
    }
    return 0;
}

static int getString(void* input, int argc, char** argv, char** column)
{
    (void) column;
    std::string* res = static_cast<std::string*>(input);
    if (argc == 1) {
        *res = argv[0];
    } else {
        for (int i = 0; i < argc; ++i) {
            *res += argv[i];
        }
    }

    return 0;
}

static int getStrings(void* input, int argc, char** argv, char** column)
{
    (void) column;
    std::vector<std::string>* res = static_cast<std::vector<std::string>*>(input);
    for (int i = 0; i < argc; ++i) {
        res->push_back(argv[i]);
    }
    return 0;
}



std::string escapeQuote(const std::string &str)
{
    auto cpy = str;
    std::replace(begin(cpy), end(cpy), '\'', '`');
    return cpy;
}

DatabaseDriver::DatabaseDriver(std::string path): path(path)
{
    makeTable();
}

void DatabaseDriver::createTask(const std::string &task)
{
    open();
    auto str =
    "INSERT OR IGNORE INTO TASKS (TASK, WORKINFO, COMMENT, WORKING)"
    " values ('" + escapeQuote(task)
    + "', 's;', 'COMMENT:', '0');";
    char* err = nullptr;
    int ern = sqlite3_exec(db, str.c_str(), NULL, 0, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }
    setPrevT(task);
}

void DatabaseDriver::setComment(const std::string &task, const std::string &comment)
{
    open();
    auto str =
    "UPDATE TASKS SET COMMENT = COMMENT || '\n" + escapeQuote(comment)
    + "' WHERE TASK = '" + escapeQuote(task) + "';";
    char* err = nullptr;
    int ern = sqlite3_exec(db, str.c_str(), NULL, 0, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }
}

std::string DatabaseDriver::getComment(const std::string &task)
{
    open();
    auto str =
    "SELECT COMMENT FROM TASKS WHERE TASK = '" + escapeQuote(task) + "';";
    std::string res;
    char* err = nullptr;
    int ern = sqlite3_exec(db, str.c_str(), getString, &res, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }
    return res;
}


void DatabaseDriver::addStart(const std::string &task)
{
    open();
    auto str =
    "UPDATE TASKS SET WORKINFO = WORKINFO || '" + time() + "', "
    + "WORKING = '1' "
    + "WHERE TASK = '" + escapeQuote(task) + "';";

    char* err = nullptr;
    int ern = sqlite3_exec(db, str.c_str(), NULL, 0, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }
    setPrevT(task);
}

void DatabaseDriver::addEnd(const std::string &task)
{
    open();
    auto str =
    "UPDATE TASKS SET WORKINFO = WORKINFO || ':" + time() + ";', "
    "WORKING = '0' "
    "WHERE TASK='" + escapeQuote(task) + "';";

    char* err = nullptr;
    int ern = sqlite3_exec(db, str.c_str(), NULL, 0, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }
    setPrevT(task);
}


bool DatabaseDriver::exists(const std::string &task)
{
    if (task.empty()) {
        return false;
    }
    open();
    auto str =
    "SELECT EXISTS(SELECT 1 FROM TASKS WHERE TASK='" + escapeQuote(task) +"' );";

    bool res = false;

    char* err = nullptr;
    int ern = sqlite3_exec(db, str.c_str(), getBoolean, &res, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }

    return res;
}

bool DatabaseDriver::working(const std::string &task)
{
    open();
    auto str =
    "SELECT WORKING FROM TASKS WHERE TASK='" + escapeQuote(task) +"';";

    bool res = false;
    char* err;
    int ern = sqlite3_exec(db, str.c_str(), getBoolean, &res, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }

    return res;
}

void DatabaseDriver::clear()
{
    open();
    auto str =
    "DROP TABLE TASKS;";

    char* err = nullptr;
    int ern = sqlite3_exec(db, str, NULL, 0, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }

    makeTable();
}

std::string DatabaseDriver::getTimeField(const std::string &task)
{
    open();
    auto str =
    "SELECT WORKINFO FROM TASKS WHERE TASK='" + escapeQuote(task) + "';";

    char* err = nullptr;
    std::string res;
    int ern = sqlite3_exec(db, str.c_str(), getString, &res, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }
    return res;
}

std::vector<std::string> DatabaseDriver::getTasks(time_t since)
{
    std::stringstream sinceStr;
    sinceStr << since;
    open();
    auto str =
    "SELECT TASK FROM TASKS WHERE PREVT > '" +  sinceStr.str() + "';";

    std::vector<std::string> res;
    char* err = nullptr;
    int ern = sqlite3_exec(db, str.c_str(), getStrings, &res, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }

    return res;
}

void DatabaseDriver::setPrevT(const std::string &task)
{
    open();
    auto str =
    "UPDATE TASKS SET PREVT = '" + time() + "' "
    "WHERE TASK == '" + escapeQuote(task) + "';";

    char* err = nullptr;
    int ern = sqlite3_exec(db, str.c_str(), NULL, 0, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }
}

void DatabaseDriver::open()
{
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        throw TTrackerException("Database " + path + " didn't open,");
    }
}

void DatabaseDriver::close()
{
    if (db) {
        sqlite3_close(db);
    }
}

std::string DatabaseDriver::time()
{
    return TTrackerTimecalc().timeStr();
}

void DatabaseDriver::makeTable()
{
    open();
    auto str =
    "CREATE TABLE IF NOT EXISTS TASKS("
    "TASK       TEXT UNIQUE, "
    "PREVT      INTEGER, "
    "WORKINFO   TEXT, "
    "COMMENT    TEXT, "
    "WORKING    TEXT"
    ");";

    char* err = nullptr;
    int ern = sqlite3_exec(db, str, NULL, 0, &err);

    close();
    if (ern != SQLITE_OK) {
        auto exept = TTrackerException(err);
        sqlite3_free(err);
        throw exept;
    }
}
}