#include <gtest/gtest.h>
#include "core/Service.h"

TEST(ServiceTest, CreateSetsId)
{
    Service s = Service::create("Auth API", "http://localhost:3000");
    EXPECT_FALSE(s.id.isEmpty());
}

TEST(ServiceTest, CreateSetsName)
{
    Service s = Service::create("Auth API", "http://localhost:3000");
    EXPECT_EQ(s.name, "Auth API");
}

TEST(ServiceTest, CreateSetsUrl)
{
    Service s = Service::create("Auth API", "http://localhost:3000");
    EXPECT_EQ(s.url, "http://localhost:3000");
}

TEST(ServiceTest, HttpUrlDetectedAsHttp)
{
    Service s = Service::create("API", "http://localhost:3000");
    EXPECT_EQ(s.type, ServiceType::Http);
}

TEST(ServiceTest, TcpAddressDetectedAsTcp)
{
    Service s = Service::create("DB", "localhost:5432");
    EXPECT_EQ(s.type, ServiceType::Tcp);
}

TEST(ServiceTest, DefaultStatusIsUnknown)
{
    Service s = Service::create("API", "http://localhost:3000");
    EXPECT_EQ(s.status, ServiceStatus::Unknown);
}

TEST(ServiceTest, DefaultLatencyIsMinusOne)
{
    Service s = Service::create("API", "http://localhost:3000");
    EXPECT_EQ(s.latencyMs, -1);
}

TEST(ServiceTest, StatusStringUnknown)
{
    Service s;
    s.status = ServiceStatus::Unknown;
    EXPECT_EQ(s.statusString(), "Unknown");
}

TEST(ServiceTest, StatusStringUp)
{
    Service s;
    s.status = ServiceStatus::Up;
    EXPECT_EQ(s.statusString(), "Up");
}

TEST(ServiceTest, StatusStringDown)
{
    Service s;
    s.status = ServiceStatus::Down;
    EXPECT_EQ(s.statusString(), "Down");
}

TEST(ServiceTest, StatusStringDegraded)
{
    Service s;
    s.status = ServiceStatus::Degraded;
    EXPECT_EQ(s.statusString(), "Degraded");
}

TEST(ServiceTest, TwoServicesHaveDifferentIds)
{
    Service a = Service::create("A", "http://localhost:3000");
    Service b = Service::create("B", "http://localhost:3001");
    EXPECT_NE(a.id, b.id);
}
