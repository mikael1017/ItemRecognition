//
//  functions.hpp
//  hw4
//
//  Created by Jaewoo Cho on 11/9/22.
//

#ifndef functions_hpp
#define functions_hpp

#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <opencv2/bgsegm.hpp>

using namespace std;
using namespace cv;

int detectClassifier(Mat &frame, CascadeClassifier &classifier, vector<Rect> &faces);
int applyGlassesFilter(Mat &img, vector<Rect> &eyes, Mat &frame, CascadeClassifier faceClassifier, CascadeClassifier eyeClassifier,  int xOffset, int yOffset , int sizeOffset);
int applyFilter(Mat &img, vector<Rect> &faces, Mat &frame, CascadeClassifier faceClassifier, int xOffset, int yOffset , int sizeOffset, int type);
int applyBackground(Mat &img, Mat &frame);

#endif /* functions_hpp */
