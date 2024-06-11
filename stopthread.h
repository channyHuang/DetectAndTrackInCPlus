
#ifndef STOPTHREAD_H
#define STOPTHREAD_H

#include <chrono>
#include <mutex>
#include <future>
#include <thread>

class StopThread {
public:
    StopThread() {}
    ~StopThread() {}

    void run() {
        mThread = std::thread([this]() {
            this->threadLoop(this->mExitSignal.get_future());
        });
        mThread.detach();
    }

    void stop() {
        mExitSignal.set_value();
    }

private:
    std::thread mThread;
    std::promise<void> mExitSignal;

    virtual void threadLoop(std::future<void> exitListener) {
        do {
            // do something here
        } while (exitListener.wait_for(std::chrono::microseconds(1)) == std::future_status::timeout);
    }
};

#endif
