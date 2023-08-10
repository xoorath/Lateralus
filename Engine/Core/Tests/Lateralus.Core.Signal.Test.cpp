#include <gtest/gtest.h>

import Lateralus.Core.Signal;

using namespace std;

namespace Lateralus::Core::Tests
{
TEST(Core_Signal, SignalFiresOnce)
{
    Signal<void()> sig;
    int calls = 0;
    sig += [&calls]() { calls++; };
    EXPECT_EQ(calls, 0);
    sig();
    EXPECT_EQ(calls, 1);
}

TEST(Core_Signal, SignalDoubleSubscribeFiresTwice)
{
    Signal<void()> sig;
    int calls = 0;
    auto func = [&calls]() { calls++; };
    // Subscribing multiple times is supported.
    sig += func;
    sig += func;
    EXPECT_EQ(calls, 0);
    sig();
    EXPECT_EQ(calls, 2);
}

TEST(Core_Signal, UnsubscribeIsUniquePerToken)
{
    Signal<void()> sig;
    int calls = 0;
    auto func = [&calls]() { calls++; };

    auto token = sig += func;
    sig += func;
    EXPECT_EQ(calls, 0);
    sig();
    EXPECT_EQ(calls, 2);
    sig -= token;
    // We expect unsubscribing has no effect on call count
    EXPECT_EQ(calls, 2);
    // this call should only increment calls by 1 because one of the subscriptions was removed
    sig();
    EXPECT_EQ(calls, 3);
}

TEST(Core_Signal, DoubleUnsubscribe)
{
    Signal<void()> sig;
    int calls = 0;
    auto token = sig += [&calls]() { calls++; };
    EXPECT_EQ(calls, 0);
    sig();
    EXPECT_EQ(calls, 1);
    // unsubscribe should have no effect on calls
    sig -= token;
    EXPECT_EQ(calls, 1);
    // nothing to call here. no change expected
    sig();
    EXPECT_EQ(calls, 1);
    // unusbscribe again should be a safe no-op.
    sig -= token;
}

TEST(Core_Signal, PassByCopyIsOncePerCall)
{
    struct CopyTracker
    {
        CopyTracker(int &countRef) : m_CountRef(countRef) { m_CountRef++; }

        CopyTracker(CopyTracker const &other) : m_CountRef(other.m_CountRef) { m_CountRef++; }

        CopyTracker(CopyTracker &&other) : m_CountRef(other.m_CountRef) {}

        ~CopyTracker() = default;

        int &m_CountRef;
    };

    Signal<void(CopyTracker)> sig;
    auto func = [](CopyTracker ct) { (void)ct; };

    sig += func;

    int constructcount = 0;
    CopyTracker ct(constructcount);
    EXPECT_EQ(constructcount, 1);

    sig(ct);
    EXPECT_EQ(constructcount, 2);

    sig(ct);
    EXPECT_EQ(constructcount, 3);

    sig += func;
    sig(ct);
    EXPECT_EQ(constructcount, 5);
}

TEST(Core_Signal, AsyncSubscribeWaitsForInvoke)
{
    Signal<void(), SignalLockMutex> sig;

    constexpr auto sleepTime = 50ms;
    atomic_int counter = 0;

    auto slowInvoke = [&counter]() {
        this_thread::sleep_for(sleepTime);
        counter++;
    };
    sig += slowInvoke;
    EXPECT_EQ(counter, 0);

    atomic_bool threadStarted = false;
    thread t([&sig, &threadStarted]() {
        threadStarted = true;
        sig();
    });
    // wait for thread creation
    while (!threadStarted) {}
    // there is a minimum sleep time > 1ms. We just need to wait for enough time to be confident
    // that slowInvoke is in the middle of a sleep.
    this_thread::sleep_for(1ms);
    // the following should wait for the thread to finish invoking
    sig += []() {};
    // Check that the slowInvoke sleep has completed.
    EXPECT_EQ(counter, 1);
    // This should be effectively redundant, but let's make sure we conclude the thread.
    if (t.joinable())
    {
        t.join();
    }
}

TEST(Core_Signal, AsyncUnsubscribeWaitsForInvoke)
{
    Signal<void(), SignalLockMutex> sig;

    constexpr auto sleepTime = 50ms;
    atomic_int counter = 0;

    auto slowInvoke = [&counter]() {
        this_thread::sleep_for(sleepTime);
        counter++;
    };
    auto token = sig += slowInvoke;
    EXPECT_EQ(counter, 0);

    atomic_bool threadStarted = false;
    thread t([&sig, &threadStarted]() {
        threadStarted = true;
        sig();
    });
    // wait for thread creation
    while (!threadStarted) {}
    // there is a minimum sleep time > 1ms. We just need to wait for enough time to be confident
    // that slowInvoke is in the middle of a sleep.
    this_thread::sleep_for(1ms);
    // the following should wait for the thread to finish invoking
    sig -= token;
    // Check that the slowInvoke sleep has completed.
    EXPECT_EQ(counter, 1);
    // This should be effectively redundant, but let's make sure we conclude the thread.
    if (t.joinable())
    {
        t.join();
    }
}
} // namespace Lateralus::Core::Tests
