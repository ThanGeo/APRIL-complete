#include "controller.h"

namespace APRIL
{
    /**
     * GLOBALS
    */
    spatial_lib::QueryT* g_query;
    // pipeline functions set at runtime by configuration
    // void (*april_query_function_ptr)(uint idR, uint idS, spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS);
    // int (*april_filter_function_ptr)(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS);

    // void (*refinement_gateway_ptr)(uint idR, uint idS);

    /************************************************/
    static void APRILIntersectionFilter(uint idR, uint idS, spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS) {
        // use APRIL intermediate filter
        int iFilterResult = intersectionJoinAPRILUncompressed(aprilR, aprilS);
        // count result
        spatial_lib::countAPRILResult(iFilterResult);
        if (iFilterResult != spatial_lib::INCONCLUSIVE) {
            // true negative or true hit, return
            return;
        }
        // i filter ended
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);

        // refinement
        spatial_lib::time::markRefinementFilterTimer();
        spatial_lib::refineIntersectionJoin(idR, idS);
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    static void APRILWithinFilter(uint idR, uint idS, spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS) {
        // use APRIL intermediate filter
        int iFilterResult = withinJoinAPRILUncompressed(aprilR, aprilS);
        // count result
        spatial_lib::countAPRILResult(iFilterResult);
        if (iFilterResult != spatial_lib::INCONCLUSIVE) {
            // true negative or true hit, return
            return;
        }
        // i filter ended
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);

        // refinement
        spatial_lib::time::markRefinementFilterTimer();
        spatial_lib::refineWithinJoin(idR, idS);
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    void APRILFindTopology(uint idR, uint idS, spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS) {
        // use APRIL intermediate filter
        int iFilterResult = findRelationAPRILUncompressed(idR, idS, aprilR, aprilS);

        int relation;
        // switch based on result
        switch(iFilterResult) {
            // true hits, count and return
            case spatial_lib::TR_INSIDE:
            case spatial_lib::TR_CONTAINS:
            case spatial_lib::TR_EQUAL:
            case spatial_lib::TR_DISJOINT:
            case spatial_lib::TR_INTERSECTS:
                // result
                spatial_lib::countTopologyRelationResult(iFilterResult);
                // time
                spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
                return;
                break;
            
            // inconclusive, do selective refinement
            // result holds what type of refinement needs to be made
            case spatial_lib::REFINE_ALL_NO_EQUAL:
                // time 
                spatial_lib::time::markRefinementFilterTimer();
                // refine
                relation = spatial_lib::refineAllRelationsNoEqual(idR, idS);
                break;
            
            case spatial_lib::REFINE_CONTAIN_PLUS:
                // time
                spatial_lib::time::markRefinementFilterTimer();
                // refine
                relation = spatial_lib::refineContainsPlus(idR, idS);
                break;
            
            case spatial_lib::REFINE_CONTAINMENT_ONLY:
                // time
                spatial_lib::time::markRefinementFilterTimer();
                // refine
                relation = spatial_lib::refineContainmentsOnly(idR, idS);
                break;
            
            case spatial_lib::REFINE_INSIDE_PLUS:
                // time
                spatial_lib::time::markRefinementFilterTimer();
                // refine
                relation = spatial_lib::refineInsidePlus(idR, idS);
                break;
            
            case spatial_lib::REFINE_NO_CONTAINMENT:
                // time
                spatial_lib::time::markRefinementFilterTimer();
                // refine
                relation = spatial_lib::refineGuaranteedNoContainment(idR, idS);
                break;
        }
        // time
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
        // count the result
        spatial_lib::g_queryOutput.refinementCandidates+=1;
        spatial_lib::countTopologyRelationResult(relation);

    }

    void setupAPRILIntermediateFilter(spatial_lib::QueryT *query) {
        // store query info
        g_query = query;
    }


    void IntermediateFilterEntrypoint(uint idR, uint idS) {
        // time 
        spatial_lib::time::markiFilterTimer();
        
        // count post mbr candidate
        spatial_lib::g_queryOutput.postMBRFilterCandidates += 1;
        // fetch the APRIL of R and S
        spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataOfObjectFromDatasetMap(g_query->R, idR);
        spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataOfObjectFromDatasetMap(g_query->S, idS);
        // use appropriate query function

        switch (g_query->type) {
            case spatial_lib::Q_INTERSECTION_JOIN:
                APRILIntersectionFilter(idR, idS, aprilR, aprilS);
                break;
            case spatial_lib::Q_WITHIN_JOIN:
                APRILWithinFilter(idR, idS, aprilR, aprilS);
                break;
            case spatial_lib::Q_FIND_RELATION:
                APRILFindTopology(idR, idS, aprilR, aprilS);
                break;
            default:
                // not supported/unknown
                exit(-1);
                break;
        }

    }
}