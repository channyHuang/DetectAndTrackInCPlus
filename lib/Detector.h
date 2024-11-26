#include <iostream>
#include <fstream>
#include <experimental/filesystem>
#include <cstring>
#include <chrono>
#include <queue>
#include <mutex>

namespace fs = std::experimental::filesystem;

#include "libExport.h"
#include "stopthread.h"

// 用于导出的检测类Detector
class Detector : public StopThread {
public:
    static Detector *getInstance() {
        if (m_pInstance == nullptr) {
            m_pInstance = new Detector();
        }
        return m_pInstance;
    }
    ~Detector();

    bool init(const char* pModelName);
    bool deinit();
    stDetectResult* detect(char* pChar, int nWidth = 1920, int nHeight = 1080);
    void setThreshold(float fThreshold = 0.8f);
    void setClassNum(int nClassNum = 13);

    void track(stDetectResult* stResult);
    void setCallback(CBFun_Callback pFunc = nullptr, void* pUser = nullptr);

    // 通用接口
    // 设置输出branch数
    void setBranchNum(int nBranchNum);
    void setRootDirectory(const char* pChar);

private:
    Detector();
    void release();
    void writeLog(const std::string& sMsg);
    // 线程循环主体
    virtual void threadLoop(std::future<void> exitListener);

private:
    static Detector *m_pInstance;

    int ret = 0;
    
    // 传输到外部的检测结果 
    stDetectResult stResult;
    // 回调函数
    CBFun_Callback m_pCallback = nullptr;
    // 回调函数传入的用户数据
    void* m_pUser = nullptr;
    // 线程锁
    std::mutex mutex;
    
    bool m_bRunning = false;
};
