#include "libExport.h"

#include <fstream>
#include "Detector.h"

extern "C"

bool init(const char* pModelString)
{
    return Detector::getInstance()->init(pModelString);
}

stDetectResult* detect(char* pChar, int nWidth, int nHeight)
{
    return Detector::getInstance()->detect(pChar, nWidth, nHeight);
}

void track(stDetectResult* stResult) {
    Detector::getInstance()->track(stResult);
}

void setCallback(CBFun_Callback pFunc, void *pUser)
{
    Detector::getInstance()->setCallback(pFunc, pUser);
}

bool deinit()
{
    return Detector::getInstance()->deinit();
}

void setThreshold(float fThreshold) {
    Detector::getInstance()->setThreshold(fThreshold);
}

void setClassNum(int nClassNum) {
    Detector::getInstance()->setClassNum(nClassNum);
}

// 通用接口
// 设置输出branch数
void setBranchNum(int nBranchNum) {
    Detector::getInstance()->setBranchNum(nBranchNum);
}
// 设置根目录
void setRootDirectory(const char* pChar) {
    Detector::getInstance()->setRootDirectory(pChar);
}