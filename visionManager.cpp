/** @file visionManager.c
 *  @brief Functions for managing images/vision.
 *
 *  These functions currently setup the pointers to where the PRU will write image data
 *  and an image data ready flag, process and display the image data from the shared memory
 *  in OpenCV windows, and garbage collect the image data when the program shuts down.
 *  The image ready flag is a cheap way to deal with the fact
 *  that the PRU can write data to the shared memory much faster than the application
 *  processor can consume it. We will want to come up with a better way of working with images
 *  on the application processor side. Image processing is currently just using OpenCV
 *  cvInRangeS function to mask a certain color (aibo ball pink) and cvMoments to identify
 *  instances of it in the image. The initialize function creates two windows, one for the
 *  image and one for the thresholded mask. The main loop updates these windows when the image
 *  ready flag is set.
 *
 *  @author Bill Merryman
 *  @bug No known bugs.
 *
 *  Created on: Nov 19, 2017
 *
 */

#include <iostream>
#include <fstream>
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/opencv.hpp"
#include "visionManager.hpp"

extern "C"
{
#include "PRUInterop.h"
#include "pru.h"
}

using namespace std;
using namespace cv;

volatile int *imageReadyFlag;
cv::Mat displayImage;
cv::Mat processingImage;

int imageProcessingType = 0;

Scalar thresholdLow = Scalar(60, 0, 100);
Scalar thresholdHigh = Scalar(200, 80, 255);
Rect thresholdROI = Rect(135, 95, 50, 50);

void visionManagerInitialize()
{

	CvSize inputSize;
	inputSize.width = IMAGE_COLUMNS_IN_PIXELS;
	inputSize.height = IMAGE_ROWS_IN_PIXELS;

	PRU_INTEROP_1_DATA* PRUInterop1Data = &(getPRUInteropData()->PRUInterop1Data);

	displayImage = cv::Mat(inputSize, CV_8UC3, (void*)(PRUInterop1Data->imageData));
	processingImage = cv::Mat(inputSize, CV_8UC3);
	imageReadyFlag = ((int *)(&(PRUInterop1Data->imageReadyFlag)));

	cvNamedWindow("Display_Image", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Processing_Image", CV_WINDOW_AUTOSIZE);
	setWindowTitle("Display_Image", "No Image Processing.");
	setWindowTitle("Processing_Image", "Not Used.");
}

void visionManagerUninitialize()
{
	cvDestroyWindow("Display_Image");
	cvDestroyWindow("Processing_Image");
}

void visionManagerProcess(char key)
{
	if(*imageReadyFlag == IMAGE_NOT_READY) return;

	if(key=='n')
	{
		imageProcessingType=0;
		setWindowTitle("Display_Image", "No Image Processing.");
		setWindowTitle("Processing_Image", "Not Used.");
	}
	if(key=='t')
	{
		imageProcessingType=1;
		setWindowTitle("Display_Image", "Process Image By Threshold");
		setWindowTitle("Processing_Image", "Image Moments");
	}
	if(key=='k')
	{
		imageProcessingType=2;
		setWindowTitle("Display_Image", "Capture color key for Threshold");
		setWindowTitle("Processing_Image", "Not Used.");
	}

	switch(imageProcessingType)
	{
		case 0:
			visionManagerProcessNone();
			break;
		case 1:
			visionManagerProcessThreshold();
			break;
		case 2:
			visionManagerCaptureThreshold();
			break;
	}

	*imageReadyFlag = IMAGE_NOT_READY;
}

void visionManagerProcessNone()
{
	imshow("Display_Image", displayImage);
	imshow("Processing_Image", processingImage);
}

void visionManagerProcessThreshold()
{
	double area = 0;
	CvPoint position;
	char outputMessage[50];

	/*
	 * Might want to revert this to not using the cloned images. This is a
	 * problem in 9.5, but wasn't in this version (9.3)
	 */

	Mat display = displayImage.clone();
	Mat processing = displayImage.clone();

	inRange(display, thresholdLow, thresholdHigh, processing);
	cv::Moments moments = cv::moments(processing, false);
	area = moments.m00;
	if (area > 1000000)
	{
		position.x = moments.m10 / area;
		position.y = moments.m01 / area;
		sprintf(outputMessage, "pos: %d, %d", position.x, position.y);
		rectangle(display, cvPoint(position.x - 5, position.y - 5), cvPoint(position.x + 5, position.y + 5), cvScalar(0, 255, 0, 0), 1, 8, 0);
		putText(display, outputMessage, Point(position.x + 10, position.y + 5), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2, 8, false);
	}
	imshow("Display_Image", display);
	imshow("Processing_Image", processing);
}

void visionManagerCaptureThreshold()
{
	char outputMessage[64];
	double minR;
	double minG;
	double minB;
	double maxR;
	double maxG;
	double maxB;
	Mat bgr[3];

	if(*imageReadyFlag == IMAGE_NOT_READY) return;

	split(displayImage(thresholdROI), bgr);
	minMaxLoc(bgr[0], &minR, &maxR);
	minMaxLoc(bgr[1], &minG, &maxG);
	minMaxLoc(bgr[2], &minB, &maxB);

	thresholdLow = Scalar(minR, minG, minB);
	thresholdHigh = Scalar(maxR, maxG, maxB);

	putText(displayImage, format("minR: %f", minR), Point(0, 10), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2, 8, false);
	putText(displayImage, format("minG: %f", minG), Point(0, 25), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2, 8, false);
	putText(displayImage, format("minB: %f", minB), Point(0, 40), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2, 8, false);
	putText(displayImage, format("maxR: %f", maxR), Point(0, 55), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2, 8, false);
	putText(displayImage, format("maxG: %f", maxG), Point(0, 70), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2, 8, false);
	putText(displayImage, format("maxB: %f", maxB), Point(0, 85), CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0), 2, 8, false);
	rectangle(displayImage, thresholdROI, Scalar(0, 255, 0), 1, 8, 0);

	imshow("Display_Image", displayImage);
	imshow("Processing_Image", processingImage);
}

