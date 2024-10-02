#pragma once

#include "UWB_TAG_ANCHOR/DW1000Handlers.h"
#include "UWB_TAG_ANCHOR/utils.h"

#include <ArduinoEigen.h>
using namespace Eigen;

#ifdef IS_TAG
Vector3f multilateration(AnchorLinkedList& uwb_data, bool debugPrints);
#endif