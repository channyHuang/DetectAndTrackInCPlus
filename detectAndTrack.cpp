#include "detectAndTrack.h"

DetectAndTrack* DetectAndTrack::m_pInstance = nullptr;

DetectAndTrack::DetectAndTrack() {

}

DetectAndTrack::~DetectAndTrack() {
    if (m_pInf != nullptr) {
        delete m_pInf;
        m_pInf = nullptr;
    }

    if (m_pInstance != nullptr) {
        delete m_pInstance;
        m_pInstance = nullptr;
    }
}

bool DetectAndTrack::init(char* pVideoUri) {
    if (pVideoUri == nullptr) {
        return false;
    }
    mCvCap.open(pVideoUri);
    if (!mCvCap.isOpened()) {
        printf("Error: open video failed. Please check if video exists\n");
        return false;
    }

    if (m_pInf == nullptr) {
        m_pInf = new Inference("../models/yolov8n_640.onnx", cv::Size(640, 640));
    }
    run();
    return true;
}

void DetectAndTrack::setStatus(int nStatus) {
    m_nStatus = nStatus;
    std::string sStatus = (m_nStatus == 0 ? "idle" : (m_nStatus == 1 ? "Detect" : "Track"));
    std::cout << "setStatus " << sStatus << std::endl;
}

int DetectAndTrack::getStatus() {
    return m_nStatus;
}

void DetectAndTrack::setCallback(CBFun_Callback pFunc, void *pUser) {
    m_pCallbackFunc = pFunc;
    m_pUser = pUser;
}

void DetectAndTrack::setRectInterest(int nLeft, int nTop, int nWidth, int nHeight) {
    m_stDetectRect = DetectRect(nLeft, nTop, nWidth, nHeight);
    m_nStatus = Track;
    std::cout << "setRectInterest (left, top, width, heigth) =" << nLeft << " " << nTop << " " << nWidth << " " << nHeight << std::endl;
}

void DetectAndTrack::threadLoop(std::future<void> exitListener) {
    auto stStartTime = std::chrono::steady_clock::now();
    do {
        mCvCap >> mCvFrame;
        if (mCvFrame.empty()) {
            break;
        }
        m_nFrameCount += 1;

        // Detect
        std::vector<Detection> output = m_pInf->runInference(mCvFrame);
        int detections = output.size();

        std::vector<DetectRect> vDetectOutput;
        std::vector<byte_track::Object> objects;

        for (int i = 0; i < detections; ++i) {
            Detection detection = output[i];
            vDetectOutput.push_back(DetectRect(detection.box.x, detection.box.y, detection.box.width, detection.box.height));

            float x = detection.box.x;
            float y = detection.box.y;
            float width = detection.box.width;
            float height = detection.box.height;
            float conf = detection.confidence;
            objects.emplace_back(byte_track::Rect(x, y, width, height), 0, conf);
        }
        const auto vTrackOutput = m_cTracker.update(objects);
        
        switch(m_nStatus) {
        case Detect:
            for (int i = 0; i < detections; ++i) {
                Detection detection = output[i];
                cv::Rect box = detection.box;
                cv::Scalar color = detection.color;
                cv::rectangle(mCvFrame, box, color, 2);
            }
            if (m_pCallbackFunc != nullptr) {
                m_pCallbackFunc(vDetectOutput, m_pUser);
            }
            break;
        case Track:
        {
            bool bChecked = false;
            for (int i = 0; i < detections; ++i) {
                Detection detection = output[i];
                cv::Rect box = detection.box;
                cv::Scalar color = detection.color;
                cv::rectangle(mCvFrame, box, color, 2);
                if (m_stDetectRect.m_fCenterX >= box.x && m_stDetectRect.m_fCenterY <= box.y &&
                    m_stDetectRect.m_fCenterX <= box.x + box.width && 
                    m_stDetectRect.m_fCenterY >= box.y - box.height) {
                        bChecked = true;
                        m_stDetectRect = DetectRect(box.x, box.y, box.width, box.height);
                        break;
                    }
            }
            if (bChecked == false) {
                for (const auto &output : vTrackOutput)
                {
                    const auto &rect = output->getRect();
                    
                    if (fabs(rect.x() - m_stDetectRect.m_nLeft) < m_fEps && 
                        fabs(rect.y() - m_stDetectRect.m_nTop) < m_fEps && 
                        fabs(rect.width() - m_stDetectRect.m_nWidth) < m_fEps &&
                        fabs(rect.height() - m_stDetectRect.m_nHeight) < m_fEps) {
                            m_stDetectRect = DetectRect(rect.x(), rect.y(), rect.width(), rect.height());
                            break;
                        }
                }
            }
            cv::rectangle(mCvFrame, cv::Rect(m_stDetectRect.m_nLeft, m_stDetectRect.m_nTop, m_stDetectRect.m_nWidth, m_stDetectRect.m_nHeight), cv::Scalar(0, 0, 255), 2);
        }
            break;
        case Idle:
        default:
            break;
        }
        
        if (m_nFrameCount >= 30) {
            auto stEndTime = std::chrono::steady_clock::now();
            float seconds = std::chrono::duration<float>(stEndTime - stStartTime).count();
            m_fFps = m_nFrameCount / seconds;
            stStartTime = std::chrono::steady_clock::now();
            m_nFrameCount = 0;
        }
        // show
        cv::putText(mCvFrame, m_sFPS + std::to_string(m_fFps), cv::Point(30, 30), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(255, 0, 0), 2, 0);
        float scale = 0.8;
        cv::resize(mCvFrame, mCvFrame, cv::Size(mCvFrame.cols*scale, mCvFrame.rows*scale));
        cv::imshow("Inference", mCvFrame);
        cv::waitKey(1);
    } while (exitListener.wait_for(std::chrono::microseconds(1)) == std::future_status::timeout);
}
