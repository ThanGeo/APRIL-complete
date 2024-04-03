#ifndef TWO_LAYER_FILTER_H
#define TWO_LAYER_FILTER_H

#include "relation.h"
#include "SpatialLib.h"
#include "APRIL.h"

namespace two_layer
{

    namespace fs_2d{
        
        extern vector<pair<uint,uint>> mbr_filter_output_pairs;
        extern spatial_lib::IntermediateFilterTypeE g_iFilterType;
    
        namespace single{        

            namespace sweepY{
                
                namespace oneArray{

                    inline unsigned long long ForwardScanBased_PlaneSweep_CNT_Y_Less(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition);
                }
            }

            // unsigned long long ForwardScanBased_PlaneSweep_CNT_Less(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition);

        }
        
    }
}

#endif