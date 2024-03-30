#ifndef SPATIAL_LIB_REFINEMENT_H
#define SPATIAL_LIB_REFINEMENT_H

#include "def.h"

namespace spatial_lib 
{
    void setupRefinement(QueryT &query);

    /**
     * Entrypoint function for when there is NO intermediate filter.
     * Intermediate filters forward to refine() function and NOT this one.
    */
    void refinementEntrypoint(uint idR, uint idS);

    /**
     * Loads geometries and refines based on setup configuration.
     * Difference with refineEntrypoint function is that this function does not collect statistics apart
     * from the true hits after the refinement.
    */
    void refine(uint idR, uint idS);

}

#endif