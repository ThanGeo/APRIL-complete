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
                    // refine directly
                    spatial_lib::g_queryOutput.refinementCandidates+=1;
                    if (spatial_lib::refineEqual(idR, idS)){
                        return spatial_lib::TR_EQUAL;
                    }
                }
            } else if (!aprilR->numIntervalsFULL && !aprilS->numIntervalsFULL) {
                // if either has full, also possibly EQUAL
                // refine directly
                spatial_lib::g_queryOutput.refinementCandidates+=1;
                if (spatial_lib::refineEqual(idR, idS)){
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
                    // return spatial_lib::refineContainmentsOnly(idR, idS);
                    return spatial_lib::REFINE_CONTAINMENT_ONLY;
                } else {
                    // AA containment + AF no intersection
                    // refine specific relations
                    // return spatial_lib::refineInsidePlus(idR, idS);
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
                    // return spatial_lib::refineContainmentsOnly(idR, idS);
                    return spatial_lib::REFINE_CONTAINMENT_ONLY;
                } else {
                    // AA containment + FA no intersection
                    // need refinement for specific containment
                    // return spatial_lib::refineContainsPlus(idR, idS);
                    return spatial_lib::REFINE_CONTAIN_PLUS;
                }
            }
        } 

        if (AAresult == spatial_lib::IL_INTERSECT) {
            // NO containment in AA, but there is an intersection
            if (aprilS->numIntervalsFULL) {
                int AFresult = intersectionJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsFULL, aprilS->numIntervalsFULL);
                // printf("AF: %d\n", AFresult);
                if (AFresult) {
                    // TODO::::
                    // true hit intersect, but candidate for some relations
                    return spatial_lib::TR_INTERSECTS;
                }
            }
            if (aprilR->numIntervalsFULL) {
                int FAresult = intersectionJoinIntervalLists(aprilR->intervalsFULL, aprilR->numIntervalsFULL, aprilS->intervalsALL, aprilS->numIntervalsALL);
                // printf("FA: %d\n", FAresult);
                if (FAresult) {
                    // TODO::::
                    // true hit intersect, but candidate for some relations
                    return spatial_lib::TR_INTERSECTS;
                }
            }
            // no containment
            // return spatial_lib::refineGuaranteedNoContainment(idR, idS);
            return spatial_lib::REFINE_NO_CONTAINMENT;
        }

        // refine for all relationships
        // return spatial_lib::refineAllRelationsNoEqual(idR, idS);
        return spatial_lib::REFINE_ALL_NO_EQUAL;
    }
}