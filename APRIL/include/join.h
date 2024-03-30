#ifndef APRIL_JOIN_H
#define APRIL_JOIN_H

#include "SpatialLib.h"
#include <sys/types.h>

namespace APRIL
{

    /**
     * joins two APRIL approximations to detect true hits, true negatives and inconclusive cases for INTERSECTION.
    */
    int intersectionJoinAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS);

    /**
     * joins two APRIL approximations to detect true hits, true negatives and inconclusive cases for WITHIN (R inside S).
    */
    int withinJoinAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS);
}

#endif