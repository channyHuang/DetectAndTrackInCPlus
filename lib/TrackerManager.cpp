#include "TrackerManager.h"

TrackerManager* TrackerManager::m_pInstance = nullptr;

TrackerManager::TrackerManager() {

}

// 释放资源
TrackerManager::~TrackerManager() {
    if (m_pInstance != nullptr) {
        delete m_pInstance;
    }
    m_pInstance = nullptr;
}

void TrackerManager::updateObjects(stDetectResult* stResults) {
    int nDetectNum = stResults->nDetectNum;
    objects.clear();
    for (int i = 0; i < nDetectNum; ++i) {
        float x = stResults->pBoxes[i << 2];
        float y = stResults->pBoxes[(i << 2) + 1];
        float width = stResults->pBoxes[(i << 2) + 2] - x;
        float height = stResults->pBoxes[(i << 2) + 3] - y;
        float conf = stResults->pProb[i];

        objects.emplace_back(byte_track::Rect(x, y, width, height), 0, conf);
    }
}

void TrackerManager::track() {
    const auto vTrackOutput = m_cTracker.update(objects);
    int i = 0;
    for (const auto &output : vTrackOutput) {
        const auto &rect = output->getRect();
        
        if (fabs(rect.x() - objects[i].rect.x()) < m_fEps && 
            fabs(rect.y() - objects[i].rect.y()) < m_fEps && 
            fabs(rect.width() - objects[i].rect.width()) < m_fEps &&
            fabs(rect.height() - objects[i].rect.height()) < m_fEps) {
                printf("tracked\n");
            }
        ++i;
    }
}
