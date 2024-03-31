#include "controller.h"

namespace APRIL
{
    /**
     * GLOBALS
    */
    spatial_lib::AprilConfigT* g_aprilConfig;
    spatial_lib::QueryT* g_query;
    // pipeline functions set at runtime by configuration
    void (*april_query_function_ptr)(uint idR, uint idS, spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS);
    int (*april_filter_function_ptr)(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS);

    void (*refinement_gateway_ptr)(uint idR, uint idS);

    /************************************************/
    void registerRefinementFunction(void (*funcPtr)(uint idR, uint idS)){
        refinement_gateway_ptr = funcPtr;
    }

    void APRILJoin(uint idR, uint idS, spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS) {
        // use APRIL intermediate filter
        int iFilterResult = (*april_filter_function_ptr)(aprilR, aprilS);
        // count result
        spatial_lib::countAPRILResult(iFilterResult);
        if (iFilterResult != spatial_lib::INCONCLUSIVE) {
            // true negative or true hit, return
            return;
        }
        if (refinement_gateway_ptr != NULL) {
            // refine
            (*refinement_gateway_ptr)(idR, idS);     
        }
    }

    void APRILFindTopology(uint idR, uint idS, spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS) {
        // int iFilterResult = spatial_lib::REFINE_ALL_NO_EQUAL;

        int iFilterResult = findRelationAPRILUncompressed(idR, idS, aprilR, aprilS);

        int relation;
        switch(iFilterResult) {
            // true hits
            case spatial_lib::TR_INSIDE:
            case spatial_lib::TR_CONTAINS:
            case spatial_lib::TR_EQUAL:
            case spatial_lib::TR_DISJOINT:
            case spatial_lib::TR_INTERSECTS:
                spatial_lib::countTopologyRelationResult(iFilterResult);
                return;
                break;

            // selective refinements
            // result holds what type of refinement needs to be made
            case spatial_lib::REFINE_ALL_NO_EQUAL:
                relation = spatial_lib::refineAllRelationsNoEqual(idR, idS);
                break;
            
            case spatial_lib::REFINE_CONTAIN_PLUS:
                relation = spatial_lib::refineContainsPlus(idR, idS);
                break;
            
            case spatial_lib::REFINE_CONTAINMENT_ONLY:
                relation = spatial_lib::refineContainmentsOnly(idR, idS);
                break;
            
            case spatial_lib::REFINE_INSIDE_PLUS:
                relation = spatial_lib::refineInsidePlus(idR, idS);
                break;
            
            case spatial_lib::REFINE_NO_CONTAINMENT:
                relation = spatial_lib::refineGuaranteedNoContainment(idR, idS);
                break;
        }
        // count the result
        // printf("%d and %d refinement\n", idR, idS);

        spatial_lib::g_queryOutput.refinementCandidates+=1;
        spatial_lib::countTopologyRelationResult(relation);

    }


    static void setupIntersectionFilter() {
        april_query_function_ptr = &APRILJoin;
        april_filter_function_ptr = &intersectionJoinAPRILUncompressed;
    }

    static void setupWithinFilter() {
        april_query_function_ptr = &APRILJoin;
        april_filter_function_ptr = &withinJoinAPRILUncompressed;
    }

    static void setupFindRelationFilter() {
        april_query_function_ptr = &APRILFindTopology;
        
    }

    void setupAPRILIntermediateFilter(spatial_lib::QueryT *query) {
        // store config and query info
        g_query = query;
        g_aprilConfig = &query->R.aprilConfig;
        
        switch(query->type) {
            case spatial_lib::Q_INTERSECTION_JOIN:
                setupIntersectionFilter();
                break;
            case spatial_lib::Q_WITHIN_JOIN:
                setupWithinFilter();
                break;
            case spatial_lib::Q_FIND_RELATION:
                setupFindRelationFilter();
                break;
            default:
                printf("Query type '%s' not supported yet.\n", spatial_lib::queryTypeIntToText(query->type).c_str());
                exit(-1);
        }
    }


    void intermediateFilterEntrypoint(uint idR, uint idS) {
        // count post mbr candidate
        spatial_lib::g_queryOutput.postMBRFilterCandidates += 1;
        // fetch the APRIL of R and S
        spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataOfObject(g_query->R, idR);
        spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataOfObject(g_query->S, idS);
        // use appropriate query function
        (*april_query_function_ptr)(idR, idS, aprilR, aprilS);
    }
}