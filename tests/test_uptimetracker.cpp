#include <gtest/gtest.h>
#include "core/monitoring/uptimetracker.h"

TEST(UptimeTrackerTest, DefaultUptimeIs100)
{
    UptimeTracker tracker(10);
    EXPECT_DOUBLE_EQ(tracker.uptimePercent("svc1"), 100.0);
}

TEST(UptimeTrackerTest, AllUpIs100Percent)
{
    UptimeTracker tracker(10);
    for (int i = 0; i < 10; ++i)
        tracker.record("svc1", true);
    EXPECT_DOUBLE_EQ(tracker.uptimePercent("svc1"), 100.0);
}

TEST(UptimeTrackerTest, AllDownIs0Percent)
{
    UptimeTracker tracker(10);
    for (int i = 0; i < 10; ++i)
        tracker.record("svc1", false);
    EXPECT_DOUBLE_EQ(tracker.uptimePercent("svc1"), 0.0);
}

TEST(UptimeTrackerTest, HalfUpIs50Percent)
{
    UptimeTracker tracker(10);
    for (int i = 0; i < 5; ++i)
        tracker.record("svc1", true);
    for (int i = 0; i < 5; ++i)
        tracker.record("svc1", false);
    EXPECT_DOUBLE_EQ(tracker.uptimePercent("svc1"), 50.0);
}

TEST(UptimeTrackerTest, RollingWindowDropsOldData)
{
    UptimeTracker tracker(5);
    // Fill with downs
    for (int i = 0; i < 5; ++i)
        tracker.record("svc1", false);
    // Fill with ups — should push out the downs
    for (int i = 0; i < 5; ++i)
        tracker.record("svc1", true);
    EXPECT_DOUBLE_EQ(tracker.uptimePercent("svc1"), 100.0);
}

TEST(UptimeTrackerTest, IndependentServiceTracking)
{
    UptimeTracker tracker(10);
    tracker.record("svc1", true);
    tracker.record("svc2", false);
    EXPECT_DOUBLE_EQ(tracker.uptimePercent("svc1"), 100.0);
    EXPECT_DOUBLE_EQ(tracker.uptimePercent("svc2"), 0.0);
}

TEST(UptimeTrackerTest, TotalChecksCount)
{
    UptimeTracker tracker(10);
    tracker.record("svc1", true);
    tracker.record("svc1", true);
    tracker.record("svc1", false);
    EXPECT_EQ(tracker.totalChecks("svc1"), 3);
}

TEST(UptimeTrackerTest, RemoveServiceClearsData)
{
    UptimeTracker tracker(10);
    tracker.record("svc1", false);
    tracker.removeService("svc1");
    EXPECT_DOUBLE_EQ(tracker.uptimePercent("svc1"), 100.0);
}
