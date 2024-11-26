#include "Detector.h"
#include "YoloManager.h"
#include "TrackerManager.h"

Detector::Detector() {}

Detector* Detector::m_pInstance = nullptr;

// 释放资源
Detector::~Detector() {
    release();
    if (m_pInstance != nullptr) {
        delete m_pInstance;
    }
    m_pInstance = nullptr;
}

// 初始化，输入权重模型路径名称，加载模型
bool Detector::init(const char* pModelName) {
    return YoloManager::getInstance()->init(pModelName, 640, 640);
}

bool Detector::deinit() {
    release();
    return true;
}
// 释放传输到外部的检测结果资源
void resetStDetectResult(stDetectResult &result) {
    if (result.nDetectNum == 0) return;
    result.nDetectNum = 0;
    delete []result.pBoxes;
    delete []result.pProb;
    delete []result.pClasses;
}

// 检测，输入图像数据及图像宽高，输出检测信息
stDetectResult* Detector::detect(char* pChar, int nWidth, int nHeight) {
    if (nWidth <= 0 || nHeight <= 0) return nullptr;
    cv::Mat frame(nHeight, nWidth, CV_8UC3, pChar);
    // 填充传输到外部的检测结果
    resetStDetectResult(stResult);
    YoloManager::getInstance()->detect(frame, stResult);
    
    // 画框和概率
    char text[256];
    for (int i = 0; i < stResult.nDetectNum; i++) {
        int x1 = stResult.pBoxes[i << 2];
        int y1 = stResult.pBoxes[(i << 2) + 1];
        int x2 = stResult.pBoxes[(i << 2) + 2];
        int y2 = stResult.pBoxes[(i << 2) + 3];

        cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), 3);

        sprintf(text, "[%d]%.1f%%", stResult.pClasses[i], stResult.pProb[i] * 100);
        cv::putText(frame, text, cv::Point(x1, y1 - 20), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2, 0);
    }

    return &stResult;
}

void Detector::track(stDetectResult* stResult) {
    TrackerManager::getInstance()->updateObjects(stResult);
    TrackerManager::getInstance()->track();
}

// 设置异步检测的回调函数 
void Detector::setCallback(CBFun_Callback pFunc, void* pUser) {
    m_pCallback = pFunc;
    m_pUser = pUser;
    if (pFunc == nullptr && m_bRunning) {
        stop();
        m_bRunning = false;
    } else if (pFunc != nullptr && !m_bRunning) {
        run();
        m_bRunning = true;
    }
    writeLog(pFunc == nullptr ? "setCallback pFunc is nullptr" : "setCallback");
}

void Detector::release() {
   
}

void Detector::threadLoop(std::future<void> exitListener) {
    do {
        

        if (m_pCallback != nullptr) {
            m_pCallback(&stResult, nullptr);
        }
    } while (exitListener.wait_for(std::chrono::microseconds(1)) == std::future_status::timeout);
}
// 设置目标检测置信度阈值，即时生效 
void Detector::setThreshold(float threshold) {
    printf("setThreshold %f\n", threshold);
}
// 设置模型对应的目标总类型数
void Detector::setClassNum(int nClassNum) {
    printf("setClassNum %d\n", nClassNum);
}

// 写日志到文件
void Detector::writeLog(const std::string& sMsg) {
    std::ofstream off("liblog.txt", std::ios::app);
    off << sMsg << std::endl;
    off.close();
}

// 设置输出branch数
void Detector::setBranchNum(int nBranchNum) {
    printf("setBranchNum to %d\n", nBranchNum);
}

void Detector::setRootDirectory(const char* pChar) {
}