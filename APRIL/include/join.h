#ifndef APRIL_JOIN_H
#define APRIL_JOIN_H

#include "SpatialLib.h"
#include <sys/types.h>

namespace APRIL
{
    /**
     * joins two interval lists for intersection
    */
    int intersectionJoinIntervalLists(std::vector<uint> &ar1, uint &numintervals1, std::vector<uint> &ar2, uint &numintervals2);


    /**
     * joins two APRIL approximations to detect true hits, true negatives and inconclusive cases for INTERSECTION.
    */
    int intersectionJoinAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS);

    /**
     * joins two APRIL approximations to detect true hits, true negatives and inconclusive cases for WITHIN (R inside S).
    */
    int withinJoinAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS);

    /**
     * joins two interval lists to detect symmetrical containment (R inside S, S inside R) and if there is at least
     * 1 intersection.
     * returns a spatial_lib::IntervalListsRelationshipE value indicating the lists' relationships
    */
    int joinIntervalListsSymmetrical(std::vector<uint> &ar1, uint &numintervals1, std::vector<uint> &ar2, uint &numintervals2);

    /**
     * joins two interval lists to detect if they match 100%
    */
    int joinIntervalsForMatch(std::vector<uint> &ar1, uint &numintervals1, std::vector<uint> &ar2, uint &numintervals2);


    /**
     * joins two interval lists for containment "R inside S" and at least 1 intersection.
     * returns a spatial_lib::IntervalListsRelationshipE value indicating the lists' relationships.
     * It is hybrid for optimization: after non-containment is found, continue looking for at least 1 intersection in a different way.
    */
    int joinIntervalsHybrid(std::vector<uint> &ar1, uint &numintervals1, std::vector<uint> &ar2, uint &numintervals2);
}

#endif