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
}

int DetectAndTrack::getStatus() {
    return m_nStatus;
}

bool DetectAndTrack::setRectInterest(int nLeft, int nTop, int nWidth, int nHeight) {
    m_stDetectRect = DetectRect(nLeft, nTop, nWidth, nHeight);
    m_nStatus = 1;
}

void DetectAndTrack::threadLoop(std::future<void> exitListener) {
    do {
        mCvCap >> mCvFrame;
        if (mCvFrame.empty()) {
            break;
        }
        // Inference starts here...
        std::vector<Detection> output = m_pInf->runInference(mCvFrame);

        int detections = output.size();
        // std::cout << "Number of detections:" << detections << std::endl;

        for (int i = 0; i < detections; ++i)
        {
            Detection detection = output[i];

            cv::Rect box = detection.box;
            cv::Scalar color = detection.color;

            // Detection box
            cv::rectangle(mCvFrame, box, color, 2);

            // Detection box text
            // std::string classString = detection.className + ' ' + std::to_string(detection.confidence).substr(0, 4);
            // cv::Size textSize = cv::getTextSize(classString, cv::FONT_HERSHEY_DUPLEX, 1, 2, 0);
            // cv::Rect textBox(box.x, box.y - 40, textSize.width + 10, textSize.height + 20);

            // cv::rectangle(mCvFrame, textBox, color, cv::FILLED);
            // cv::putText(mCvFrame, classString, cv::Point(box.x + 5, box.y - 10), cv::FONT_HERSHEY_DUPLEX, 1, cv::Scalar(0, 0, 0), 2, 0);
        }
        // Inference ends here...

        // This is only for preview purposes
        float scale = 0.8;
        cv::resize(mCvFrame, mCvFrame, cv::Size(mCvFrame.cols*scale, mCvFrame.rows*scale));
        cv::imshow("Inference", mCvFrame);

        cv::waitKey(1);
        // do something here
    } while (exitListener.wait_for(std::chrono::microseconds(1)) == std::future_status::timeout);
}
