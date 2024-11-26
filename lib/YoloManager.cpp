#include "YoloManager.h"

YoloManager* YoloManager::m_pInstance = nullptr;

YoloManager::YoloManager() {

}

// 释放资源
YoloManager::~YoloManager() {
    if (m_pInstance != nullptr) {
        delete m_pInstance;
    }
    m_pInstance = nullptr;
}

// load onnx model
bool YoloManager::init(const char* pModelName, int nWidth, int nHeight) {
    net = cv::dnn::readNetFromONNX(pModelName);

    if (bEnableCuda) {
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    } else {
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
    }
    m_nModelHeight = nHeight;
    m_nModelWidth = nWidth;
}

void YoloManager::detect(const cv::Mat& frame, stDetectResult &stResult) {
    cv::Mat modelInput = frame;
    modelInput = fillFrameToModelSize(frame);

    cv::Mat blob;
    cv::dnn::blobFromImage(modelInput, blob, 1.0/255.0, cv::Size(m_nModelHeight, m_nModelWidth), cv::Scalar(), true, false);
    net.setInput(blob);

    std::vector<cv::Mat> outputs;
    net.forward(outputs, net.getUnconnectedOutLayersNames());

    int rows = outputs[0].size[1];
    int dimensions = outputs[0].size[2];

    bool yolov8 = false;
    if (dimensions > rows) {
        yolov8 = true;
        rows = outputs[0].size[2];
        dimensions = outputs[0].size[1];

        outputs[0] = outputs[0].reshape(1, dimensions);
        cv::transpose(outputs[0], outputs[0]);
    }

    float *data = (float *)outputs[0].data;

    float x_factor = modelInput.cols / m_nModelWidth;
    float y_factor = modelInput.rows / m_nModelHeight;

    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;

    for (int i = 0; i < rows; ++i) {
        if (yolov8) {
            float *classes_scores = data+4;

            cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double maxClassScore;

            minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);

            if (maxClassScore > m_fScoreThreshold) {
                if (class_id.x != 0 && class_id.x != 2 && class_id.x != 5) {
                    continue;
                }
                confidences.push_back(maxClassScore);
                class_ids.push_back(class_id.x);

                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];

                int left = int((x - 0.5 * w) * x_factor);
                int top = int((y - 0.5 * h) * y_factor);

                int width = int(w * x_factor);
                int height = int(h * y_factor);

                boxes.push_back(cv::Rect(left, top, width, height));
            }
        }
        else { // yolov5
            float confidence = data[4];

            if (confidence >= m_fConfidenceThreshold) {
                float *classes_scores = data+5;

                cv::Mat scores(1, classes.size(), CV_32FC1, classes_scores);
                cv::Point class_id;
                double max_class_score;

                minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

                if (max_class_score > m_fScoreThreshold) {
                    confidences.push_back(confidence);
                    class_ids.push_back(class_id.x);

                    float x = data[0];
                    float y = data[1];
                    float w = data[2];
                    float h = data[3];

                    int left = int((x - 0.5 * w) * x_factor);
                    int top = int((y - 0.5 * h) * y_factor);

                    int width = int(w * x_factor);
                    int height = int(h * y_factor);

                    boxes.push_back(cv::Rect(left, top, width, height));
                }
            }
        }

        data += dimensions;
    }

    std::vector<int> nms_result;
    cv::dnn::NMSBoxes(boxes, confidences, m_fScoreThreshold, m_fNMSThreshold, nms_result);

    // stDetectResult stResult;
    if (nms_result.size() > 0) {
        stResult.nDetectNum = nms_result.size();
        stResult.pClasses = new int[nms_result.size()];
        stResult.pBoxes = new int[nms_result.size() << 2];
        stResult.pProb = new float[nms_result.size()];
    }
    for (unsigned long i = 0; i < nms_result.size(); ++i) {
        int idx = nms_result[i];

        stResult.pClasses[i] = class_ids[idx];
        stResult.pProb[i] = confidences[idx];
        stResult.pBoxes[(i << 2)] = boxes[idx].x;
        stResult.pBoxes[(i << 2) + 1] = boxes[idx].y;
        stResult.pBoxes[(i << 2) + 2] = boxes[idx].x + boxes[idx].width;
        stResult.pBoxes[(i << 2) + 3] = boxes[idx].y + boxes[idx].height;
    }
    if (stResult.pFrame == nullptr) {
        stResult.pFrame = new unsigned char[sizeof(frame.data)];
    }
    memcpy(stResult.pFrame, frame.data, sizeof(frame.data) * sizeof(unsigned char));
    stResult.nWidth = frame.cols;
    stResult.nHeight = frame.rows;

    return;
}

cv::Mat YoloManager::fillFrameToModelSize(const cv::Mat& frame) {
    int col = frame.cols;
    int row = frame.rows;
    int _max = MAX(col, row);
    cv::Mat result = cv::Mat::zeros(_max, _max, CV_8UC3);
    frame.copyTo(result(cv::Rect(0, 0, col, row)));
    return result;
}