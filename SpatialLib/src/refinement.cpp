#include "refinement.h"

namespace spatial_lib
{   
    /**
     * TOPOLOGY RELATION DE-9IM CODES
    */
    char insideCode[] = "T*F**F***";
    char coveredbyCode1[] = "T*F**F***";
    char coveredbyCode2[] = "*TF**F***";
    char coveredbyCode3[] = "**FT*F***";
    char coveredbyCode4[] = "**F*TF***";
    char containsCode[] = "T*****FF*";
    char coversCode1[] = "T*****FF*";
    char coversCode2[] = "*T****FF*";
    char coversCode3[] = "***T**FF*";
    char coversCode4[] = "****T*FF*";
    char meetCode1[] = "FT*******"; 
    char meetCode2[] = "F**T*****"; 
    char meetCode3[] = "F***T****"; 
    char equalCode[] = "T*F**FFF*"; 
    char disjointCode[] = "FF*FF****";
    char intersectCode1[] = "T********";
    char intersectCode2[] = "*T*******";
    char intersectCode3[] = "***T*****";
    char intersectCode4[] = "****T****";

    //define topological masks for refinement
    // a within b
    boost::geometry::de9im::mask withinMask(insideCode); 
    // a contains b
    boost::geometry::de9im::mask containsMask(containsCode); 
    // a covered by b
    std::vector<boost::geometry::de9im::mask> coveredByMaskList = {
                    boost::geometry::de9im::mask(coveredbyCode1),
                    boost::geometry::de9im::mask(coveredbyCode2),
                    boost::geometry::de9im::mask(coveredbyCode3),
                    boost::geometry::de9im::mask(coveredbyCode4)};
    // a covers b
    std::vector<boost::geometry::de9im::mask> coversMaskList = {
                    boost::geometry::de9im::mask(coversCode1),
                    boost::geometry::de9im::mask(coversCode2),
                    boost::geometry::de9im::mask(coversCode3),
                    boost::geometry::de9im::mask(coversCode4)};
    // a and b meet
    boost::geometry::de9im::mask meetMask1(meetCode1); 
    boost::geometry::de9im::mask meetMask2(meetCode2); 
    boost::geometry::de9im::mask meetMask3(meetCode3); 
    // a and b are equal
    boost::geometry::de9im::mask equalMask(equalCode); 
    // a and b are disjoint
    boost::geometry::de9im::mask disjointMask(disjointCode); 
    // a overlaps b
    std::vector<boost::geometry::de9im::mask> overlapMaskList = {
                    boost::geometry::de9im::mask(intersectCode1),
                    boost::geometry::de9im::mask(intersectCode2),
                    boost::geometry::de9im::mask(intersectCode3),
                    boost::geometry::de9im::mask(intersectCode4)};

    // refinement function pointers
    void (*refinement_entrypoint_switch_ptr)(uint idR, uint idS);
    int (*refinement_join_function_ptr)(spatial_lib::bg_polygon *boostPolygonR, spatial_lib::bg_polygon *boostPolygonS);
    int (*refinement_topology_function_ptr)(spatial_lib::bg_polygon *boostPolygonR, spatial_lib::bg_polygon *boostPolygonS, bool markedForEqual);
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

    static bool compareDe9imChars(char character, char char_mask) {
        if (character != 'F' && char_mask == 'T') {
            // character is 0,1,2 and char_mask is T
            return true;
        } else if (character == 'F' && char_mask == 'F'){
            // both are F
            return true;
        } else {
            // no match
            return false;
        }
    }

    static bool compareMasks(std::string &de9imCode, char* maskCode) {
        for(int i=0; i<9; i++) {
            if (de9imCode[i] == '*' || maskCode[i] == '*' || compareDe9imChars(de9imCode[i], maskCode[i])){
                continue;
            } else {
                return false;
            }
        }
        return true;
    }

    static std::string createMaskCode(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS) {
        boost::geometry::de9im::matrix matrix = boost::geometry::relation(*polygonR, *polygonS);
        return matrix.str();
    }

    static int refineFindRelation(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS, bool markedForEqual){
        // get the mask codes
        std::string code = createMaskCode(polygonR, polygonS);
        
        //disjoint
        if(compareMasks(code, disjointCode)){
            return TR_DISJOINT;
        }

        /**
         * it definitely intersects at this point
        */

        if (markedForEqual) {
            // check equality first because it is a subset of covers and covered by
            if(compareMasks(code, equalCode)){
                return TR_EQUAL;
            }
        }

        // covers
        if(compareMasks(code, coversCode1) || compareMasks(code, coversCode2) || compareMasks(code, coversCode3)|| compareMasks(code, coversCode4)){
            // first check contains because it is a subset of covers
            if(compareMasks(code, containsCode)){
                return TR_CONTAINS;
            }
            return TR_COVERS;
        }

        // covered by
        if(compareMasks(code, coveredbyCode1) || compareMasks(code, coveredbyCode2) || compareMasks(code, coveredbyCode3)|| compareMasks(code, coveredbyCode4)){
            // first check within because it is a subset of covered by
            if(compareMasks(code, insideCode)){
                return TR_INSIDE;
            }
            return TR_COVERED_BY;
        }

        // meet
        if(compareMasks(code, meetCode1) || 
                    compareMasks(code, meetCode2) || 
                    compareMasks(code, meetCode3)){
            return TR_MEET;
        }

        // else return intersects
        return TR_INTERSECTS;
    }

