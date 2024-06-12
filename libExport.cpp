#include "libExport.h"

#include <fstream>

extern "C"

void funParamVoidReturnVoid()
{
    std::ofstream off("log.txt", std::ios::app);
    off << "call function funParamVoidReturnVoid" << std::endl;
    off.close();
}

void funParamValidReturnVoid(const char* pString, int nNumber)
{
    std::ofstream off("log.txt", std::ios::app);
    off << "call function funParamValidReturnVoid: input string = " << pString << ", input number = " << nNumber << std::endl;
    off.close();
}

char* funParamVoidReturnValid()
{
    std::ofstream off("log.txt", std::ios::app);
    off << "call function funParamVoidReturnValid" << std::endl;
    off.close();

    char pReturnValue[128];
    memset(pReturnValue, 0, sizeof(pReturnValue));
    //std::string str = Manager::getInstance()->getString();
    std::string str = "hello~";
    memcpy(pReturnValue, str.c_str(), str.length());

    return pReturnValue;
}

int funParamValidReturnValid(const char* pString, int nNumber)
{
    std::ofstream off("log.txt", std::ios::app);
    off << "call function funcParamValidReturnValid: input string = " << pString << ", input number = " << nNumber << std::endl;
    off.close();

    return rand() % 1000009;
}

int funSetNetworkCallback(CBFun_NetworkCallback pFunc, void *pUser)
{
    if (g_pNetworkManager == nullptr) {
        g_pNetworkManager = NetworkManager::getInstance();
    }
    CommonFunc::writeLog("funSetNetworkCallback " + QString().sprintf("%p", QThread::currentThread()->currentThreadId()));
    if (g_pNetworkManager) {
        g_pNetworkManager->setCallback(pFunc, pUser);
    }
    return 0;
}

void funRequestNetworkAndCallback(std::string sUserName, std::string sPassword)
{
    CommonFunc::writeLog("funRequestNetworkAndCallback " + QString().sprintf("%p", QThread::currentThread()->currentThreadId()));
    if (g_pNetworkManager == nullptr) {
        g_pNetworkManager = NetworkManager::getInstance();
    }
    if (g_pNetworkManager) {
        g_pNetworkManager->login(sUserName, sPassword);
    }
}
/*
int funSetSerialCallback(int nHandle, CBFun_Callback pFunc, void* pUser)
{
    CommonFunc::writeLog("funSetCallback");
    if (nullptr == pUser)
        return ERROR_CODE_PARAM;

    if(g_pSerialManager) return g_pSerialManager->SetCallback(nHandle, pFunc, pUser);
}*/

int Init(int nType,char* sParas)
{
    if (nullptr == sParas)
        return ERROR_CODE_PARAM;

    try
    {
        if(nullptr == g_pSerialManager)
            g_pSerialManager = SerailManager::getInstance();
        if(g_pSerialManager) return g_pSerialManager->Init(nType, sParas);
    }
    catch(QException &e)
    {
        CommonFunc::writeLog(QString("Init exception :") + e.what());
        return ERROR_CODE_UNKNOW;
    }
    catch(...)
    {
        return ERROR_CODE_UNKNOW;
    }

    return ERROR_CODE_UNKNOW;
}

int Deinit(int nHandle)
{
    if(g_pSerialManager)
    {
        g_pSerialManager->Deinit(nHandle);
        delete g_pSerialManager;
        g_pSerialManager = nullptr;
        return ERROR_CODE_NORMAL;
    }
    return ERROR_CODE_UNKNOW;
}

//int testLoad() { return 0; }