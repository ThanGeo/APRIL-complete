#include "controller.h"

namespace APRIL
{
    /**
     * GLOBALS
    */
    spatial_lib::QueryT* g_query;

    /************************************************/
    static void APRILIntersectionFilter(uint idR, uint idS) {
        // get common sections
        std::vector<uint> commonSections = spatial_lib::getCommonSectionIDsOfObjects(g_query->R, g_query->S, idR, idS);
        // for each common section
        for (auto &sectionID : commonSections) {
            // fetch the APRIL of R and S for this section
            spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->R, sectionID, idR);
            spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->S, sectionID, idS);

            // use APRIL intermediate filter
            int iFilterResult = intersectionAPRILUncompressed(aprilR, aprilS);
            if (iFilterResult != spatial_lib::INCONCLUSIVE) {
                // true negative or true hit, return
                // measure time
                spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
                // count result
                spatial_lib::countAPRILResult(iFilterResult);
                return;
            }
        }
        // i filter ended, measure time
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
        
        // count refinement candidate
        spatial_lib::countAPRILResult(spatial_lib::INCONCLUSIVE);
        // refinement
        spatial_lib::time::markRefinementFilterTimer();
        spatial_lib::refineIntersectionJoin(idR, idS);
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    static void APRILInsideFilter(uint idR, uint idS) {
        // get common sections
        std::vector<uint> commonSections = spatial_lib::getCommonSectionIDsOfObjects(g_query->R, g_query->S, idR, idS);
        // for each common section
        for (auto &sectionID : commonSections) {
            // fetch the APRIL of R and S for this section
            spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->R, sectionID, idR);
            spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->S, sectionID, idS);

            // use APRIL intermediate filter
            int iFilterResult = insideAPRILUncompressed(aprilR, aprilS);
            if (iFilterResult != spatial_lib::INCONCLUSIVE) {
                // true negative or true hit, return
                // measure time
                spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
                // count result
                spatial_lib::countAPRILResult(iFilterResult);
                return;
            }
        }
        // i filter ended, measure time
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
        
        // count refinement candidate
        spatial_lib::countAPRILResult(spatial_lib::INCONCLUSIVE);

        // refinement
        spatial_lib::time::markRefinementFilterTimer();
        spatial_lib::refineInsideJoin(idR, idS);
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    static void APRILDisjointFilter(uint idR, uint idS) {
        // get common sections
        std::vector<uint> commonSections = spatial_lib::getCommonSectionIDsOfObjects(g_query->R, g_query->S, idR, idS);
        // for each common section
        for (auto &sectionID : commonSections) {
            // fetch the APRIL of R and S for this section
            spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->R, sectionID, idR);
            spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->S, sectionID, idS);

            // use APRIL intermediate filter
            int iFilterResult = disjointAPRILUncompressed(aprilR, aprilS);
            if (iFilterResult != spatial_lib::INCONCLUSIVE) {
                // true negative or true hit, return
                // measure time
                spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
                // count result
                spatial_lib::countAPRILResult(iFilterResult);
                return;
            }
        }
        // i filter ended, measure time
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
    
        // count result
        spatial_lib::countAPRILResult(spatial_lib::INCONCLUSIVE);

        // refinement
        spatial_lib::time::markRefinementFilterTimer();
        spatial_lib::refineDisjointJoin(idR, idS);
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    static void APRILEqualFilter(uint idR, uint idS) {
        // get common sections
        std::vector<uint> commonSections = spatial_lib::getCommonSectionIDsOfObjects(g_query->R, g_query->S, idR, idS);
        // for each common section
        for (auto &sectionID : commonSections) {
            // fetch the APRIL of R and S for this section
            spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->R, sectionID, idR);
            spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->S, sectionID, idS);

            // use APRIL intermediate filter
            int iFilterResult = equalAPRILUncompressed(aprilR, aprilS);
            if (iFilterResult != spatial_lib::INCONCLUSIVE) {
                // true negative or true hit, return
                // measure time
                spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
                // count result
                spatial_lib::countAPRILResult(iFilterResult);
                return;
            }
        }
        // i filter ended, measure time
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
    
        // count result
        spatial_lib::countAPRILResult(spatial_lib::INCONCLUSIVE);

        // refinement
        spatial_lib::time::markRefinementFilterTimer();
        spatial_lib::refineEqualJoin(idR, idS);
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    static void APRILMeetFilter(uint idR, uint idS) {
        // get common sections
        std::vector<uint> commonSections = spatial_lib::getCommonSectionIDsOfObjects(g_query->R, g_query->S, idR, idS);
        // for each common section
        for (auto &sectionID : commonSections) {
            // fetch the APRIL of R and S for this section
            spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->R, sectionID, idR);
            spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->S, sectionID, idS);

            // use APRIL intermediate filter
            int iFilterResult = meetAPRILUncompressed(aprilR, aprilS);
            if (iFilterResult != spatial_lib::INCONCLUSIVE) {
                // true negative or true hit, return
                // measure time
                spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
                // count result
                spatial_lib::countAPRILResult(iFilterResult);
                return;
            }
        }
        // i filter ended, measure time
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
    
        // count result
        spatial_lib::countAPRILResult(spatial_lib::INCONCLUSIVE);
        
        // refinement
        spatial_lib::time::markRefinementFilterTimer();
        spatial_lib::refineMeetJoin(idR, idS);
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    static void APRILContainsFilter(uint idR, uint idS) {
        // get common sections
        std::vector<uint> commonSections = spatial_lib::getCommonSectionIDsOfObjects(g_query->R, g_query->S, idR, idS);
        // for each common section
        for (auto &sectionID : commonSections) {
            // fetch the APRIL of R and S for this section
            spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->R, sectionID, idR);
            spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->S, sectionID, idS);

            // use APRIL intermediate filter
            int iFilterResult = containsAPRILUncompressed(aprilR, aprilS);
            if (iFilterResult != spatial_lib::INCONCLUSIVE) {
                // true negative or true hit, return
                // measure time
                spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
                // count result
                spatial_lib::countAPRILResult(iFilterResult);
                return;
            }
        }
        // i filter ended, measure time
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
    
        // count result
        spatial_lib::countAPRILResult(spatial_lib::INCONCLUSIVE);

        // refinement
        spatial_lib::time::markRefinementFilterTimer();
        spatial_lib::refineContainsJoin(idR, idS);
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    static void APRILCoversFilter(uint idR, uint idS) {
        // get common sections
        std::vector<uint> commonSections = spatial_lib::getCommonSectionIDsOfObjects(g_query->R, g_query->S, idR, idS);
        // for each common section
        for (auto &sectionID : commonSections) {
            // fetch the APRIL of R and S for this section
            spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->R, sectionID, idR);
            spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->S, sectionID, idS);

            // use APRIL intermediate filter
            int iFilterResult = coversAPRILUncompressed(aprilR, aprilS);
            if (iFilterResult != spatial_lib::INCONCLUSIVE) {
                // true negative or true hit, return
                // measure time
                spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
                // count result
                spatial_lib::countAPRILResult(iFilterResult);
                return;
            }
        }
        // i filter ended, measure time
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
    
        // count result
        spatial_lib::countAPRILResult(spatial_lib::INCONCLUSIVE);
        
        // refinement
        spatial_lib::time::markRefinementFilterTimer();
        spatial_lib::refineCoversJoin(idR, idS);
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    static void APRILCoveredByFilter(uint idR, uint idS) {
        // get common sections
        std::vector<uint> commonSections = spatial_lib::getCommonSectionIDsOfObjects(g_query->R, g_query->S, idR, idS);
        // for each common section
        for (auto &sectionID : commonSections) {
            // fetch the APRIL of R and S for this section
            spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->R, sectionID, idR);
            spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->S, sectionID, idS);

            // use APRIL intermediate filter
            int iFilterResult = coveredByAPRILUncompressed(aprilR, aprilS);
            if (iFilterResult != spatial_lib::INCONCLUSIVE) {
                // true negative or true hit, return
                // measure time
                spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
                // count result
                spatial_lib::countAPRILResult(iFilterResult);
                return;
            }
        }
        // i filter ended, measure time
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
    
        // count result
        spatial_lib::countAPRILResult(spatial_lib::INCONCLUSIVE);

        // refinement
        spatial_lib::time::markRefinementFilterTimer();
        spatial_lib::refineCoveredByJoin(idR, idS);
        spatial_lib::g_queryOutput.refinementTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.refTimer);
    }

    void APRILFindTopology(uint idR, uint idS) {
        // get common sections
        std::vector<uint> commonSections = spatial_lib::getCommonSectionIDsOfObjects(g_query->R, g_query->S, idR, idS);
        // for each common section
        int iFilterResult;
        for (auto &sectionID : commonSections) {
            // fetch the APRIL of R and S for this section
            spatial_lib::AprilDataT* aprilR = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->R, sectionID, idR);
            spatial_lib::AprilDataT* aprilS = spatial_lib::getAprilDataBySectionAndObjectIDs(g_query->S, sectionID, idS);

            // use APRIL intermediate filter
            iFilterResult = findRelationAPRILUncompressed(idR, idS, aprilR, aprilS);
            // switch based on result
            switch(iFilterResult) {
                // true hits, count and return
                case spatial_lib::TR_INSIDE:
                case spatial_lib::TR_CONTAINS:
                case spatial_lib::TR_EQUAL:
                case spatial_lib::TR_DISJOINT:
                case spatial_lib::TR_INTERSECT:
                    // result
                    spatial_lib::countTopologyRelationResult(iFilterResult);
                    // time
                    spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
                    return;
            }
        }
        // i filter ended, measure time
        spatial_lib::g_queryOutput.iFilterTime += spatial_lib::time::getElapsedTime(spatial_lib::time::g_timer.iFilterTimer);
    
        // count refinement candidate
        spatial_lib::countAPRILResult(spatial_lib::INCONCLUSIVE);

        int relation;
        // switch based on result
        switch(iFilterResult) {            
            // inconclusive, do selective refinement
            // result holds what type of refinement needs to be made
            case spatial_lib::REFINE_ALL_NO_EQUAL:
                // time 
                spatial_lib::time::markRefinementFilterTimer();
                // refine
                relation = spatial_lib::refineAllRelationsNoEqual(idR, idS);
                break;
            
            case spatial_lib::REFINE_CONTAINS_PLUS:
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
        // use appropriate query function
        switch (g_query->type) {
            case spatial_lib::Q_INTERSECT:
                APRILIntersectionFilter(idR, idS);
                break;
            case spatial_lib::Q_INSIDE:
                APRILInsideFilter(idR, idS);
                break;
            case spatial_lib::Q_DISJOINT:
                APRILDisjointFilter(idR, idS);
                break;
            case spatial_lib::Q_EQUAL:
                APRILEqualFilter(idR, idS);
                break;
            case spatial_lib::Q_MEET:
                APRILMeetFilter(idR, idS);
                break;
            case spatial_lib::Q_CONTAINS:
                APRILContainsFilter(idR, idS);
                break;
            case spatial_lib::Q_COVERS:
                APRILCoversFilter(idR, idS);
                break;
            case spatial_lib::Q_COVERED_BY:   
                APRILCoveredByFilter(idR, idS);
                break;
            case spatial_lib::Q_FIND_RELATION:
                APRILFindTopology(idR, idS);
                break;
            default:
                // not supported/unknown
                exit(-1);
                break;
        }
        

    }
}