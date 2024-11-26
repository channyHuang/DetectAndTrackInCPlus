#pragma once

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

#include "libExport.h"

class YoloManager {
public:
    static YoloManager *getInstance() {
        if (m_pInstance == nullptr) {
            m_pInstance = new YoloManager();
        }
        return m_pInstance;
    }
    ~YoloManager();

    bool init(const char* pModelName, int nWidth, int nHeight);
    void detect(const cv::Mat& frame, stDetectResult& stResult);

private:
    YoloManager();

    cv::Mat fillFrameToModelSize(const cv::Mat& frame) ;

private:
    static YoloManager *m_pInstance;

    cv::dnn::Net net;
    bool bEnableCuda = false;
    int m_nModelWidth = 1920, m_nModelHeight = 1080;

    float m_fConfidenceThreshold = 0.005;
    float m_fScoreThreshold = 0.45;
    float m_fNMSThreshold = 0.50;

    std::vector<std::string> classes{"person", "bicycle", "car", "motorcycle", "airplane", "bus", "train", "truck", "boat", "traffic light", "fire hydrant", "stop sign", "parking meter", "bench", "bird", "cat", "dog", "horse", "sheep", "cow", "elephant", "bear", "zebra", "giraffe", "backpack", "umbrella", "handbag", "tie", "suitcase", "frisbee", "skis", "snowboard", "sports ball", "kite", "baseball bat", "baseball glove", "skateboard", "surfboard", "tennis racket", "bottle", "wine glass", "cup", "fork", "knife", "spoon", "bowl", "banana", "apple", "sandwich", "orange", "broccoli", "carrot", "hot dog", "pizza", "donut", "cake", "chair", "couch", "potted plant", "bed", "dining table", "toilet", "tv", "laptop", "mouse", "remote", "keyboard", "cell phone", "microwave", "oven", "toaster", "sink", "refrigerator", "book", "clock", "vase", "scissors", "teddy bear", "hair drier", "toothbrush"};
};
