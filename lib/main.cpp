#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <opencv2/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include "libExport.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("%s <model_path> <image_path>\n", argv[0]);
        return -1;
    }

    const char *model_path = argv[1];
    const char *image_path = argv[2];
    
    init(model_path);
    
    cv::Mat frame = cv::imread(image_path);
    if (frame.empty()) {
        printf("read image fail! image_path=%s\n", image_path);
        return 0;
    }

    stDetectResult* stResult = detect((char*)frame.data, frame.cols, frame.rows);
    cv::Mat resFrame = cv::Mat(stResult->nHeight, stResult->nWidth, CV_8UC3, frame.data);

    cv::imshow("res", frame);
    cv::waitKey(1000);
    cv::destroyAllWindows();

    track(stResult);
    
    deinit();
    return 0;
}
