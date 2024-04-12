#include "find_relation_filter.h"

namespace two_layer
{
    static double d_xmin;
    static double d_ymin;
    static double d_xmax;
    static double d_ymax;

    static inline int myQuotient(int numer, int denom) {
        return numer/denom;
    };


    static inline int myRemainder(int numer, int denom) {
        return numer%denom;
    };

    static inline void forwardPair(uint idR, uint idS, spatial_lib::MBRRelationCaseE relationCase) {
        
        switch (g_iFilterType) {
            case spatial_lib::IF_APRIL:
                if (relationCase != spatial_lib::MBR_CROSS) {
                    // forward to intermediate filter
                    APRIL::IntermediateFilterFindRelationEntrypoint(idR, idS, relationCase);
                } else {
                    // they cross, true hit intersect (skip intermediate filter)
                    spatial_lib::countTopologyRelationResult(spatial_lib::TR_INTERSECT);
                    // printf("%u,%u\n", idR, idS);
                }
                break;
            case spatial_lib::IF_NONE:
                // selective refinement
                if (relationCase != spatial_lib::MBR_CROSS) {
                    spatial_lib::refinementEntrypoint(idR, idS);
                } else {
                    // they cross, true hit intersect (skip intermediate filter)
                    spatial_lib::countTopologyRelationResult(spatial_lib::TR_INTERSECT);
                }


                break;
            default:
                break;
        }
    }

    static inline void relateMBRs(RelationIterator &rIterator, RelationIterator &sIterator) {
        // compute deltas
        d_xmin = rIterator->xStart - sIterator->xStart;
        d_ymin = rIterator->yStart - sIterator->yStart;
        d_xmax = rIterator->xEnd - sIterator->xEnd;
        d_ymax = rIterator->yEnd - sIterator->yEnd;

        
        // if(rIterator->id == 21101 && sIterator->id == 257975) {
        //     printf("Deltas: %f,%f,%f,%f\n", d_xmin, d_ymin, d_xmax, d_ymax);
        // }

        // check for equality using an error margin because doubles
        if (abs(d_xmin) < EPS) {
            if (abs(d_xmax) < EPS) {
                if (abs(d_ymin) < EPS) {
                    if (abs(d_ymax) < EPS) {
                        // equal MBRs
                        forwardPair(rIterator->id, sIterator->id, spatial_lib::MBR_EQUAL);
                        return;
                    }
                }
            }
        }
        // not equal MBRs, check other relations
        if (d_xmin <= 0) {
            if (d_xmax >= 0) {
                if (d_ymin <= 0) {
                    if (d_ymax >= 0) {
                        // MBR(s) inside MBR(r)
                        forwardPair(rIterator->id, sIterator->id, spatial_lib::MBR_S_IN_R);
                        return;
                    }
                } else {
                    if (d_ymax < 0) {
                        // MBRs cross each other
                        forwardPair(rIterator->id, sIterator->id, spatial_lib::MBR_CROSS);
                        return;
                    }
                }
            }
        } 
        if (d_xmin >= 0) {
            if (d_xmax <= 0) {
                if (d_ymin >= 0) {
                    if (d_ymax <= 0) {
                        // MBR(r) inside MBR(s)
                        forwardPair(rIterator->id, sIterator->id, spatial_lib::MBR_R_IN_S);
                        return;
                    }
                } else {
                    if (d_ymax > 0) {
                        // MBRs cross each other
                        forwardPair(rIterator->id, sIterator->id, spatial_lib::MBR_CROSS);
                        return;
                    }
                }
            }
        }

        // MBRs intersect generally
        forwardPair(rIterator->id, sIterator->id, spatial_lib::MBR_INTERSECT);
    }
    
