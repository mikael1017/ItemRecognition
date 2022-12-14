//
//  functions.cpp
//  hw4
//
//  Created by Jaewoo Cho on 11/9/22.
//

#include <cstdlib>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <fstream>
#include <string>
#include <iostream>
#include <cstdio>
#include <opencv2/bgsegm.hpp>
using namespace std;
using namespace cv;

int detectClassifier(Mat &frame, CascadeClassifier &classifier, vector<Rect> &detects) {
    
    classifier.detectMultiScale(frame, detects);
    
    for (int i = 0; i < detects.size(); i++) {
//        uncomment for debug
//        rectangle(frame, detects[i].tl(), detects[i].br(), Scalar(50,50,225), 3);
    }
    return 0;
}

int applyBackground(Mat &img, Mat &frame) {
    Mat thresholdImage_inv;
    Mat thresholdImage;
    
    Mat bg_mask;
    inRange(frame, Scalar(0,0,180), Scalar(255,255,255), thresholdImage_inv);
    bitwise_not(thresholdImage_inv, bg_mask);
    
    int morph_size = 6;
    Mat element = getStructuringElement(
            MORPH_RECT, Size(2 * morph_size + 1,
                             2 * morph_size + 1),
            Point(morph_size, morph_size));
    Mat erod, dill;
      
    dilate(bg_mask, bg_mask, element,
               Point(-1, -1), 1);
    
//    imshow("mask", bg_mask);
    Mat new_img;
    Mat new_bg;
    Mat color_bg_mask;
    Mat color_bg_mask_inv;
    Mat new_frame;
    Mat new_fg;
    resize(img, new_img, frame.size());
    cvtColor(bg_mask, color_bg_mask, COLOR_GRAY2BGR);
    bitwise_not(color_bg_mask, color_bg_mask_inv);
    bitwise_and(color_bg_mask_inv, new_img, new_bg);
    bitwise_and(color_bg_mask, frame, new_fg);
    bitwise_or(new_fg, new_bg, new_frame);
    
//  uncomment for debugging
//    imshow("new fg", new_fg);
//    imshow("new bg", new_bg);
//    imshow("new frame", new_frame);
    new_frame.copyTo(frame);
    return 0;
}

/**
 Filter that applys 
 */
int applyFilter(Mat &img, vector<Rect> &faces, Mat &frame, CascadeClassifier faceClassifier,  int xOffset, int yOffset , int sizeOffset, int type) {
    Mat filterMask;
    Mat filterMask_inv;
    Mat img_g;
    Mat color_img;
    Mat img_copy;
    
    detectClassifier(frame, faceClassifier, faces);
    cvtColor(img, img_g, COLOR_BGR2GRAY);
    threshold(img_g, filterMask, 10, 255, THRESH_BINARY);
    img.copyTo(img_copy);
    bitwise_not(filterMask, filterMask_inv);
    
    for (int i = 0; i < faces.size(); i++) {
        Mat face = frame(faces[i]);
        Rect face_rect = faces[i];

        Mat mask;
        Mat color_mask;
        
        int new_x = face_rect.x;
        int new_y = face_rect.y;
        int new_width = face_rect.width;
        int new_height = face_rect.height;
        
//        using inverted picture as an input image
//        reverting back to the original image
        bitwise_not(img_copy, color_img);
        switch (type) {
            case 1: {
//                apply hat filter
                
                new_x = face_rect.x - xOffset;
                new_y = face_rect.y - (face_rect.height) + yOffset;
                new_width = face_rect.width + (3 * sizeOffset);
                new_height = face_rect.height + (3 * sizeOffset);
                break;
            }
            case 2: {
//                apply mask filter
                new_x = face_rect.x - xOffset;
                new_y = face_rect.y + yOffset;
                new_width = face_rect.width + (2 * sizeOffset);
                new_height = face_rect.height + (2 * sizeOffset);
                break;
            }
        }
        if (new_y < 0 || new_x < 0 || new_x + new_width > frame.cols || new_y + new_height > frame.rows) {
            return -1;
        }
        
        Rect filter_rect(new_x, new_y, new_width, new_height);
        
        resize(filterMask, mask, Size(filter_rect.width, filter_rect.height));
        resize(color_img, color_mask, Size(filter_rect.width, filter_rect.height));
        
        Mat roi(frame, filter_rect);
        Mat baseFrame(roi.rows, roi.cols, roi.type(), Scalar(0, 0, 0));
        Mat baseFrameInv;
        Mat masked;
        Mat colorMask;
        Mat frame_bg;
        Mat masked_inv;
        Mat frame_fg;
        Mat filtered;
        
//        first use grayscale image of filter to get the mask image
//        then apply bitwise to get the background and foreground
        cvtColor(mask, colorMask, COLOR_GRAY2BGR);
        bitwise_not(baseFrame, baseFrameInv);
//        imshow("baseframe", baseFrame);
        bitwise_and(baseFrameInv, colorMask, masked);
//        imshow("baseframe", masked);
        bitwise_not(masked, masked_inv);
//        imshow("masked inv", masked_inv);
        bitwise_and(roi, masked_inv, frame_bg);
//        imshow("frame bg", frame_bg);
        bitwise_and(color_mask, masked, frame_fg);
//        imshow("frame fg", frame_fg);
        bitwise_or(frame_fg, frame_bg, filtered);
    
//        imshow("fg", frame_fg);
//        imshow("bg", frame_bg);
        
        filtered.copyTo(frame(filter_rect));
    }
    
    return 0;
}

/**
 apply glasses filter with given input parameters
 */
int applyGlassesFilter(Mat &img, vector<Rect> &eyes, Mat &frame, CascadeClassifier faceClassifier, CascadeClassifier eyeClassifier, int xOffset, int yOffset , int sizeOffset) {
    
    Mat sunglassMask;
    Mat sunglassMask_inv;
    vector<Rect> faces;
    Rect left_eye;
    Rect right_eye;
    
    detectClassifier(frame, faceClassifier, faces);
    
    img.copyTo(sunglassMask);
    
    bitwise_not(sunglassMask, sunglassMask_inv);
    
    for (int i = 0; i < faces.size(); i++) {
        
        Mat face = frame(faces[i]);
        eyeClassifier.detectMultiScale(face, eyes);
        if (eyes.size() == 2) {
            //            when 2 eyes are found inside one face
            if (eyes[0].x < eyes[1].x) {
                left_eye = eyes[0];
                right_eye = eyes[1];
            } else {
                left_eye = eyes[1];
                right_eye = eyes[0];
            }
            Rect eye_rect(left_eye.x - xOffset, left_eye.y - yOffset, right_eye.x + right_eye.width - left_eye.x + (3 * sizeOffset), right_eye.y + right_eye.height - left_eye.y + (3 * sizeOffset));
            bool is_inside = (eye_rect & cv::Rect(0, 0, face.cols, face.rows)) == eye_rect;
            if (is_inside) {
                Mat roi(face, eye_rect);
                
//                Mat mask;
                Mat mask_inv;
//                resize(sunglassMask, mask, Size(eye_rect.width, eye_rect.height));
                resize(sunglassMask_inv, mask_inv, Size(eye_rect.width, eye_rect.height));
                Mat frame_fg;
                bitwise_and(mask_inv, roi, frame_fg);
                frame_fg.copyTo(face(eye_rect));
                //                imshow("face", face);
                face.copyTo(frame(faces[i]));
            }
        }
    }

    return 0;
}
