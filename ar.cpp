//
//  ar.cpp
//  hw4
//
//  Created by Jaewoo Cho on 11/10/22.
//

#include "ar.hpp"
#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <fstream>
#include "functions.hpp"
#include <opencv2/bgsegm.hpp>

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
//    load face and eye classifiers
    CascadeClassifier faceClassifier;
    CascadeClassifier eyeClassifier;
    faceClassifier.load( "/usr/local/Cellar/opencv/4.6.0_1/share/opencv4/haarcascades/haarcascade_frontalface_default.xml");
    eyeClassifier.load("/usr/local/Cellar/opencv/4.6.0_1/share/opencv4/haarcascades/haarcascade_eye_tree_eyeglasses.xml");
    vector<Rect> faces;
    vector<Rect> eyes;
    char lastClicked = ' ';
    int x_offset;
    int y_offset;
    int size_offset;
    int FILTER_OPTION_HAT = 1;
    int FILTER_OPTION_MASK = 2;
    
    for(;;) {
        *capdev >> frame; // get a new frame from the camera, treat as a stream
        if( frame.empty() ) {
            printf("frame is empty\n");
            break;
        }
        
        switch(lastClicked) {
            case ' ': {
//                detectClassifier(frame, faceClassifier, faces);
                break;
            }
            case 'g': {
                x_offset = 20;
                y_offset = 15;
                size_offset = 15;
                Mat glassImg = imread("/Users/jaewoocho/Desktop/School work/CS 5330/project/ItemRecognition/filter_images/glasses.png");
                Mat background = imread("/Users/jaewoocho/Desktop/School work/CS 5330/project/ItemRecognition/filter_images/library_bg.jpg");
                applyGlassesFilter(glassImg, eyes, frame, faceClassifier, eyeClassifier, x_offset, y_offset, size_offset);
                applyBackground(background, frame);
                break;
            }
            case 'h': {
                //                harry potter filter
                x_offset = 40;
                y_offset = 80;
                size_offset = 30;
                Mat filterImg = imread("/Users/jaewoocho/Desktop/School work/CS 5330/project/ItemRecognition/filter_images/harry_potter2.png");
                Mat background = imread("/Users/jaewoocho/Desktop/School work/CS 5330/project/ItemRecognition/filter_images/harry_potter_bg_2.jpeg");
                applyGlassesFilter(filterImg, eyes, frame, faceClassifier, eyeClassifier, x_offset, y_offset, size_offset);
                applyBackground(background, frame);
                break;
            }
            case 'm': {
//                apply mask filter
                x_offset = 10;
                y_offset = 50;
                size_offset = 10;
                Mat maskImg = imread("/Users/jaewoocho/Desktop/School work/CS 5330/project/ItemRecognition/filter_images/black_mask.png");
                Mat background = imread("/Users/jaewoocho/Desktop/School work/CS 5330/project/ItemRecognition/filter_images/covid_bg.jpeg");
                applyFilter(maskImg, faces, frame, faceClassifier, x_offset, y_offset, size_offset, FILTER_OPTION_MASK);
                applyBackground(background, frame);
                break;
            }
            case 's': {
                x_offset = 20;
                y_offset = 35;
                size_offset = 5;
                Mat hatImg = imread("/Users/jaewoocho/Desktop/School work/CS 5330/project/ItemRecognition/filter_images/christmas_hat.png");
                Mat background = imread("/Users/jaewoocho/Desktop/School work/CS 5330/project/ItemRecognition/filter_images/santa_bg.jpg");
                applyFilter(hatImg, faces, frame, faceClassifier, x_offset, y_offset, size_offset, FILTER_OPTION_HAT);
                applyBackground(background, frame);
                break;
            }
        }
//        Mat background = imread("/Users/jaewoocho/Desktop/School work/CS 5330/project/ItemRecognition/filter_images/harry_potter_bg.jpeg");
//        applyBackground(background, frame, bg_subtractor);
        imshow("Video", frame);
        
        char key = waitKey(10);
        
        if( key == 'q') {
            break;
        } else if ( key == 's' || key == ' ' || key == 'g' || key == 'm' || key == 'h') {
            lastClicked = key;
        }
        
        
    }
    delete capdev;
    return(0);
}
