#include "join.h"

namespace APRIL
{
    static int withinJoinIntervalLists(std::vector<uint> &ar1, uint &numintervals1, std::vector<uint> &ar2, uint &numintervals2){
        //they may not have any intervals of this type
        if(numintervals1 == 0 || numintervals2 == 0){
            return 0;
        }
        
        // ID st1,st2,end1,end2;
        uint cur1=0;
        uint cur2=0;
        
        auto st1 = ar1.begin();
        auto end1 = ar1.begin() + 1;
        cur1++;
        
        auto st2 = ar2.begin();
        auto end2 = ar2.begin() + 1;
        cur2++;

        bool intervalRcontained = false;

        do {
            //check if it is contained completely
            if(*st1 >= *st2 && *end1 <= *end2){
                intervalRcontained = true;
            }

            if (*end1<=*end2)
            {
                if(!intervalRcontained){
                    //we are skipping this interval because it was never contained, so return false (not within)
                    return 0;
                }
                st1 += 2;
                end1 += 2;
                cur1++;
                intervalRcontained = false;
            }
            else 
            {
                st2 += 2;
                end2 += 2;
                cur2++;
            }
        } while(cur1<=numintervals1 && cur2<=numintervals2);
                
        //if we didnt check all of the R intervals
        if(cur1 <= numintervals1){	
            return 0;
        }
        //all intervals R were contained
        return 1;
    }

    static int intersectionJoinIntervalLists(std::vector<uint> &ar1, uint &numintervals1, std::vector<uint> &ar2, uint &numintervals2){
        //they may not have any intervals of this type
        if(numintervals1 == 0 || numintervals2 == 0){
            return 0;
        }
        
        // st1,st2,end1,end2;
        uint cur1=0;
        uint cur2=0;
        
        auto st1 = ar1.begin();
        auto end1 = ar1.begin() + 1;
        cur1++;
        
        auto st2 = ar2.begin();
        auto end2 = ar2.begin() + 1;
        cur2++;

        do {
            if (*st1<=*st2)
            {
                if (*end1>*st2) // overlap, end1>=st2 if intervals are [s,e] and end1>st2 if intervals are [s,e)
                {
                    //they overlap, return 1
                    return 1;
                }	
                else
                {
                    st1 += 2;
                    end1 += 2;
                    cur1++;
                }
            }
            else // st2<st1
            {
                if (*end2>*st1) // overlap, end2>=st1 if intervals are [s,e] and end2>st1 if intervals are [s,e)
                {

                    //they overlap, return 1
                    return 1;
                }
                else
                {
                    st2 += 2;
                    end2 += 2;
                    cur2++;
                }
            }
        } while(cur1<=numintervals1 && cur2<=numintervals2);
        
        //no overlap, return 0
        return 0;
    }

    //join two uncompressed APRIL approximations for intersection
    int intersectionJoinAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS){
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

    //join two uncompressed APRIL approximations for within (R in S)
    int withinJoinAPRILUncompressed(spatial_lib::AprilDataT *aprilR, spatial_lib::AprilDataT *aprilS){
        //check ALL - ALL
        if(!intersectionJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsALL, aprilS->numIntervalsALL)){
            //guaranteed not hit
            return spatial_lib::TRUE_NEGATIVE;
        }

        //check ALL - F (if all intervals in R_ALL are inside any intervals of S_FULL)
        if(aprilS->numIntervalsFULL){
            if(withinJoinIntervalLists(aprilR->intervalsALL, aprilR->numIntervalsALL, aprilS->intervalsFULL, aprilS->numIntervalsFULL)){
                //hit
                return spatial_lib::TRUE_HIT;
            }
        }
        //send to refinement
        return spatial_lib::INCONCLUSIVE; 
    }
}