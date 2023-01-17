#ifndef TTRACKER_TIMECALC_H
#define TTRACKER_TIMECALC_H

#include <string>
#include <chrono>

namespace ttracker{

class TTrackerTimecalc
{
public:
    TTrackerTimecalc() {};
    ~TTrackerTimecalc() {};

    time_t stringToTime(const std::string &str);
    time_t parseTime(std::string str, time_t from=0);

    time_t getTime(time_t from=0);
    std::string timeStr(time_t from=0);

    time_t daysToSeconds(float days);
    time_t hoursToSeconds(float hours);

    float secondsToHours(time_t seconds) { return static_cast<float>(seconds) / (60.0 * 60.0); };
    float secondsToMinutes(time_t seconds) { return static_cast<float>(seconds) / (60.0); };
    float secondsToDays(time_t seconds) { return static_cast<float>(seconds) / (60.0*60.0*24.0); };

private:

};

}

#endif