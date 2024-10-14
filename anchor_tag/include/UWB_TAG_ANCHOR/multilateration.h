#pragma once

#include "UWB_TAG_ANCHOR/utils.h"
#include "UWB_TAG_ANCHOR/link.h"

extern AnchorLinkedList mul_data;

#include <ArduinoEigen.h>
using namespace Eigen;

#ifdef IS_TAG
Vector3f multilateration(AnchorLinkedList& mul_data, bool debugPrints);
#endif