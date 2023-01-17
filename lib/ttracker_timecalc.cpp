#include <ttracker_timecalc.h>
#include <ttracker_exception.h>
#include <iostream>
#include <sstream>

time_t ttracker::TTrackerTimecalc::stringToTime(const std::string &str)
{
    if (str.empty()) {
        return 0;
    }
    return std::stol(str);
}

time_t ttracker::TTrackerTimecalc::parseTime(std::string str, time_t from)
{
    size_t pos = str.find(";");

    time_t total = 0;

    while (pos < str.size() - 1) {
        auto wb = pos; // wb = work begin
        auto wbEnd = str.find(":", pos);
        time_t wbT = stringToTime(str.substr(wb + 1, wbEnd - wb - 1));

        pos = str.find(";", pos + 1);
        if (pos == std::string::npos) {
            continue;
        }
        time_t weT = stringToTime(str.substr(wbEnd + 1, pos - wbEnd - 1));

        if (wbT >= from) {
            total += (weT - wbT);
        }
    }
    return total;
}

time_t ttracker::TTrackerTimecalc::getTime(time_t from)
{
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(now).count() - from;
}

std::string ttracker::TTrackerTimecalc::timeStr(time_t from)
{
    auto time = getTime(from);
    std::stringstream s;
    s << time;
    return s.str();
}

time_t ttracker::TTrackerTimecalc::daysToSeconds(float days)
{
    return static_cast<time_t>(days * 24 * 60 * 60);
}

time_t ttracker::TTrackerTimecalc::hoursToSeconds(float hours)
{
    return static_cast<time_t>(hours * 60 * 60);
}
