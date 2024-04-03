#include "topology.h"


namespace APRIL 
{
    int findRelationAPRILUncompressed(uint idR, uint idS, spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS) {
        // AA join to look for exact relationship between the lists
        int AAresult = joinIntervalListsSymmetrical(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsALL, aprilS->numIntervalsALL);
        // printf("AA: %d\n", AAresult);
        if (AAresult == spatial_lib::IL_DISJOINT) {
            // true negative
            return spatial_lib::TR_DISJOINT;
        }
         
        if (AAresult == spatial_lib::IL_MATCH) {
            // AA symmetrical containment, happens in EQUAL
            // check if candidate for EQUAL
            if (aprilR->numIntervalsFULL && aprilS->numIntervalsFULL) {
                int FF = joinIntervalsForMatch(aprilR->intervalsFULL, aprilR->numIntervalsFULL, aprilS->intervalsFULL, aprilS->numIntervalsFULL);
                if (FF == 1) {
                    // if full lists match, then the polygons might be EQUAL
                    // refine directly for equal
                    spatial_lib::g_queryOutput.refinementCandidates+=1;
                    if (spatial_lib::isEqual(idR, idS)){
                        return spatial_lib::TR_EQUAL;
                    }
                }
            } else if (!aprilR->numIntervalsFULL && !aprilS->numIntervalsFULL) {
                // if either has full, also possibly EQUAL
                // refine directly
                spatial_lib::g_queryOutput.refinementCandidates+=1;
                if (spatial_lib::isEqual(idR, idS)){
                    return spatial_lib::TR_EQUAL;
                }
            }
        }
        
        if (AAresult == spatial_lib::IL_R_INSIDE_S || AAresult == spatial_lib::IL_MATCH) {
            // R_ALL contained in S_ALL guaranteed
            if(aprilS->numIntervalsFULL) {
                int AFresult = joinIntervalsHybrid(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsFULL, aprilS->numIntervalsFULL);
                // printf("AF: %d\n", AFresult);
                if (AFresult == spatial_lib::IL_R_INSIDE_S) {
                    // true hit inside
                    return spatial_lib::TR_INSIDE;
                }

                if (AFresult == spatial_lib::IL_INTERSECT) {
                    // intersection between ALL-FULL
                    // no true hit for containment doesnt mean NO containment
                    // refine
                    return spatial_lib::REFINE_CONTAINMENT_ONLY;
                } else {
                    // AA containment + AF no intersection
                    // refine specific relations
                    return spatial_lib::REFINE_INSIDE_PLUS;
                }
            }
        }
        
        if(AAresult == spatial_lib::IL_S_INSIDE_R || AAresult == spatial_lib::IL_MATCH) {
            // S_ALL contained in R_ALL guaranteed
            if (aprilR->numIntervalsFULL) {
                int FAresult = joinIntervalsHybrid(aprilS->intervalsALL, aprilS->numIntervalsALL, aprilR->intervalsFULL, aprilR->numIntervalsFULL);
                // printf("FA: %d\n", FAresult);
                // in this case R is S because joinIntervalsHybrid handles the first list as R and the second as S
                // and only checks for assymetrical containment R in S
                if (FAresult == spatial_lib::IL_R_INSIDE_S) {
                    // true hit contains
                    return spatial_lib::TR_CONTAINS;
                } 

                if (FAresult == spatial_lib::IL_INTERSECT) {
                    // intersection between FULL-ALL
                    // no true hit for containment doesnt mean NO containment
                    // refine
                    return spatial_lib::REFINE_CONTAINMENT_ONLY;
                } else {
                    // AA containment + FA no intersection
                    // need refinement for specific containment
                    return spatial_lib::REFINE_CONTAIN_PLUS;
                }
            }
        } 

        int AFresult = intersectionJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsFULL, aprilS->numIntervalsFULL);
        if (AFresult) {
            // true hit intersect, but candidate for some relations
            return spatial_lib::TR_INTERSECT;
        }
        
        int FAresult = intersectionJoinIntervalLists(aprilR->intervalsFULL, aprilR->numIntervalsFULL, aprilS->intervalsALL, aprilS->numIntervalsALL);
        if (FAresult) {
            // true hit intersect, but candidate for some relations
            return spatial_lib::TR_INTERSECT;
        }

