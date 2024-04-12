#include "filter.h"

namespace two_layer
{
    
    static inline int myQuotient(int numer, int denom) {
        return numer/denom;
    };


    static inline int myRemainder(int numer, int denom) {
        return numer%denom;
    };

    inline void forwardPair(uint idR, uint idS) {
        switch (g_iFilterType) {
            case spatial_lib::IF_APRIL:
                APRIL::IntermediateFilterEntrypoint(idR, idS);
                break;
            case spatial_lib::IF_NONE:
                // straight to refinement
                spatial_lib::refinementEntrypoint(idR, idS);
                break;
            default:
                break;
        }
    }
    
    inline unsigned long long InternalLoop_Rolled_CNT_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/,int flag)
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
            // forward pair
            if(flag == 0){
                forwardPair(rec->id, pivot->id);
            }else{
                forwardPair(pivot->id, rec->id);
            }
            result++;
            pivot++;
        }

        return result;
    };
    
    
    inline unsigned long long InternalLoop_Rolled_CNT_V2_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/, int flag)
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
            // forward pair
            if(flag == 0){
                forwardPair(rec->id, pivot->id);
            }else{
                forwardPair(pivot->id, rec->id);
            }
            result++;
            pivot++;
        }

        return result;
    };
    
    
    inline unsigned long long InternalLoop_Rolled_CNT_V3_1_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/, int flag)
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

            // forward pair
            if(flag == 0){
                forwardPair(rec->id, pivot->id);
            }else{
                forwardPair(pivot->id, rec->id);
            }
            result++;
            pivot++;
        }

        return result;
    };
    

    inline unsigned long long InternalLoop_Rolled_CNT_V3_2_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/, int flag)
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
            // forward pair
            if(flag == 0){
                forwardPair(rec->id, pivot->id);
            }else{
                forwardPair(pivot->id, rec->id);
            }
            result++;
            pivot++;
        }

        return result;
    };
    
    
    inline unsigned long long InternalLoop_Rolled_CNT_V4_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/, int flag)
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
            // forward pair
            if(flag == 0){
                forwardPair(rec->id, pivot->id);
            }else{
                forwardPair(pivot->id, rec->id);
            }
            result++;
            pivot++;
        }

        return result;
    };
    
    
    inline unsigned long long InternalLoop_Rolled_CNT_V5_Y_(RelationIterator &rec, RelationIterator &firstFS, RelationIterator &lastFS/*, unsigned long long &count*/, int flag)
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
            // forward pair
            if(flag == 0){
                forwardPair(rec->id, pivot->id);
            }else{
                forwardPair(pivot->id, rec->id);
            }
            result++;
            pivot++;
        }

        return result;
    };
    
                    
    inline unsigned long long Sweep_Rolled_CNT_Y_(Relation &R, Relation &S, size_t startR, size_t endR, size_t startS, size_t endS)
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


    inline unsigned long long Sweep_Rolled_CNT_V2_Y_(Relation &R, Relation &S, size_t startR, size_t endR, size_t startS, size_t endS, int flag)
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


    inline unsigned long long Sweep_Rolled_CNT_V3_Y_(Relation &R, Relation &S, size_t startR, size_t endR, size_t startS, size_t endS, int flag)
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


    inline unsigned long long Sweep_Rolled_CNT_V4_Y_(Relation &R, Relation &S,size_t startR, size_t endR, size_t startS, size_t endS, int flag)
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


    inline unsigned long long Sweep_Rolled_CNT_V5_Y_(Relation &R, Relation &S, size_t startR, size_t endR, size_t startS, size_t endS, int flag)
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

    unsigned long long ForwardScanBased_PlaneSweep_CNT_Y_Less(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition)
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