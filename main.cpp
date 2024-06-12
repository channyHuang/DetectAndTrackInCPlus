#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

#include "json.hpp"
#include "detectAndTrack.h"

class Caller {
public:
    Caller() {}

    void test(std::vector<DetectRect> vDetectRects) {
        // std::cout << "Caller.test" << std::endl;
        for (auto &rect : vDetectRects) {
            // std::cout << rect.m_nLeft << " " << rect.m_nTop << " " << rect.m_nWidth << " " << rect.m_nHeight << std::endl;
        }
        m_vDetectRects = vDetectRects;
    }

    void run() {
        DetectAndTrack::getInstance()->init("./../data/1945.MP4");

        DetectAndTrack::getInstance()->setStatus(1);
        std::this_thread::sleep_for(std::chrono::seconds(5));

        if (m_vDetectRects.size() > 0) {
            DetectAndTrack::getInstance()->setRectInterest(m_vDetectRects[0].m_nLeft, m_vDetectRects[0].m_nTop, m_vDetectRects[0].m_nWidth, m_vDetectRects[0].m_nHeight);
        }

        std::this_thread::sleep_for(std::chrono::seconds(8));

        DetectAndTrack::getInstance()->setStatus(1);
        std::this_thread::sleep_for(std::chrono::seconds(8));

        DetectAndTrack::getInstance()->setStatus(0);
        std::this_thread::sleep_for(std::chrono::seconds(50));
    }

private:
    std::vector<DetectRect> m_vDetectRects;
};

void caller_func(std::vector<DetectRect> vDetectRects, void* pUser) {
    ((Caller*)pUser)->test(vDetectRects);
}

int main(int argc, char** argv) {
    Caller caller;
    DetectAndTrack::getInstance()->setCallback(caller_func, &caller);
    caller.run();

    return 0;
}