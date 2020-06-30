/** @file dynamixels.h
 *  @brief Function prototypes for handling devices generically on the serial bus.
 *
 *  These functions check for the presence of a dynamixel device, and determine
 *  the nature of that device (whether it is an AX-12 servo, an AX-S1 sensor, etc...).
 *
 *  @author Bill Merryman
 *  @bug No known bugs.
 *
 *  Created on: Dec 19, 2013
 *
 */

#ifndef DYNAMIXELS_H_
#define DYNAMIXELS_H_

#include "common.h"

#define DYNAMIXEL_ID_WIDTH							1
#define DYNAMIXEL_SYNC_STARTING_ADDRESS_POSITION	0
#define DYNAMIXEL_SYNC_STARTING_ADDRESS_WIDTH		1
#define DYNAMIXEL_SYNC_LENGTH_OF_DATA_POSITION		1
#define DYNAMIXEL_SYNC_LENGTH_OF_DATA_WIDTH			1

#define DYNAMIXEL_BROADCASTING_ID					0xfe

#define DYNAMIXEL_MODEL_NUMBER_L					0x00
#define DYNAMIXEL_MODEL_NUMBER_H					0x01

#define DYNAMIXEL_MAX_NUM							253

/** @brief Check for the presence of a device on the serial bus.
 *
 * 	This function tests for the presence of a dynamixel device on the
 * 	serial bus.
 *
 *	@param	byte		The ID of the device for which to check.
 * 	@return bool		True if a device is present, false otherwise.
 *
 */
bool dynamixelsPing(byte bID);

/** @brief Identify if a device on the serial bus is a given type.
 *
 * 	This function identifies if a device on the serial bus is a given
 * 	type (whether it is an AX-12 servo, an AX-S1 sensor, etc...)
 *
 *	@param	byte		The ID of the device for which we want to identify type.
 *	@param	uint16_t	The model number with which to compare the model number
 *						returned by the device.
 * 	@return bool		True if a device is present and has a device model number
 * 						which matches that for which we queried, false otherwise.
 *
 */
bool dynamixelsIsType(byte bID, uint16_t dynamixelType);

#endif /* DYNAMIXELS_H_ */
