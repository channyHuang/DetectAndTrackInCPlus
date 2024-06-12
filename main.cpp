#include <iostream>

#include "Detect/detectAndTrack.h"

int test_detect() {
    DetectAndTrack::getInstance()->init("./../data/1945.MP4");

    std::this_thread::sleep_for(std::chrono::seconds(1));
    DetectAndTrack::getInstance()->setStatus(1);
    std::this_thread::sleep_for(std::chrono::seconds(5));
    DetectAndTrack::getInstance()->setStatus(0);
    std::this_thread::sleep_for(std::chrono::seconds(8));
    DetectAndTrack::getInstance()->setStatus(1);

    std::this_thread::sleep_for(std::chrono::seconds(50));
    return 0;
}

#include "Track/BYTETracker.h"
#include <cstddef>
#include <fstream>
#include <iostream>
#include <string>
#include <string.h>
#include "json.hpp"

struct Info {
    int frame_id;
    int track_id;
    float x, y;
    float width, height;
    float prob;
};

struct Results {
    int fps;
    int track_buffer;
    std::vector<Info> results;
};

nlohmann::json readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        file.clear();
        return "";
    }
    nlohmann::json inputJson;
    file >> inputJson;
    return inputJson;
}

Results readTrackResults(bool bHasProb = false) {
    const std::string RESULTS_FILE = bHasProb ? "detection_results.json" :  "./tracking_results.json";
    nlohmann::json inputJson = readFile(RESULTS_FILE);
    Results stResults;
    stResults.fps = inputJson.at("fps");
    stResults.track_buffer = inputJson.at("track_buffer");
    for (auto &itemJson : inputJson.at("results")) {
        Info info;
        info.frame_id = std::stoi(std::string(itemJson.at("frame_id")), 0, 10);
        info.x = std::stof(std::string(itemJson.at("x")));
        info.y = std::stof(std::string(itemJson.at("y")));
        info.width = std::stof(std::string(itemJson.at("width")));
        info.height = std::stof(std::string(itemJson.at("height")));
        if (bHasProb) {
            info.prob = std::stof(std::string(itemJson.at("prob")));
        } else {
            info.track_id = std::stoi(std::string(itemJson.at("track_id")), 0, 10);
        }
        stResults.results.push_back(info);
    }

    return stResults;
}

int test_track() {
    constexpr double EPS = 1e-2;

    // key: track_id, value: rect of tracking object
    using BYTETrackerOut = std::map<size_t, byte_track::Rect<float>>;
    
    std::map<size_t, std::vector<byte_track::Object>> inputs_ref;

    Results input = readTrackResults(true);
    for (auto itr = input.results.begin(); itr != input.results.end(); itr++) {
        int frame_id = itr->frame_id;
        float x = itr->x;
        float y = itr->y;
        float width = itr->width;
        float height = itr->height;
        float prob = itr->prob;
        auto nitr = inputs_ref.find(frame_id);
        if (nitr != inputs_ref.end()) {
            nitr->second.emplace_back(byte_track::Rect(x, y, width, height), 0, prob);
        } else {
            std::vector<byte_track::Object> v(1, {byte_track::Rect(x, y, width, height), 0, prob});
            inputs_ref.emplace_hint(inputs_ref.end(), frame_id, v);
        }
    }

    std::map<size_t, BYTETrackerOut> outputs_ref;
    Results output = readTrackResults(false);
    for (auto itr = output.results.begin(); itr != output.results.end(); itr++) { 
        int frame_id = itr->frame_id;
        int track_id = itr->track_id;
        float x = itr->x;
        float y = itr->y;
        float width = itr->width;
        float height = itr->height;


        auto nitr = outputs_ref.find(frame_id);
        if (nitr != outputs_ref.end())
        {
            nitr->second.emplace(track_id, byte_track::Rect<float>(x, y, width, height));
        }
        else
        {
            BYTETrackerOut v{
                {track_id, byte_track::Rect<float>(x, y, width, height)},
            };
            outputs_ref.emplace_hint(outputs_ref.end(), frame_id, v);
        }
    }

    const auto fps = output.fps;
    const auto track_buffer = output.track_buffer;

    byte_track::BYTETracker tracker(fps, track_buffer);
    for (const auto &[frame_id, objects] : inputs_ref)
    {
        const auto outputs = tracker.update(objects);

        // Verify between the reference data and the output of the BYTETracker impl
        // EXPECT_EQ(outputs.size(), outputs_ref[frame_id].size());
        for (const auto &outputs_per_frame : outputs)
        {
            const auto &rect = outputs_per_frame->getRect();
            const auto &track_id = outputs_per_frame->getTrackId();
            const auto &ref = outputs_ref[frame_id][track_id];
            // EXPECT_NEAR(ref.x(), rect.x(), EPS);
            // EXPECT_NEAR(ref.y(), rect.y(), EPS);
            // EXPECT_NEAR(ref.width(), rect.width(), EPS);
            // EXPECT_NEAR(ref.height(), rect.height(), EPS);

            std::cout << ref.x() << " " << ref.y() << " " << ref.width() << " " << ref.height() << std::endl;
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    // test_detect();
    test_track();
    return 0;
}