        // refine for all relationships
        return spatial_lib::REFINE_ALL_NO_EQUAL;
    }


    int intersectionAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS){
        //check ALL - ALL
        if(!intersectionJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsALL, aprilS->numIntervalsALL)){
            //guaranteed not hit
            return spatial_lib::TRUE_NEGATIVE;
        }
        //check ALL - FULL
        if(aprilS->numIntervalsFULL > 0){
            if(intersectionJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsFULL, aprilS->numIntervalsFULL)){
                //hit
                return spatial_lib::TRUE_HIT;
            }
        }
        //check FULL - ALL
        if(aprilR->numIntervalsFULL){
            if(intersectionJoinIntervalLists(aprilR->intervalsFULL, aprilR->numIntervalsFULL, aprilS->intervalsALL, aprilS->numIntervalsALL)){
                //hit
                return spatial_lib::TRUE_HIT;
            }
        }
        //send to refinement
        return spatial_lib::INCONCLUSIVE;
    }

    int disjointAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS){
        //check ALL - ALL
        if(!intersectionJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsALL, aprilS->numIntervalsALL)){
            //guaranteed not hit
            return spatial_lib::TRUE_HIT;
        }
        //check ALL - FULL
        if(aprilS->numIntervalsFULL > 0){
            if(intersectionJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsFULL, aprilS->numIntervalsFULL)){
                //hit
                return spatial_lib::TRUE_NEGATIVE;
            }
        }
        //check FULL - ALL
        if(aprilR->numIntervalsFULL){
            if(intersectionJoinIntervalLists(aprilR->intervalsFULL, aprilR->numIntervalsFULL, aprilS->intervalsALL, aprilS->numIntervalsALL)){
                //hit
                return spatial_lib::TRUE_NEGATIVE;
            }
        }
        //send to refinement
        return spatial_lib::INCONCLUSIVE;
    }

    int insideAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS){
        // compare AA for containment
        if(!insideJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsALL, aprilS->numIntervalsALL)){
            // true negative
            return spatial_lib::TRUE_NEGATIVE;
        }

        // compare AF for containment
        if(aprilS->numIntervalsFULL){
            if(insideJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsFULL, aprilS->numIntervalsFULL)){
                // true hit
                return spatial_lib::TRUE_HIT;
            }
        }
        //send to refinement
        return spatial_lib::INCONCLUSIVE; 
    }

    int equalAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS){
        // compare AA for match
        if(!joinIntervalsForMatch(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsALL, aprilS->numIntervalsALL)) {
            return spatial_lib::TRUE_NEGATIVE;
        }
        if(aprilR->numIntervalsFULL && aprilS->numIntervalsFULL) {
            // if their FULL intervals dont match, its TRUE NEGATIVE for equal
            if (!joinIntervalsForMatch(aprilR->intervalsFULL, aprilR->numIntervalsFULL, aprilS->intervalsFULL, aprilS->numIntervalsFULL)) {
                return spatial_lib::TRUE_NEGATIVE;
            }
        } 
        // equal candidate, refinement
        return spatial_lib::INCONCLUSIVE;
    }

    int meetAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS){
        // compare AA for intersection
        if(!intersectionJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsALL, aprilS->numIntervalsALL)){
            //guaranteed not hit
            return spatial_lib::TRUE_NEGATIVE;
        }
        // compare AF for intersection
        if(aprilS->numIntervalsFULL > 0){
            if(intersectionJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsFULL, aprilS->numIntervalsFULL)){
                // intersection between ALL and FULL (interior) is guaranteed to be NOT MEET
                return spatial_lib::TRUE_NEGATIVE;
            }
        }
        // compare FA for intersection
        if(aprilR->numIntervalsFULL){
            if(intersectionJoinIntervalLists(aprilR->intervalsFULL, aprilR->numIntervalsFULL, aprilS->intervalsALL, aprilS->numIntervalsALL)){
                // intersection between FULL (interior) and ALL is guaranteed to be NOT MEET
                return spatial_lib::TRUE_NEGATIVE;
            }
        }
        //send to refinement
        return spatial_lib::INCONCLUSIVE;
    }

    int containsAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS){
        // compare AA for containment
        if(!insideJoinIntervalLists(aprilS->intervalsALL, aprilS->numIntervalsALL, aprilR->intervalsALL, aprilR->numIntervalsALL)) {
            return spatial_lib::TRUE_NEGATIVE;
        }
        // compare FA for containment
        if(aprilR->numIntervalsFULL) {
            if(insideJoinIntervalLists(aprilS->intervalsALL, aprilS->numIntervalsALL, aprilR->intervalsFULL, aprilR->numIntervalsFULL)) {
                return spatial_lib::TRUE_HIT;
            }
        }
        // inconclusive
        return spatial_lib::INCONCLUSIVE;
    }

    int coversAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS){
        // compare AA for containment
        if(!insideJoinIntervalLists(aprilS->intervalsALL, aprilS->numIntervalsALL, aprilR->intervalsALL, aprilR->numIntervalsALL)) {
            return spatial_lib::TRUE_NEGATIVE;
        }
        // compare FA for containment
        if(aprilR->numIntervalsFULL) {
            if(insideJoinIntervalLists(aprilS->intervalsALL, aprilS->numIntervalsALL, aprilR->intervalsFULL, aprilR->numIntervalsFULL)) {
                return spatial_lib::TRUE_HIT;
            }
        }
        // inconclusive
        return spatial_lib::INCONCLUSIVE;
    }
    
    int coveredByAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS){
        // compare AA for containment
        if(!insideJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsALL, aprilS->numIntervalsALL)){
            //guaranteed not hit
            return spatial_lib::TRUE_NEGATIVE;
        }

        // compare AF for containment
        if(aprilS->numIntervalsFULL){
            if(insideJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsFULL, aprilS->numIntervalsFULL)){
                //hit
                return spatial_lib::TRUE_HIT;
            }
        }
        //send to refinement
        return spatial_lib::INCONCLUSIVE;
    }

}