/** @file main.c
 *  @brief Initialization and system starting point.
 *
 *  Right now we just initialize the PRUs and loop through looking for keyboard input
 *  to initiate a servo action (like playing a page), and taking a frame of video and
 *  processing it with OpenCV color range matching and moments recognition.
 *
 *  @author Bill Merryman
 *  @bug No known bugs.
 *
 *  Created on: Oct 16, 2015
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "cv.h"
#include "highgui.h"
#include "pru.h"
#include "PRUInterop.h"
#include "motionManager.h"
#include "visionManager.h"

int main (int argc, char *argv[])
{

	if(argc < 8)
	{
		fprintf(stderr, "Usage: BioloidBeaglebone PRU0Firmware PRU1Firmware MotionFile MLProtoFile MLCaffeFile");
		return -1;
	}

	int key = 0;

	initializePRU();

	configurePRU_0(argv[1], argv[2]);
	startPRU_0();
	configurePRU_1(argv[3], argv[4]);
	startPRU_1();

	motionManagerInitialize(argv[5]);
	visionManagerInitialize(argv[6], argv[7]);

	while(key != 'x')
	{
		visionManagerProcess(key);
		motionManagerProcess(key);
		key = cvWaitKey(25);
	}

	visionManagerUninitialize();
	stopPRU_0();
	stopPRU_1();
}
