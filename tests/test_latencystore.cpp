#include <gtest/gtest.h>
#include "core/monitoring/latencystore.h"

TEST(LatencyStoreTest, LatestReturnsMinusOneWhenEmpty)
{
    LatencyStore store(5);
    EXPECT_EQ(store.latest("svc1"), -1);
}

TEST(LatencyStoreTest, StoresSingleReading)
{
    LatencyStore store(5);
    store.addReading("svc1", 100);
    EXPECT_EQ(store.latest("svc1"), 100);
}

TEST(LatencyStoreTest, ReturnsCorrectLatest)
{
    LatencyStore store(5);
    store.addReading("svc1", 100);
    store.addReading("svc1", 200);
    store.addReading("svc1", 300);
    EXPECT_EQ(store.latest("svc1"), 300);
}

TEST(LatencyStoreTest, CircularOverwrite)
{
    LatencyStore store(3);
    store.addReading("svc1", 100);
    store.addReading("svc1", 200);
    store.addReading("svc1", 300);
    store.addReading("svc1", 400);
    QVector<int> r = store.readings("svc1");
    EXPECT_EQ(r.size(), 3);
    EXPECT_EQ(store.latest("svc1"), 400);
}

TEST(LatencyStoreTest, AverageCalculation)
{
    LatencyStore store(5);
    store.addReading("svc1", 100);
    store.addReading("svc1", 200);
    store.addReading("svc1", 300);
    EXPECT_DOUBLE_EQ(store.average("svc1"), 200.0);
}

TEST(LatencyStoreTest, MinimumCalculation)
{
    LatencyStore store(5);
    store.addReading("svc1", 300);
    store.addReading("svc1", 100);
    store.addReading("svc1", 200);
    EXPECT_EQ(store.minimum("svc1"), 100);
}

TEST(LatencyStoreTest, MaximumCalculation)
{
    LatencyStore store(5);
    store.addReading("svc1", 300);
    store.addReading("svc1", 100);
    store.addReading("svc1", 200);
    EXPECT_EQ(store.maximum("svc1"), 300);
}

TEST(LatencyStoreTest, IndependentServiceTracking)
{
    LatencyStore store(5);
    store.addReading("svc1", 100);
    store.addReading("svc2", 999);
    EXPECT_EQ(store.latest("svc1"), 100);
    EXPECT_EQ(store.latest("svc2"), 999);
}

TEST(LatencyStoreTest, RemoveServiceClearsData)
{
    LatencyStore store(5);
    store.addReading("svc1", 100);
    store.removeService("svc1");
    EXPECT_EQ(store.latest("svc1"), -1);
}

TEST(LatencyStoreTest, ReadingsInChronologicalOrder)
{
    LatencyStore store(5);
    store.addReading("svc1", 10);
    store.addReading("svc1", 20);
    store.addReading("svc1", 30);
    QVector<int> r = store.readings("svc1");
    EXPECT_EQ(r[0], 10);
    EXPECT_EQ(r[1], 20);
    EXPECT_EQ(r[2], 30);
}
