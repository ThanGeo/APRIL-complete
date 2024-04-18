#ifndef APRIL_CONTROLLER_H
#define APRIL_CONTROLLER_H

#include "SpatialLib.h"
#include "Rasterizer.h"
#include <fstream>

#include "utils.h"
#include "join.h"
#include "topology.h"

namespace APRIL
{
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
    void IntermediateFilterEntrypoint(uint idR, uint idS);


    /**
     * @brief Entrypoint function for the find relation intermediate filter
     * 
     * @param idR 
     * @param idS 
     * @param relationCase 
     */
    void IntermediateFilterFindRelationEntrypoint(uint idR, uint idS, int relationCase);

    /**
     * @brief Standard APRIL intermediate filter that detects disjoint and intersection pairs,
     * but used for find relation queries. Non-disjoint pairs are refined using DE-9IM
     * 
     * 
     * @param idR 
     * @param idS 
     */
    void StandardIntermediateFilterEntrypoint(uint idR, uint idS);

    /**
     * @brief Intermediate filter called after the specialized find relation MBR Filter.
     * Does not require APRIL pre-processing. Creates APRIL ON THE FLY!!!
     * 
     * @param idR 
     * @param idS 
     * @param relationCase 
     */
    void IntermediateFilterOnTheFlyEntrypoint(uint idR, uint idS, int relationCase);
}

#endif