#ifndef DETECTANDTRACK_H
#define DETECTANDTRACK_H

#include <iostream>
#include <vector>
#include <getopt.h>

#include <opencv2/opencv.hpp>

#include "stopthread.h"
#include "Detect/inference.h"
#include "Track/BYTETracker.h"

using namespace std;
using namespace cv;

struct DetectRect {
    int m_nLeft = 30;
    int m_nTop = 30;
    int m_nWidth = 50;
    int m_nHeight = 50;
    float m_fCenterX = 0;
    float m_fCenterY = 0;

    DetectRect() {}
    DetectRect(int nLeft, int nTop, int nWidth, int nHeight) : 
        m_nLeft(nLeft), m_nTop(nTop), m_nWidth(nWidth), m_nHeight(nHeight) {
            m_fCenterX = (m_nLeft + m_nWidth / 2.0f);
            m_fCenterY = (m_nTop - m_nHeight / 2.0f);
        }
};

enum DetectStatus {
    Idle = 0,
    Detect,
    Track
};

typedef void (*CBFun_Callback)(std::vector<DetectRect> vDetectRects, void* pUser);

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
    void setCallback(CBFun_Callback pFunc = nullptr, void *pUser = nullptr);
    void setRectInterest(int nLeft, int nTop, int nWidth, int nHeight);

private:
    DetectAndTrack();
    virtual void threadLoop(std::future<void> exitListener);

private:
    static DetectAndTrack* m_pInstance;
    Inference *m_pInf = nullptr;
    DetectRect m_stDetectRect;

    cv::VideoCapture mCvCap;
    cv::Mat mCvFrame;
    int m_nStatus = 0;
    std::string m_sFPS = "30 frames average fps: ";
    float m_fFps = 0;
    int m_nFrameCount = 0;

    CBFun_Callback m_pCallbackFunc = nullptr;
    void* m_pUser = nullptr;

    int m_nFrameRate = 30;
    int m_nFrameBuffer = 30;
    byte_track::BYTETracker m_cTracker;
    float m_fEps = 50;
    
};

#endif
