//
//  main.cpp
//  Test
//
//  Created by Nursultan on 3/11/16.
//  Copyright © 2016 Nursultan. All rights reserved.
//

#include <iostream>
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <cmath>
#include <ctime>

using namespace std;
using namespace cv;


const int maxSlider = 100;
int R = 255, G, B;
int radius = 200*200;
int nRows, nCols, channels;
int slider, vidSlider;
char fileName[200];
Mat img, img2, img3, imgs[10];

bool ok(int r, int g, int b, int rad) {

    return ((r - R) * (r - R) + (g - G) * (g - G) + (b - B) * (b - B)) < rad;
}

void onTrackbar(int, void*) {

    uchar *p, *p2;
    uchar r, g, b;

    for(int i = 0; i < nRows; i++){
        
        p = img.ptr<uchar>(i);
        p2 = img2.ptr<uchar>(i);
        
        for(int j = 0; j < nCols; j++) {
            b = *p++;
            g = *p++;
            r = *p++;
            
            if(!ok(r, g, b, radius * slider * 0.15)) {
                
                *p2++ = (r + g + b) / 3;
                *p2++ = (r + g + b) / 3;
                *p2++ = (r + g + b) / 3;
            }
            else {
                *p2++ = b;
                *p2++ = g;
                *p2++ = r;
            }
        }
    }
    imshow("Project3", img2);
}

void callBackFunc(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN ||  event == EVENT_MBUTTONDOWN) {
        uchar *p = img.ptr<uchar>(nRows * y);
        B = p[channels * x];
        G = p[channels * x + 1];
        R = p[channels * x + 2];
    }
  
}

void processImage() {

     img = imread(fileName, CV_LOAD_IMAGE_UNCHANGED);
     img2 = img.clone();
    
     if(img.empty()) {
         puts("File not found");
         exit(0);
     }
    
     nRows = img.rows;
     nCols = img.cols;
     channels = img.channels();
     radius = 100 * 100;
    
     if(img.isContinuous()) nCols *= nRows, nRows = 1;
    
     namedWindow("Project3", 1);
    
     char TrackbarName[50];
    
     sprintf(TrackbarName, "Alpha x %d", maxSlider);
     createTrackbar(TrackbarName, "Project3", &slider, maxSlider, onTrackbar);
     setMouseCallback("Project3", callBackFunc, NULL);
     onTrackbar(slider, 0);
     waitKey(0);
}

void callBackFuncVid(int event, int x, int y, int flags, void* userdata)
{
    if  ( event == EVENT_LBUTTONDOWN || event == EVENT_RBUTTONDOWN ||  event == EVENT_MBUTTONDOWN) {
        uchar *p = img.ptr<uchar>(nRows * y);
        B = p[channels * x];
        G = p[channels * x + 1];
        R = p[channels * x + 2];
    }
    
}
void processVideo() {
    
    VideoCapture vid;
    vid.open(0);
    radius = 150 * 150;
    
    if(!vid.isOpened()) {
        cout << "Here is a problem with camera. Sorry bro!!!";
        exit(0);
    }
    
    char TrackbarName[50];
    
    sprintf(TrackbarName, "Alpha x %d", maxSlider);
    
    while(1) {
        
        vid >> img;
        
        createTrackbar(TrackbarName, "p", &vidSlider, maxSlider);
        setMouseCallback("p", callBackFuncVid, NULL);
        
        if(img.empty()) {
            cout << "Empty frame" << endl;
            break;
        }
        
        uchar *p;
        uchar r, g, b;
        nRows = img.rows;
        nCols = img.cols;
        
        if(img.isContinuous()) nCols *= nRows, nRows = 1;
        
        for(int i = 0; i < nRows; i++){
            p = img.ptr<uchar>(i);
            for(int j = 0; j < nCols; j++) {
                b = *p++;
                g = *p++;
                r = *p++;
                
                if(!ok(r, g, b, radius * vidSlider * 0.07)) {
                    p -= 3;
                    *p++ = (r + g + b) / 3;
                    *p++ = (r + g + b) / 3;
                    *p++ = (r + g + b) / 3;
                }
            }
        }
        if(waitKey(30) == 27) break;
        imshow("p", img);
        waitKey(10);
    }
}

void marvel() {
    imgs[0] = imread("m1.jpg", CV_LOAD_IMAGE_UNCHANGED);
    imgs[1] = imread("m2.jpg", CV_LOAD_IMAGE_UNCHANGED);
    imgs[2] = imread("m3.jpg", CV_LOAD_IMAGE_UNCHANGED);
    imgs[3] = imread("m4.jpg", CV_LOAD_IMAGE_UNCHANGED);
    imgs[4] = imread("m5.jpg", CV_LOAD_IMAGE_UNCHANGED);
    imgs[5] = imread("m6.jpg", CV_LOAD_IMAGE_UNCHANGED);
    imgs[6] = imread("m7.jpg", CV_LOAD_IMAGE_UNCHANGED);
    imgs[7] = imread("m8.jpg", CV_LOAD_IMAGE_UNCHANGED);
    imgs[8] = imread("m9.jpg", CV_LOAD_IMAGE_UNCHANGED);
    imgs[9] = imread("m10.jpg", CV_LOAD_IMAGE_UNCHANGED);
    img = imread("marvel.jpg", CV_LOAD_IMAGE_UNCHANGED);
    
    Size size(900, 600);
    
    for(int i = 0; i < 10; i++)
        resize(imgs[i], imgs[i], size);
    
    resize(img, img, size);
    img3 = imgs[0].clone();
    nRows = img3.rows;
    nCols = img3.cols;
    
    namedWindow("", 1);
    double alpha = 0, beta;
    uchar * p, *p2;

    int coef = 10;
    for(int k = 0; k < 100; k++) {
        int f = k % 10;
        int s = (k + 1) % 10;
        for(int h = coef; h < 600; h += coef) {
            
            for(int i = 0; i < h; i++) {
                p = imgs[s].ptr<uchar>(i);
                p2 = img3.ptr<uchar>(i);
                for(int j = 0; j < nCols; j++) {
                    *p2++ = *p++;
                    *p2++ = *p++;
                    *p2++ = *p++;
                }
            }
        
            for(int i = h; i < 600; i++) {
                p = imgs[f].ptr<uchar>(i);
                p2 = img3.ptr<uchar>(i);
                for(int j = 0; j < nCols; j++) {
                    *p2++ = *p++;
                    *p2++ = *p++;
                    *p2++ = *p++;
                }
            }
            coef+= 1;
            beta = ( 1.0 - alpha);
            addWeighted(img, alpha, img3, beta, 0.0, img3);
            imshow("", img3);
            waitKey(1);
            alpha += 0.004;
            if(alpha > 1) alpha = 1;
    
        }
    }
   
}

int main() {
    
    int tmp;
    
    puts("PLEASE, ENTER 1 AND FILE NAME TO PROCESS IMAGE, ENTER 2 TO PROCESS A LIFE STREAM VIDEO OR ENTER 3 FOR A BONUS PART.");

    scanf("%d", &tmp);
    
   // namedWindow("main", 1);
   // createButton("button", callbackButton, NULL, CV_RADIOBOX, 0);
    
   
    if(tmp == 1) {
        scanf("%s", fileName);
        processImage();
    }
    else
    if(tmp == 2)
        processVideo();
    else
        marvel();
    return 0;
}
