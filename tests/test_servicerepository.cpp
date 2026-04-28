#include <gtest/gtest.h>
#include <QCoreApplication>
#include "core/servicerepository.h"
#include "core/Service.h"

// Qt requires QCoreApplication for signals
class ServiceRepositoryTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        int argc = 0;
        app = new QCoreApplication(argc, nullptr);
        repo = new ServiceRepository();
    }

    void TearDown() override
    {
        delete repo;
        delete app;
    }

    QCoreApplication *app;
    ServiceRepository *repo;
};

TEST_F(ServiceRepositoryTest, StartsEmpty)
{
    EXPECT_EQ(repo->count(), 0);
}

TEST_F(ServiceRepositoryTest, AddServiceIncreasesCount)
{
    Service s = Service::create("API", "http://localhost:3000");
    repo->addService(s);
    EXPECT_EQ(repo->count(), 1);
}

TEST_F(ServiceRepositoryTest, FindByIdReturnsService)
{
    Service s = Service::create("API", "http://localhost:3000");
    repo->addService(s);
    Service found = repo->findById(s.id);
    EXPECT_EQ(found.id, s.id);
    EXPECT_EQ(found.name, s.name);
}

TEST_F(ServiceRepositoryTest, FindByIdReturnsEmptyForUnknown)
{
    Service found = repo->findById("nonexistent");
    EXPECT_TRUE(found.id.isEmpty());
}

TEST_F(ServiceRepositoryTest, RemoveServiceDecreasesCount)
{
    Service s = Service::create("API", "http://localhost:3000");
    repo->addService(s);
    repo->removeService(s.id);
    EXPECT_EQ(repo->count(), 0);
}

TEST_F(ServiceRepositoryTest, DuplicateNotAdded)
{
    Service s = Service::create("API", "http://localhost:3000");
    repo->addService(s);
    repo->addService(s);
    EXPECT_EQ(repo->count(), 1);
}

TEST_F(ServiceRepositoryTest, UpdateServiceChangesData)
{
    Service s = Service::create("API", "http://localhost:3000");
    repo->addService(s);
    s.latencyMs = 999;
    repo->updateService(s);
    Service updated = repo->findById(s.id);
    EXPECT_EQ(updated.latencyMs, 999);
}

TEST_F(ServiceRepositoryTest, AllServicesReturnsAll)
{
    Service a = Service::create("A", "http://localhost:3000");
    Service b = Service::create("B", "http://localhost:3001");
    repo->addService(a);
    repo->addService(b);
    EXPECT_EQ(repo->allServices().size(), 2);
}
