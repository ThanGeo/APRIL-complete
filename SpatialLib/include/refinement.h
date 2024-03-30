#ifndef SPATIAL_LIB_REFINEMENT_H
#define SPATIAL_LIB_REFINEMENT_H

#include "def.h"

namespace spatial_lib 
{
    int refineIntersection(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS);
    int refineWithin(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS);
}

#endif