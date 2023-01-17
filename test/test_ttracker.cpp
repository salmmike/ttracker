

#include <gtest/gtest.h>
#include <iostream>
#include <cassert>

#include <ttracker_database.h>
#include <ttracker_timecalc.h>

#define NAME TTrackerTest

#define NONAME "this task doesn't exists name"
#define TESTTASK "test1"


TEST(NAME, test_ttracker_timecalc)
{
    ttracker::TTrackerTimecalc tcalc;
    std::string time("12345123124");
    ASSERT_TRUE(tcalc.stringToTime(time) == 12345123124);

    auto t = tcalc.parseTime("s;0:100;200:300;550:555;");
    ASSERT_TRUE(t == 205) << "T was: " << t;

    t = tcalc.parseTime("s;0:100;200:300;550:555;580");
    ASSERT_TRUE(t == 205) << "T was: " << t;

    t = tcalc.parseTime("s;0:100;200:300;550:555;580", 150);
    ASSERT_TRUE(t == 105) << "T was: " << t;
}

TEST(NAME, test_database_start_no_exists)
{
    ttracker::TTrackerDatabase db;
    db.clear();

    ASSERT_ANY_THROW(db.startTask(NONAME));
}

TEST(NAME, test_database_create)
{
    ttracker::TTrackerDatabase db;
    db.clear();

    try {
        db.createTask(TESTTASK);
        ASSERT_TRUE(db.checkExists(TESTTASK)) << "Check exists failed\n";
        ASSERT_TRUE(db.currentTask() == "");
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
        ASSERT_TRUE(false);
    }

}

TEST(NAME, test_database_start)
{
    ttracker::TTrackerDatabase db;
    try {
    db.clear();
    db.createTask(TESTTASK);
    db.startTask(TESTTASK);

    ASSERT_TRUE(db.currentTask() == TESTTASK);
    } catch (std::exception &e) {
        std::cout << "Exception: " << e.what() << std::endl;
        ASSERT_TRUE(false);
    }
}

TEST(NAME, test_database_pause)
{
    ttracker::TTrackerDatabase db;
    try {
    db.clear();
    db.createTask(TESTTASK);
    db.startTask(TESTTASK);
    db.pauseTask();
    } catch (std::exception &e) {
        std::cout << e.what() << std::endl;
    }

}

TEST(NAME, test_database_comment)
{
    ttracker::TTrackerDatabase db;
    db.clear();
    db.createTask(TESTTASK);

    db.setComment(TESTTASK, "abc");
    auto comment = db.getComment(TESTTASK);
    ASSERT_TRUE(comment == "COMMENT:\nabc") << comment;
    db.setComment(TESTTASK, "def");
    comment = db.getComment(TESTTASK);
    ASSERT_TRUE(comment == "COMMENT:\nabc\ndef") << comment;

    db.createTask(NONAME);

    auto all = db.allTasks();
    ASSERT_TRUE(all.size() == 2);
    db.createTask("test 3");
    all = db.allTasks();
    ASSERT_TRUE(all.size() == 3);
}


int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}