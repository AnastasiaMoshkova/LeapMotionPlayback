/******************************************************************************\
* Copyright (C) 2012-2017 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/

#undef __cplusplus

#include <stdio.h>
#include <stdlib.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>

//#include <nlohmann\json.hpp>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "LeapC.h"
#include "ExampleConnection.h"
#include <conio.h>


//#include <iostream>
//#include <fstream>
//#include <string>

int64_t lastFrameID = 0; //The last frame received

int main(int argc, char** argv) {
	OpenConnection();
	while (!IsConnected)
		millisleep(100); //wait a bit to let the connection complete

	printf("Connected.\n");
	LEAP_DEVICE_INFO* deviceProps = GetDeviceProperties();
	if (deviceProps)
		printf("Using device %s.\n", deviceProps->serial);

	LEAP_RECORDING recordingHandle;
	LEAP_RECORDING_PARAMETERS params;
	unsigned char key;

		//Reopen the recording for reading
	params.mode = eLeapRecordingFlags_Reading;
	eLeapRS result = LeapRecordingOpen(&recordingHandle, argv[1], params);
	if (LEAP_SUCCEEDED(result)) {
		LEAP_TRACKING_EVENT* frame = 0;
		while (true) {
			uint64_t nextFrameSize = 0;
			result = LeapRecordingReadSize(recordingHandle, &nextFrameSize);
			if (!LEAP_SUCCEEDED(result))
			{
				printf("Couldn't get next frame size: %s\n", ResultString(result));
				break;
			}
			if (nextFrameSize > 0) {
				frame = (LEAP_TRACKING_EVENT*)malloc(nextFrameSize);
				result = LeapRecordingRead(recordingHandle, frame, nextFrameSize);
				if (LEAP_SUCCEEDED(result)) {
					printf("Read frame %"PRIu64" with %i hands.\n", frame->tracking_frame_id, frame->nHands);

					FILE* file = fopen(argv[2], "a");

					static int i;
					i++;
					fprintf(file, "    frame %i.\n",i);
					printf("Frame %lli with %i hands.\n", (long long int)frame->info.frame_id, frame->nHands);

					for (uint32_t h = 0; h < frame->nHands; h++) {

						float version = 2.2;
						long int timestamp = frame->info.timestamp;
						long int frame_id = frame->info.frame_id;
						long int tracking_frame_id = frame->tracking_frame_id;
						float framerate = frame->framerate;

						LEAP_HAND* hand = &frame->pHands[h];


						printf("    Hand id %i is a %s hand with position (%f, %f, %f,%f, %f, %f, %f).\n",
							hand->id,
							(hand->type == eLeapHandType_Left ? "left" : "right"),
							hand->palm.position.x,
							hand->palm.position.y,
							hand->palm.position.z,
							hand->palm.orientation.w,
							hand->palm.orientation.x,
							hand->palm.orientation.y,
							hand->palm.orientation.z);


						fprintf(file, "    Hand id %i is a %s hand with position (%f, %f, %f,%f, %f, %f, %f).\n",
							hand->id,
							(hand->type == eLeapHandType_Left ? "left" : "right"),
							hand->palm.position.x,
							hand->palm.position.y,
							hand->palm.position.z,
							hand->palm.orientation.w,
							hand->palm.orientation.x,
							hand->palm.orientation.y,
							hand->palm.orientation.z);

						fprintf(file, "    Confidence %f id_img %i visible_time %li pinch_distance %f pinch_strength %f grab_angle %f grab_strength %f palm_width %f timestamp %li img_id %li tracking_img_id %li fps %f version %f .\n",
							hand->confidence,
							hand->id,
							hand->visible_time,
							hand->pinch_distance,
							hand->pinch_strength,
							hand->grab_angle,
							hand->grab_strength,
							hand->palm.width,
							timestamp,
							frame_id,
							tracking_frame_id,
							framerate,
							version);




						//-------------------
						for (int f = 0; f < 5; f++) {
							LEAP_DIGIT finger = hand->digits[f];
							printf("    Finger %i.\n",
								f);

							for (int b = 0; b < 4; b++) {
								LEAP_BONE bone = finger.bones[b];
								/*
								0-THUMB
								1-FORE-Finger
								2-Middle
								3-ring_finger
								4-pinky_little_finger
								*/

								printf("    bone with position (%f, %f, %f, %f, %f, %f, %f, %f, %f).\n",
									bone.next_joint.x,
									bone.next_joint.y,
									bone.next_joint.z,
									bone.prev_joint.x,
									bone.prev_joint.y,
									bone.prev_joint.z,
									bone.rotation.x,
									bone.rotation.y,
									bone.rotation.z);
								fprintf(file, "    bone with position (%f, %f, %f, %f, %f, %f, %f, %f, %f, %f).\n",
									bone.next_joint.x,
									bone.next_joint.y,
									bone.next_joint.z,
									bone.prev_joint.x,
									bone.prev_joint.y,
									bone.prev_joint.z,
									bone.rotation.x,
									bone.rotation.y,
									bone.rotation.z,
									bone.rotation.w); /*новое добавление*/
								/*
								0-JOINT_MCP
								1-JOINT_PIP
								2-JOINT_DIP
								3-JOINT_TIP
								*/

							}
						}
						//--------------------
					}

					fclose(file);
					//---------------
				}
				else {
					break;
					printf("Could not read frame: %s\n", ResultString(result));
				}
			}
		}
		result = LeapRecordingClose(&recordingHandle);
		if (!LEAP_SUCCEEDED(result))
			printf("Failed to close recording: %s\n", ResultString(result));
	}
	else {
		printf("Failed to open recording for reading: %s\n", ResultString(result));
	}
	/*} else {
	  printf("Failed to open recording for writing: %s\n", ResultString(result));
	}*/
	return 0;
}
//End-of-Sample
