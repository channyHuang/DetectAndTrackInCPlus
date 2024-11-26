#pragma once

#include "libExport.h"

#include "Track/BYTETracker.h"

class TrackerManager {
public:
    static TrackerManager *getInstance() {
        if (m_pInstance == nullptr) {
            m_pInstance = new TrackerManager();
        }
        return m_pInstance;
    }
    ~TrackerManager();

    void updateObjects(stDetectResult* stResults);
    void track();

private:
    TrackerManager();

private:
    static TrackerManager *m_pInstance;

    byte_track::BYTETracker m_cTracker;
    std::vector<byte_track::Object> objects;
    float m_fEps = 1e-4;
};
