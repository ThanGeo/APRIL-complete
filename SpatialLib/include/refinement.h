#ifndef SPATIAL_LIB_REFINEMENT_H
#define SPATIAL_LIB_REFINEMENT_H

#include "def.h"
#include "timer.h"

namespace spatial_lib 
{
    extern QueryTypeE g_queryType;

    void setupRefinement(QueryT &query);

    /**
     * Entrypoint function for when there is NO intermediate filter.
     * Intermediate filters forward to refine() function and NOT this one.
    */
    void refinementEntrypoint(uint idR, uint idS);

    /**
     * loads boost geometries and refines for intersection
    */
    void refineIntersectionJoin(uint idR, uint idS);
    /**
     * loads boost geometries and refines for inside
    */
    void refineInsideJoin(uint idR, uint idS);

    /**
     * Loads geometries and refines for topology based on setup configuration.
    */
    void refineFindRelationJoin(uint idR, uint idS);

    /**
     * refinement for the EQUAl topology relation
    */
    void refineEqualJoin(uint idR, uint idS);

    /**
     * refinement for the DISJOINT topology relation
    */
    void refineDisjointJoin(uint idR, uint idS);

    /**
     * refinement for the MEET topology relation
    */
    void refineMeetJoin(uint idR, uint idS);

    /**
     * refinement for the CONTAINS topology relation
    */
    void refineContainsJoin(uint idR, uint idS);

    /**
     * refinement for the COVERS topology relation
    */
    void refineCoversJoin(uint idR, uint idS);

    /**
     * refinement for the COVERED BY topology relation
    */
    void refineCoveredByJoin(uint idR, uint idS);

    /**
     * refinement for the DISJOINT, MEET or INTERSECT topology relations
    */
    int refineGuaranteedNoContainment(uint idR, uint idS);


    /**
     * refinement for containment relations and intersect ONLY (Contains, Covers, Covered by, Inside, Intersect)
    */
    int refineContainmentsOnly(uint idR, uint idS);

    /**
     * refine DISJOINT, COVERS, CONTAINS, INTERSECT only
    */
    int refineContainsPlus(uint idR, uint idS);


    /**
     * refine DISJOINT, COVERED BY, INSIDE, INTERSECT only
    */
    int refineInsidePlus(uint idR, uint idS);

    /**
     * refine for EQUAl for APRIL
     */
    bool isEqual(uint idR, uint idS);

    /**
     * refines for ALL relations except EQUAL
    */
    int refineAllRelationsNoEqual(uint idR, uint idS);

}

#endif