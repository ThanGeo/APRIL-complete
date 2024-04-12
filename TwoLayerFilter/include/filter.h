#ifndef TWO_LAYER_FILTER_H
#define TWO_LAYER_FILTER_H

#include "relation.h"
#include "SpatialLib.h"
#include "APRIL.h"

namespace two_layer
{

    /**
     * @brief Performs the Two Layer standard spatial intersection join MBR evaluation filter
     * 
     * @param pR 
     * @param pS 
     * @param pRA_size 
     * @param pSA_size 
     * @param pRB_size 
     * @param pSB_size 
     * @param pRC_size 
     * @param pSC_size 
     * @param pRD_size 
     * @param pSD_size 
     * @param runNumPartition 
     * @return unsigned long long 
     */
    unsigned long long ForwardScanBased_PlaneSweep_CNT_Y_Less(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition);
    
    /**
     * @brief Performs a specialized two-layer MBR join filter that disqualifies topological relations from the objects' MBRs and forwards
     * to the appropriate intermediate filter receiving function.
     * 
     * @param pR 
     * @param pS 
     * @param pRA_size 
     * @param pSA_size 
     * @param pRB_size 
     * @param pSB_size 
     * @param pRC_size 
     * @param pSC_size 
     * @param pRD_size 
     * @param pSD_size 
     * @param runNumPartition 
     * @return unsigned long long 
     */
    unsigned long long FindRelationMBRFilter(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition);

}

#endif