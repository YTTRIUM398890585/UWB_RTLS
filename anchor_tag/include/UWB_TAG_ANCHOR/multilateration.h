#pragma once

#include "UWB_TAG_ANCHOR/utils.h"
#include "UWB_TAG_ANCHOR/link.h"

#include <ArduinoEigen.h>
using namespace Eigen;

#ifdef IS_TAG
Vector3f multilateration(AnchorLinkedList& mul_data, bool debugPrints);
float filterAverageDistance(float *p_distances);
#endif