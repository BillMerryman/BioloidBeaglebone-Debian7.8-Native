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

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>

#include "cv.h"
#include "highgui.h"
#include "pru.h"
#include "PRUInterop.h"
#include "motionManager.h"
#include "visionManager.hpp"

int main (int argc, char *argv[])
{

	if(argc < 6)
	{
		fprintf(stderr, "Usage: BioloidBeaglebone PRU0Firmware PRU1Firmware MotionFile");
		return -1;
	}

	int key = 0;

	const char *PRU_0Firmware_Text = argv[1];
	const char *PRU_0Firmware_Data = argv[2];
	const char *PRU_1Firmware_Text = argv[3];
	const char *PRU_1Firmware_Data = argv[4];
	const char *motionFile = argv[5];

	initializePRU();

	configurePRU_0(PRU_0Firmware_Text, PRU_0Firmware_Data);
	startPRU_0();
	configurePRU_1(PRU_1Firmware_Text, PRU_1Firmware_Data);
	startPRU_1();

	motionManagerInitialize(motionFile);
	visionManagerInitialize();

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
