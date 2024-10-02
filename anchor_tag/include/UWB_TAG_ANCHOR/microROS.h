#pragma once

#ifdef IS_TAG

#include <Arduino.h>
#include <micro_ros_platformio.h>

#include <rcl/rcl.h>
#include <rclc/rclc.h>

#include "micro_ros_interfaces/msg/tag_pub.h"

#include "UWB_TAG_ANCHOR/DW1000Handlers.h"

#include <ArduinoEigen.h>
using namespace Eigen;

extern rcl_publisher_t publisher;
extern rclc_support_t support;
extern rcl_allocator_t allocator;
extern rcl_node_t node;

#define RCCHECK(fn)                    \
	{                                  \
		rcl_ret_t temp_rc = fn;        \
		if ((temp_rc != RCL_RET_OK)) { \
			errorHandler();            \
		}                              \
	}
#define RCSOFTCHECK(fn)                \
	{                                  \
		rcl_ret_t temp_rc = fn;        \
		if ((temp_rc != RCL_RET_OK)) { \
		}                              \
	}

void errorHandler();
void setupMicroRos();
void rosPublishLocation(AnchorLinkedList& uwb_data, Vector3f tagCoords);

#endif