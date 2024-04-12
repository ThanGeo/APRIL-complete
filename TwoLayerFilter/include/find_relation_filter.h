#ifndef TWO_LAYER_FIND_RELATION_FILTER_H
#define TWO_LAYER_FIND_RELATION_FILTER_H

#include "relation.h"
#include "SpatialLib.h"
#include "APRIL.h"

namespace two_layer
{
    unsigned long long FindRelationMBRFilter(Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartition);
}


#endif