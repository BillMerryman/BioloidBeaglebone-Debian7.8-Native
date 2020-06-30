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

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "visionManager.h"
#include "cv.h"
#include "highgui.h"
//#include "motionProcessor.h"
#include "PRUInterop.h"
#include "pru.h"

volatile int *imageReadyFlag;
IplImage* displayImage;
IplImage* processingImage;
CvFont font;
int imageProcessingType = 1;


void visionManagerInitialize(const char *protoFile, const char *caffeFile)
{

	CvSize inputSize;
	inputSize.width = IMAGE_COLUMNS_IN_PIXELS;
	inputSize.height = IMAGE_ROWS_IN_PIXELS;

	PRU_INTEROP_1_DATA* PRUInterop1Data = &(getPRUInteropData()->PRUInterop1Data);

	displayImage = cvCreateImageHeader(inputSize, IPL_DEPTH_8U, 3);
	processingImage = cvCreateImage(inputSize, IPL_DEPTH_8U, 1);
	imageReadyFlag = ((int *)(&(PRUInterop1Data->imageReadyFlag)));
	cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, 8);
	cvSetData(displayImage, (void *)(PRUInterop1Data->imageData), displayImage->widthStep);
	cvNamedWindow("Display_Image", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Processing_Image", CV_WINDOW_AUTOSIZE);
}

void visionManagerUninitialize()
{
	cvReleaseImageHeader(&displayImage);
	cvReleaseImage(&processingImage);
	cvDestroyWindow("Display_Image");
	cvDestroyWindow("Processing_Image");
}

void visionManagerProcess(char key)
{
	if(key=='t') imageProcessingType=1;
	if(key=='d') imageProcessingType=2;
	if(*imageReadyFlag == IMAGE_NOT_READY) return;
	switch(imageProcessingType)
	{
		case 1:
			visionManagerProcessThreshold();
			break;
		case 2:
			visionManagerProcessDNN();
			break;
	}
	*imageReadyFlag = IMAGE_NOT_READY;
}

void visionManagerProcessThreshold()
{
	double area = 0;
	CvPoint position;
	char outputMessage[50];
	CvMoments moments;

	cvInRangeS(displayImage, cvScalar(60, 0, 100, 0), cvScalar(200, 80, 255, 0), processingImage);
	cvMoments(processingImage, &moments, 0);
	area = moments.m00;
	if (area > 1000000)
	{
		position.x = moments.m10 / area;
		position.y = moments.m01 / area;
		sprintf(outputMessage, "pos: %d, %d", position.x, position.y);
		cvRectangle(displayImage, cvPoint(position.x - 5, position.y - 5), cvPoint(position.x + 5, position.y + 5), cvScalar(0, 255, 0, 0), 1, 8, 0);
		cvPutText(displayImage, outputMessage, cvPoint(position.x + 10, position.y + 5), &font, cvScalar(0, 0, 0, 0));
	}
	//setWindowTitle("Display_Image", "Process Image By Threshold");
	//setWindowTitle("Processing_Image", "Image Moments");
	cvShowImage("Display_Image", displayImage);
	cvShowImage("Processing_Image", processingImage);
}

void visionManagerProcessDNN()
{
	//Not yet implemented
}

