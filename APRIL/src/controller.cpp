#include "controller.h"

namespace APRIL
{
    /**
     * GLOBALS
    */
    spatial_lib::AprilConfigT* g_aprilConfig;
    spatial_lib::QueryT* g_query;
    // pipeline functions set at runtime by configuration
    int (*april_filter_function_ptr)(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS);
    int (*refinement_function_ptr)(spatial_lib::bg_polygon *boostPolygonR, spatial_lib::bg_polygon *boostPolygonS);

    // input fstream for vector data
    std::ifstream finR;
    std::ifstream finS;
    //offset maps for binary geometries
    std::unordered_map<uint,unsigned long> offsetMapR;
    std::unordered_map<uint,unsigned long> offsetMapS;

    static void setupIntersectionFilter() {
        april_filter_function_ptr = &intersectionJoinAPRILUncompressed;
        refinement_function_ptr = &spatial_lib::refineIntersection;
        // open data files and keep open
        finR.open(g_query->R.path, std::fstream::in | std::ios_base::binary);
        finS.open(g_query->S.path, std::fstream::in | std::ios_base::binary);
        // load offset maps
        offsetMapR = loadOffsetMap(g_query->R.offsetMapPath);
        offsetMapS = loadOffsetMap(g_query->S.offsetMapPath);
    }

    static void setupWithinFilter() {
        april_filter_function_ptr = &withinJoinAPRILUncompressed;
        refinement_function_ptr = &spatial_lib::refineWithin;
        // open data files and keep open
        finR.open(g_query->R.path, std::fstream::in | std::ios_base::binary);
        finS.open(g_query->S.path, std::fstream::in | std::ios_base::binary);
        // load offset maps
        offsetMapR = loadOffsetMap(g_query->R.offsetMapPath);
        offsetMapS = loadOffsetMap(g_query->S.offsetMapPath);
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
        // if (aprilR == NULL) {
        //     printf("problem with R id %d\n", idR);
        //     exit(-1);
        // }
        spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataOfObject(g_query->S, idS);
        // if (aprilS == NULL) {
        //     printf("problem with S id %d\n", idS);
        //     exit(-1);
        // }

        // use APRIL intermediate filter
        int iFilterResult = (*april_filter_function_ptr)(aprilR, aprilS);
        // count result
        spatial_lib::countAPRILResult(iFilterResult);
        if (iFilterResult != spatial_lib::INCONCLUSIVE) {
            // true negative or true hit, return
            return;
        } else {
            // load geometries for refinement
            spatial_lib::bg_polygon* boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
            spatial_lib::bg_polygon* boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);
            
            // refine
            int refinementResult = (*refinement_function_ptr)(boostPolygonR, boostPolygonS);
            // count result
            if (refinementResult) {
                spatial_lib::countResult();
            }
            // free memory
            delete boostPolygonR;
            delete boostPolygonS;
        }
    }
}