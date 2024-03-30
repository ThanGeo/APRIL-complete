#include "refinement.h"

namespace spatial_lib
{
    // refinement function pointer
    int (*refinement_function_ptr)(spatial_lib::bg_polygon *boostPolygonR, spatial_lib::bg_polygon *boostPolygonS);
    // input fstream for vector data
    std::ifstream finR;
    std::ifstream finS;
    //offset maps for binary geometries
    std::unordered_map<uint,unsigned long> offsetMapR;
    std::unordered_map<uint,unsigned long> offsetMapS;


    static int refineIntersection(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::intersects(*polygonR, *polygonS);
    }

    static int refineWithin(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::within(*polygonR, *polygonS);
    }

    void refinementEntrypoint(uint idR, uint idS) {
        // count post mbr candidate
        spatial_lib::g_queryOutput.postMBRFilterCandidates += 1;
        spatial_lib::g_queryOutput.refinementCandidates += 1;

        refine(idR, idS);
    }

    void refine(uint idR, uint idS) {
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

    void setupRefinement(QueryT &query){
        // open data files and keep open
        finR.open(query.R.path, std::fstream::in | std::ios_base::binary);
        finS.open(query.S.path, std::fstream::in | std::ios_base::binary);
        // load offset maps
        offsetMapR = loadOffsetMap(query.R.offsetMapPath);
        offsetMapS = loadOffsetMap(query.S.offsetMapPath);

        switch(query.type) {
            case Q_INTERSECTION_JOIN:
                refinement_function_ptr = &refineIntersection;
                break;
            case Q_WITHIN_JOIN:
                refinement_function_ptr = &refineWithin;
                break;
            default:
                printf("Refinement for %d not supported yet.", query.type);
                break;
        }
    }

}