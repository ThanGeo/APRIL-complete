#ifndef APRIL_TOPOLOGY_H
#define APRIL_TOPOLOGY_H

#include "SpatialLib.h"
#include "join.h"

namespace APRIL
{
    int findRelationAPRILUncompressed(uint idR, uint idS, spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS);
}


#endif