    static inline unsigned long long InternalLoop_Rolled_CNT_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/,int flag)
    {
        unsigned long long result = 0;
        auto pivot = firstFS;

        while ((pivot < lastFS) && (rec->yEnd >= pivot->yStart))
        {   
            // disjoint, skip
            if ((rec->xStart > pivot->xEnd) || (rec->xEnd < pivot->xStart))
            {
                pivot++;
                continue;
            }

            // relate the MBRs for their specific relationship
            if(flag == 0){
                // rec is R, pivot is S
                relateMBRs(rec, pivot);
            } else {
                // pivot is R, rec is S
                relateMBRs(pivot, rec);
            }

            result++;
            pivot++;
        }

        return result;
    };
    
    
    static inline unsigned long long InternalLoop_Rolled_CNT_V2_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/, int flag)
    {
        unsigned long long result = 0;
        auto pivot = firstFS;

        while ((pivot < lastFS) && (rec->yEnd >= pivot->yStart))
        {
            if ((rec->xStart > pivot->xEnd) || (rec->xEnd < pivot->xStart))
            {
                pivot++;
                continue;
            }
            // relate the MBRs for their specific relationship
            if(flag == 0){
                // rec is R, pivot is S
                relateMBRs(rec, pivot);
            } else {
                // pivot is R, rec is S
                relateMBRs(pivot, rec);
            }
            result++;
            pivot++;
        }

        return result;
    };
    
    
    static inline unsigned long long InternalLoop_Rolled_CNT_V3_1_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/, int flag)
    {
        unsigned long long result = 0;
        auto pivot = firstFS;

        while ((pivot < lastFS) && (rec->yEnd >= pivot->yStart))
        {        
            if (rec->xStart > pivot->xEnd)
            {
                pivot++;
                continue;
            }

            // relate the MBRs for their specific relationship
            if(flag == 0){
                // rec is R, pivot is S
                relateMBRs(rec, pivot);
            } else {
                // pivot is R, rec is S
                relateMBRs(pivot, rec);
            }
            result++;
            pivot++;
        }

        return result;
    };
    

    static inline unsigned long long InternalLoop_Rolled_CNT_V3_2_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/, int flag)
    {
        unsigned long long result = 0;
        auto pivot = firstFS;

        while ((pivot < lastFS) && (rec->yEnd >= pivot->yStart))
        {       
            if (pivot->xStart > rec->xEnd)
            {
                pivot++;
                continue;
            }
            // relate the MBRs for their specific relationship
            if(flag == 0){
                // rec is R, pivot is S
                relateMBRs(rec, pivot);
            } else {
                // pivot is R, rec is S
                relateMBRs(pivot, rec);
            }
            result++;
            pivot++;
        }

        return result;
    };
    
    
    static inline unsigned long long InternalLoop_Rolled_CNT_V4_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/, int flag)
    {
        unsigned long long result = 0;
        auto pivot = firstFS;

        while ((pivot < lastFS) && (rec->yEnd >= pivot->yStart))
        {
            if (rec->xStart > pivot->xEnd)
            {
                pivot++;
                continue;
            }
            // relate the MBRs for their specific relationship
            if(flag == 0){
                // rec is R, pivot is S
                relateMBRs(rec, pivot);
            } else {
                // pivot is R, rec is S
                relateMBRs(pivot, rec);
            }
            result++;
            pivot++;
        }

        return result;
    };
    
    
    static inline unsigned long long InternalLoop_Rolled_CNT_V5_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/, int flag)
    {
        unsigned long long result = 0;
        auto pivot = firstFS;

        while ((pivot < lastFS) && (rec->yEnd >= pivot->yStart))
        {
            if ( rec->xEnd < pivot->xStart)
            {
                pivot++;
                continue;
            }
            // relate the MBRs for their specific relationship
            if(flag == 0){
                // rec is R, pivot is S
                relateMBRs(rec, pivot);
            } else {
                // pivot is R, rec is S
                relateMBRs(pivot, rec);
            }
            result++;
            pivot++;
        }

        return result;
    };
    
                    
    static inline unsigned long long Sweep_Rolled_CNT_Y_(Relation &R, Relation &S, size_t startR, size_t endR, size_t startS, size_t endS)
    {
        unsigned long long result = 0;
        RelationIterator r = R.begin() + startR;
        RelationIterator s = S.begin() + startS;
        RelationIterator lastR = R.begin() + endR;
        RelationIterator lastS = S.begin() + endS;

        while ((r < lastR) && (s < lastS))
        {
            if (r->yStart < s->yStart)
            {
                // Run internal loop.
                //cout << "Sweep_Rolled_CNT_Y_ 0" << endl;
                result += InternalLoop_Rolled_CNT_Y_(r, s, lastS, 0 );
                r++;
            }
            else
            {
                // Run internal loop.
                //cout << "Sweep_Rolled_CNT_Y_ 1" << endl;
                result += InternalLoop_Rolled_CNT_Y_(s, r, lastR, 1 );
                s++;
            }
        }

        return result;
    };


    static inline unsigned long long Sweep_Rolled_CNT_V2_Y_(Relation &R, Relation &S, size_t startR, size_t endR, size_t startS, size_t endS, int flag)
    {
        unsigned long long result = 0;
        RelationIterator r = R.begin() + startR;
        RelationIterator s = S.begin() + startS;
        RelationIterator lastR = R.begin() + endR;
        RelationIterator lastS = S.begin() + endS;

        while ((r < lastR))
        {
            //cout << "Sweep_Rolled_CNT_V2_Y_ " << flag << endl;
            result += InternalLoop_Rolled_CNT_V2_Y_(r, s, lastS, flag );
            r++;
        }

        return result;
    };


    static inline unsigned long long Sweep_Rolled_CNT_V3_Y_(Relation &R, Relation &S, size_t startR, size_t endR, size_t startS, size_t endS, int flag)
    {
        unsigned long long result = 0;
        RelationIterator r = R.begin() + startR;
        RelationIterator s = S.begin() + startS;
        RelationIterator lastR = R.begin() + endR;
        RelationIterator lastS = S.begin() + endS;

        while ((r < lastR) && (s < lastS))
        {
            if (r->yStart < s->yStart)
            {
                // Run internal loop.
                //cout << "Sweep_Rolled_CNT_V3_Y_ " << flag << endl;
                result += InternalLoop_Rolled_CNT_V3_1_Y_(r, s, lastS, flag^1 );
                r++;
            }
            else
            {
                // Run internal loop.
                //cout << "Sweep_Rolled_CNT_V3_Y_ " << flag << endl;
                result += InternalLoop_Rolled_CNT_V3_2_Y_(s, r, lastR, flag );
                s++;
            }
        }

        return result;
    };


    static inline unsigned long long Sweep_Rolled_CNT_V4_Y_(Relation &R, Relation &S,size_t startR, size_t endR, size_t startS, size_t endS, int flag)
    {
        unsigned long long result = 0;
        RelationIterator r = R.begin() + startR;
        RelationIterator s = S.begin() + startS;
        RelationIterator lastR = R.begin() + endR;
        RelationIterator lastS = S.begin() + endS;

        while ((r < lastR))
        { 
            // Run internal loop.
            //cout << "Sweep_Rolled_CNT_V4_Y_ " << flag << endl;
            result += InternalLoop_Rolled_CNT_V4_Y_(r, s, lastS, flag );
            r++;
        }

        return result;
    };


    static inline unsigned long long Sweep_Rolled_CNT_V5_Y_(Relation &R, Relation &S, size_t startR, size_t endR, size_t startS, size_t endS, int flag)
    {
        unsigned long long result = 0;
        RelationIterator r = R.begin() + startR;
        RelationIterator s = S.begin() + startS;
        RelationIterator lastR = R.begin() + endR;
        RelationIterator lastS = S.begin() + endS;

        while ((r < lastR))
        {
            // Run internal loop.
            //cout << "Sweep_Rolled_CNT_V5_Y_ " << flag << endl;
            result += InternalLoop_Rolled_CNT_V5_Y_(r, s, lastS, flag);
            r++;
        }

        return result;
    };

    unsigned long long FindRelationMBRFilter(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition)
    {
        unsigned long long result = 0;

        for (int pid = 0; pid < runNumPartition; pid++)
        {
            if ( (pRB_size[pid] > 0) && (pSB_size[pid] > 0)){
                result += Sweep_Rolled_CNT_Y_(pR[pid], pS[pid], 0 , pRB_size[pid], 0, pSB_size[pid]);
            }

            if ( (pRB_size[pid] > 0) && (pSC_size[pid] > pSB_size[pid])){
                result +=Sweep_Rolled_CNT_V2_Y_( pS[pid], pR[pid], pSB_size[pid] , pSC_size[pid], 0, pRB_size[pid], 1);
            }

            if ( (pRB_size[pid] > 0) && (pSD_size[pid] > pSC_size[pid])){
                result += Sweep_Rolled_CNT_V3_Y_( pR[pid], pS[pid], 0, pRB_size[pid], pSC_size[pid], pSD_size[pid], 1);
            }

            if ( (pRB_size[pid] > 0) && (pS[pid].size() > pSD_size[pid])){
                result += Sweep_Rolled_CNT_V5_Y_(pS[pid], pR[pid], pSD_size[pid], pS[pid].size(), 0, pRB_size[pid], 1);
            }

            if ( (pRC_size[pid] > pRB_size[pid]) && (pSB_size[pid] > 0)){
                result += Sweep_Rolled_CNT_V2_Y_( pR[pid], pS[pid], pRB_size[pid], pRC_size[pid], 0, pSB_size[pid], 0);
            }
    
            if ( (pRC_size[pid] > pRB_size[pid]) && (pSD_size[pid] > pSC_size[pid])){
                result += Sweep_Rolled_CNT_V4_Y_( pR[pid],pS[pid], pRB_size[pid], pRC_size[pid], pSC_size[pid], pSD_size[pid], 0);
            }
        
            if ( (pRD_size[pid] > pRC_size[pid]) && (pSB_size[pid] > 0)){
                result += Sweep_Rolled_CNT_V3_Y_( pS[pid],pR[pid], 0, pSB_size[pid], pRC_size[pid], pRD_size[pid], 0);
            }
        
            if ( (pRD_size[pid] > pRC_size[pid]) && (pSC_size[pid] > pSB_size[pid])){
                result += Sweep_Rolled_CNT_V4_Y_( pS[pid], pR[pid], pSB_size[pid], pSC_size[pid], pRC_size[pid], pRD_size[pid], 1);
            }
        
            if ( (pR[pid].size() > pRD_size[pid]) && (pSB_size[pid] > 0)){
                result += Sweep_Rolled_CNT_V5_Y_(pR[pid], pS[pid], pRD_size[pid], pR[pid].size(), 0, pSB_size[pid], 0);
            }
        }

        return result;
    };
   
}