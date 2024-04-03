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
    // a inside b
    boost::geometry::de9im::mask insideMask(insideCode); 
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

    // input fstream for vector data
    std::ifstream finR;
    std::ifstream finS;
    //offset maps for binary geometries
    std::unordered_map<uint,unsigned long> offsetMapR;
    std::unordered_map<uint,unsigned long> offsetMapS;
    
    QueryTypeE g_queryType;


    /**
     * Boost geometry refinement for specific relations
     */

    static int refineIntersection(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::intersects(*polygonR, *polygonS);
    }

    static int refineInside(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::within(*polygonR, *polygonS);
    }

    static int refineContains(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::within(*polygonS, *polygonR);
    }

    static int refineDisjoint(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::disjoint(*polygonR, *polygonS);
    }

    static int refineEqual(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::equals(*polygonR, *polygonS);
    }

    static int refineMeet(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::touches(*polygonR, *polygonS);
    }

    static int refineCovers(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::covered_by(*polygonS, *polygonR);
    }

    static int refineCoveredBy(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS){
        return boost::geometry::covered_by(*polygonR, *polygonS);
    }

    /**
     * Boost geometry refinement for find relation
     */


    static inline bool compareDe9imChars(char character, char char_mask) {
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

    static inline bool compareMasks(std::string &de9imCode, char* maskCode) {
        for(int i=0; i<9; i++) {
            if (de9imCode[i] == '*' || maskCode[i] == '*' || compareDe9imChars(de9imCode[i], maskCode[i])){
                continue;
            } else {
                return false;
            }
        }
        return true;
    }

    static inline std::string createMaskCode(spatial_lib::bg_polygon *polygonR, spatial_lib::bg_polygon *polygonS) {
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
            // first check inside because it is a subset of covered by
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

    /**
     * Refine find specific relation
     */

    void refineIntersectionJoin(uint idR, uint idS) {
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);
        
        // refine
        int refinementResult = refineIntersection(&boostPolygonR, &boostPolygonS);
        // count result
        if (refinementResult) {
            spatial_lib::countResult();
        }
    }

    void refineInsideJoin(uint idR, uint idS) {
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);
        
        // refine
        int refinementResult = refineInside(&boostPolygonR, &boostPolygonS);
        // count result
        if (refinementResult) {
            spatial_lib::countResult();
        }
    }

    void refineDisjointJoin(uint idR, uint idS) {
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);
        
        // refine
        int refinementResult = refineDisjoint(&boostPolygonR, &boostPolygonS);
        // count result
        if (refinementResult) {
            spatial_lib::countResult();
        }
    }

    void refineEqualJoin(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        // refine
        int refinementResult = refineEqual(&boostPolygonR, &boostPolygonS);
        // count result
        if (refinementResult) {
            spatial_lib::countResult();
        }
    }

    void refineMeetJoin(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        // refine
        int refinementResult = refineMeet(&boostPolygonR, &boostPolygonS);
        // count result
        if (refinementResult) {
            spatial_lib::countResult();
        }
    }

    void refineContainsJoin(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        // refine
        int refinementResult = refineContains(&boostPolygonR, &boostPolygonS);
        // count result
        if (refinementResult) {
            spatial_lib::countResult();
        }
    }

    void refineCoversJoin(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        // refine
        int refinementResult = refineCovers(&boostPolygonR, &boostPolygonS);
        // count result
        if (refinementResult) {
            spatial_lib::countResult();
        }
    }

    void refineCoveredByJoin(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        // refine
        int refinementResult = refineCoveredBy(&boostPolygonR, &boostPolygonS);
        // count result
        if (refinementResult) {
            spatial_lib::countResult();
        }
    }



    /**
     * Find Relation
     */

    void refineFindRelationJoin(uint idR, uint idS) {
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);
        
        // refine
        int refinementResult = refineFindRelation(&boostPolygonR, &boostPolygonS, true);

        // count result
        spatial_lib::countTopologyRelationResult(refinementResult);
    }

    /**
     * FOR APRIL
     */

    bool isEqual(uint idR, uint idS) {
        // load boost polygons
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);

        // refine
        return refineEqual(&boostPolygonR, &boostPolygonS);
    }

    int refineAllRelationsNoEqual(uint idR, uint idS) {
        spatial_lib::bg_polygon boostPolygonR = loadPolygonFromDiskBoostGeometry(idR, finR, offsetMapR);
        spatial_lib::bg_polygon boostPolygonS = loadPolygonFromDiskBoostGeometry(idS, finS, offsetMapS);
        
        return refineFindRelation(&boostPolygonR, &boostPolygonS, false);
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
























    void refinementEntrypoint(uint idR, uint idS) {
        // time
        spatial_lib::time::markRefinementFilterTimer();
        // count post mbr candidate
        spatial_lib::g_queryOutput.postMBRFilterCandidates += 1;
        spatial_lib::g_queryOutput.refinementCandidates += 1;
        
        switch(g_queryType) {
            case Q_INTERSECT:
                refineIntersectionJoin(idR, idS);
                break;
            case Q_INSIDE:
                refineInsideJoin(idR, idS);
                break;
            case Q_DISJOINT:
                refineDisjointJoin(idR, idS);
                break;
            case Q_EQUAL:
                refineEqualJoin(idR, idS);
                break;
            case Q_MEET:
                refineMeetJoin(idR, idS);
                break;
            case Q_CONTAINS:
                refineContainsJoin(idR, idS);
                break;
            case Q_COVERS:
                refineCoversJoin(idR, idS);
                break;
            case Q_COVERED_BY:
                refineCoveredByJoin(idR, idS);
                break;
            case Q_FIND_RELATION:
                refineFindRelationJoin(idR, idS);
                break;

            default:
                fprintf(stderr, "Failed. No refinement support for query type.\n");
                exit(-1);
                break;
        }
        // store time
        g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    void setupRefinement(QueryT &query){
        // open data files and keep open
        finR.open(query.R.path, std::fstream::in | std::ios_base::binary);
        finS.open(query.S.path, std::fstream::in | std::ios_base::binary);
        // load offset maps
        offsetMapR = loadOffsetMap(query.R.offsetMapPath);
        offsetMapS = loadOffsetMap(query.S.offsetMapPath);
        // setup query type
        g_queryType = query.type;
    }

}