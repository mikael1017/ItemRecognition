//
//  main.cpp
//  hw4
//
//  Created by Jaewoo Cho on 11/8/22.
//


#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "functions.hpp"

using namespace cv;
using namespace std;

int main(int argc, char *argv[]) {
    cv::VideoCapture *capdev;
    // open the video device
    capdev = new cv::VideoCapture(0);
    if( !capdev->isOpened() ) {
        printf("Unable to open video device\n");
        return(-1);
    }
    // get some properties of the image
    Size refS( (int) capdev->get(cv::CAP_PROP_FRAME_WIDTH ),
                  (int) capdev->get(cv::CAP_PROP_FRAME_HEIGHT));
    printf("Expected size: %d %d\n", refS.width, refS.height);
    Mat frame;
    Size patternsize(9, 6);
    vector<Point2f> corners;
    vector<Vec3f> point_set;
    vector<vector<cv::Vec3f> > point_list;
    vector<vector<cv::Point2f> > corner_list;
    int REQUIRED = 5;
    Mat cameraMat(3, 3, CV_64FC1, Scalar(0));
    Mat distCoeffs,R,T,stdDevInt, stdDevExt, perViewErrors;
//    define the cameraMat,
//    frame is 1280 x 720 so i used cols/2 and rows/2
    for (int i = 0; i < cameraMat.rows; i++) {
        for (int j = 0; j < cameraMat.cols; j++) {
            if (i == j) {
                cameraMat.at<double>(i, j) = 1;
            } else if (i == 0 && j == 2) {
                cameraMat.at<double>(i, j) = 640;
            } else if (i == 1 && j == 2) {
                cameraMat.at<double>(i, j) = 360;
            }
        }
    }
    
   
    for(;;) {
        *capdev >> frame; // get a new frame from the camera, treat as a stream
        if( frame.empty() ) {
            printf("frame is empty\n");
            break;
        }
       
        //        process the image recognition
        bool patternfound = findChessboardCorners(frame, patternsize, corners,
                CALIB_CB_ADAPTIVE_THRESH + CALIB_CB_NORMALIZE_IMAGE
                + CALIB_CB_FAST_CHECK);

//        draw the corners of the checkboard
        if (patternfound) {
            Mat viewGray;
            cvtColor(frame, viewGray, COLOR_BGR2GRAY);
            cornerSubPix( viewGray, corners, Size(11,11),
                                    Size(-1,-1), TermCriteria( TermCriteria::EPS+TermCriteria::COUNT, 30, 0.0001 ));
            drawChessboardCorners(frame, patternsize, Mat(corners), patternfound);

        }
        
        // see if there is a waiting keystroke
        char key = cv::waitKey(10);
        if (key == 's') {
            printf("%lu\n", corners.size());
            corner_list.push_back(corners);
            get3dVectors(point_set);
            point_list.push_back(point_set);
            point_set.clear();
        } else if (key == 'c') {
            float error = calibrateCamera(point_list, corner_list, Size(frame.cols,frame.rows), cameraMat, distCoeffs, R, T, stdDevInt, stdDevExt, perViewErrors, CALIB_FIX_ASPECT_RATIO);
            writeFile(cameraMat, distCoeffs);
            std::cout << "error : " << error << std::endl;
//            std::cout << "cameraMatrix : " << cameraMat << std::endl;
//              std::cout << "distCoeffs : " << distCoeffs << std::endl;
//              std::cout << "Rotation vector : " << R << std::endl;
//              std::cout << "Translation vector : " << T << std::endl;
//            std::cout << "stdDevIntrinsic : " << stdDevInt << std::endl;
//            std::cout << "stdDevExtrinsic : " << stdDevExt << std::endl;
//            std::cout << "perViewErrors : " << perViewErrors << std::endl;
            std::cout << "done" << std::endl;
        } else if( key == 'q') {
            break;
        }
        imshow("Video", frame);
    }
    delete capdev;
    return(0);
}
