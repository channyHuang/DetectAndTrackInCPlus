#ifndef DETECTANDTRACK_H
#define DETECTANDTRACK_H

#include <iostream>
#include <vector>
#include <getopt.h>

#include <opencv2/opencv.hpp>

#include "stopthread.h"
#include "inference.h"

using namespace std;
using namespace cv;

struct DetectRect {
    int m_nLeft = 0;
    int m_nTop = 0;
    int m_nWidth = 0;
    int m_nHeight = 0;

    DetectRect() {}
    DetectRect(int nLeft, int nTop, int nWidth, int nHeight) : 
        m_nLeft(nLeft), m_nTop(nTop), m_nWidth(nWidth), m_nHeight(nHeight) {}
};

enum DetectStatus {
    Idle = 0,
    Detect,
    Track
};

class DetectAndTrack : public StopThread {
public:
    static DetectAndTrack* getInstance() {
        if (m_pInstance == nullptr) {
            m_pInstance = new DetectAndTrack();
        }
        return m_pInstance;
    }

    virtual ~DetectAndTrack();

    bool init(char* pVideoUri);
    // status: 0-idle, 1-detect, 2-track
    void setStatus(int nStatus = 0);
    int getStatus();
    bool setRectInterest(int nLeft, int nTop, int nWidth, int nHeight);

private:
    DetectAndTrack();
    virtual void threadLoop(std::future<void> exitListener);

private:
    static DetectAndTrack* m_pInstance;
    Inference *m_pInf = nullptr;
    cv::VideoCapture mCvCap;
    cv::Mat mCvFrame;
    int m_nStatus = 0;
    DetectRect m_stDetectRect;
};

#endif
