#include "refinement.h"

namespace spatial_lib
{
    int refineIntersection(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::intersects(*polygonR, *polygonS);
    }

    int refineWithin(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::within(*polygonR, *polygonS);
    }
}