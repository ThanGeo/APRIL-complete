#ifndef APRIL_CONTROLLER_H
#define APRIL_CONTROLLER_H

#include "SpatialLib.h"
#include <fstream>

#include "utils.h"
#include "join.h"

namespace APRIL
{
    extern spatial_lib::AprilConfigT* g_aprilConfig;
    extern spatial_lib::QueryT* g_query;

    /**
     * Sets up the APRIL intermediate filter.
     * TODO: if R and S have different april config, i might need to make a new method
    */
    void setupAPRILIntermediateFilter(spatial_lib::QueryT *query);

    /**
     * Entrypoint function for the intermediate filter.
     * 
    */
    void intermediateFilterEntrypoint(uint idR, uint idS);
}

#endif