#ifndef TWO_LAYER_FILTER_H
#define TWO_LAYER_FILTER_H

#include "relation.h"

extern void (*g_pairIdForwardingFunctionPtr)(uint idR, uint idS);
extern void (*g_pairIdForwardingVoidFunctionPtr)(int count, ...);

namespace two_layer{

    extern vector<pair<uint,uint>> mbr_filter_output_pairs;       

    namespace sort{
        void SortXStartOneArray(Relation &pR, Relation &pS, size_t pRA_size , size_t pSA_size , size_t pRB_size, size_t pSB_size);
        void SortYStartOneArray(Relation &pR, Relation &pS, size_t pRA_size , size_t &pSA_size , size_t pRB_size, size_t pSB_size,  size_t pRC_size, size_t pSC_size);
    };  
    namespace sweepX{
        namespace oneArray{
            inline unsigned long long ForwardScanBased_PlaneSweep_CNT_X(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition);
            inline unsigned long long ForwardScanBased_PlaneSweep_CNT_X(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition, double* tileTime);
            inline unsigned long long ForwardScanBased_PlaneSweep_CNT_X_Less(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition);
        }
    }
    
    
    namespace sweepY{
        namespace oneArray{
            inline unsigned long long ForwardScanBased_PlaneSweep_CNT_Y(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition);
        

            inline unsigned long long ForwardScanBased_PlaneSweep_CNT_Y(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition, double* tileTime);
            inline unsigned long long ForwardScanBased_PlaneSweep_CNT_Y2(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition);

            inline unsigned long long ForwardScanBased_PlaneSweep_CNT_Y_Less(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition);

            inline unsigned long long  ForwardScanBased_PlaneSweep_CNT_Y_Less_Dec(vector<ABrec>* pRABdec, vector<ABrec>* pSABdec,  vector<Crec> *pRCdec, vector<Crec> *pSCdec, vector<Drec> *pRDdec, vector<Drec> *pSDdec, vector<Coord>* pRYEND, vector<Coord>* pSYEND, size_t * pRB_size, size_t * pSB_size, size_t * pRC_size, size_t * pSC_size, int runNumPartition);
        }
    }
    
    unsigned long long ForwardScanBased_PlaneSweep_CNT(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, bool runPlaneSweepOnX, int runNumPartitionsPerRelation);
    unsigned long long ForwardScanBased_PlaneSweep_CNT(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, bool runPlaneSweepOnX, int runNumPartitionsPerRelation, double* tileTime);
    unsigned long long ForwardScanBased_PlaneSweep_CNT_Dec(vector<ABrec>* pRABdec, vector<ABrec>* pSABdec,  vector<Crec> *pRCdec, vector<Crec> *pSCdec, vector<Drec> *pRDdec, vector<Drec> *pSDdec, vector<Coord>* pRYEND, vector<Coord>* pSYEND, size_t * pRB_size, size_t * pSB_size, size_t * pRC_size, size_t * pSC_size, bool runPlaneSweepOnX, int runNumPartitionsPerRelation);
    unsigned long long ForwardScanBased_PlaneSweep_CNT2(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, bool runPlaneSweepOnX, int runNumPartitionsPerRelation);
    unsigned long long ForwardScanBased_PlaneSweep_CNT_Less(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, bool runPlaneSweepOnX, int runNumPartitionsPerRelation);   
}

#endif
