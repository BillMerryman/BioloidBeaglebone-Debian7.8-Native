#include "LED.h"
#include "image.h"
#include "PRUInterop1.h"

volatile unsigned int *imageReadyFlag;

int main()
{
	imageInitialize();
	imageReadyFlag = PRUInterop1GetImageReadyFlag();
	*imageReadyFlag = IMAGE_NOT_READY;

	while(1)
	{
		while(*imageReadyFlag == IMAGE_READY);
		GET_IMAGE;
		*imageReadyFlag = IMAGE_READY;
		LED_TOGGLE(PIN_NUMBER_FOR_LED_3);
	}
}