    void refinementEntrypoint(uint idR, uint idS) {
        // count post mbr candidate
        spatial_lib::g_queryOutput.postMBRFilterCandidates += 1;
        spatial_lib::g_queryOutput.refinementCandidates += 1;

        (*refinement_entrypoint_switch_ptr)(idR, idS);
    }

    void refineJoin(uint idR, uint idS) {
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);
        
        // refine
        int refinementResult = (*refinement_join_function_ptr)(&boostPolygonR, &boostPolygonS);
        // count result
        if (refinementResult) {
            spatial_lib::countResult();
        }
    }

    void refineTopology(uint idR, uint idS) {
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);
        
        // refine
        int refinementResult = (*refinement_topology_function_ptr)(&boostPolygonR, &boostPolygonS, true);
        // if (refinementResult == TR_INSIDE) {
        //     printf("%d inside %d\n", idR, idS);
        // }
        // count result
        spatial_lib::countTopologyRelationResult(refinementResult);
    }

    int refineAllRelationsNoEqual(uint idR, uint idS) {
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);
        
        return refineFindRelation(&boostPolygonR, &boostPolygonS, false);
    }

    bool refineEqual(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        return boost::geometry::equals(boostPolygonR, boostPolygonS);
    }


    int refineGuaranteedNoContainment(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        // get the mask code
	    std::string code = createMaskCode(&boostPolygonR, &boostPolygonS);

        if (compareMasks(code, disjointCode)) {
            return TR_DISJOINT;
        }

        if (compareMasks(code, meetCode1) || compareMasks(code, meetCode2) || compareMasks(code, meetCode3)) {
            return TR_MEET;
        }

        return TR_INTERSECTS;
    }


    int refineContainmentsOnly(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        // if (idR == 117844 && idS == 2174856) {
        //     printf("polygon %d\n", idR);
        //     for (auto it : boostPolygonR.outer()) {
        //         printf("(%f,%f),", it.x(), it.y());
        //     }
        //     printf("\n");

        //     printf("polygon %d\n", idS);
        //     for (auto it : boostPolygonS.outer()) {
        //         printf("(%f,%f),", it.x(), it.y());
        //     }
        //     printf("\n");
        // }

        // get the mask code
	    std::string code = createMaskCode(&boostPolygonR, &boostPolygonS);

        if (compareMasks(code, coversCode1) || compareMasks(code, coversCode2) || 
            compareMasks(code, coversCode3) || compareMasks(code, coversCode4)) {
            if (compareMasks(code, containsCode)) {
                return TR_CONTAINS;
            }
            return TR_COVERS;
        }

        if (compareMasks(code, coveredbyCode1) || compareMasks(code, coveredbyCode2) || 
            compareMasks(code, coveredbyCode3) || compareMasks(code, coveredbyCode4)) {
            if (compareMasks(code, insideCode)) {
                return TR_INSIDE;
            }
            return TR_COVERED_BY;
        }

        return TR_INTERSECTS;
    }


    int refineContainsPlus(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        // get the mask code
	    std::string code = createMaskCode(&boostPolygonR, &boostPolygonS);

        if (compareMasks(code, disjointCode)) {
            return TR_DISJOINT;
        }

        if (compareMasks(code, coversCode1) || compareMasks(code, coversCode2) || 
            compareMasks(code, coversCode3) || compareMasks(code, coversCode4)) {
            if (compareMasks(code, containsCode)) {
                return TR_CONTAINS;
            }
            return TR_COVERS;
        }

        if (compareMasks(code, meetCode1) || compareMasks(code, meetCode2) || compareMasks(code, meetCode3)) {
            return TR_MEET;
        }

        return TR_INTERSECTS;
    }

    int refineInsidePlus(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        // get the mask code
	    std::string code = createMaskCode(&boostPolygonR, &boostPolygonS);

        if (compareMasks(code, disjointCode)) {
            return TR_DISJOINT;
        }

        if (compareMasks(code, coveredbyCode1) || compareMasks(code, coveredbyCode2) || 
            compareMasks(code, coveredbyCode3) || compareMasks(code, coveredbyCode4)) {
            if (compareMasks(code, insideCode)) {
                return TR_INSIDE;
            }
            return TR_COVERED_BY;
        }

        if (compareMasks(code, meetCode1) || compareMasks(code, meetCode2) || compareMasks(code, meetCode3)) {
            return TR_MEET;
        }

        return TR_INTERSECTS;
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
                refinement_entrypoint_switch_ptr = &refineJoin;
                refinement_join_function_ptr = &refineIntersection;
                break;
            case Q_WITHIN_JOIN:
                refinement_entrypoint_switch_ptr = &refineJoin;
                refinement_join_function_ptr = &refineWithin;
                break;
            case Q_FIND_RELATION:
                refinement_entrypoint_switch_ptr = &refineTopology;
                refinement_topology_function_ptr = &refineFindRelation;
                break;
            default:
                printf("Refinement for %d not supported yet.", query.type);
                break;
        }
    }

}