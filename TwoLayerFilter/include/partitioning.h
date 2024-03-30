#ifndef TWO_LAYER_PARTITIONING_H
#define TWO_LAYER_PARTITIONING_H

#include "relation.h"

namespace two_layer{
    namespace partition{
        namespace oneArray{
            
            void Partition_One_Array(const Relation& R, const Relation& S, Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, int runNumPartitionsPerRelation);

            void Partition_One_Array_Dec(const Relation& R, const Relation& S, Relation *pR, Relation *pS, vector<Drec> *pRDdec, vector<Drec> *pSDdec, size_t * pRA_size, size_t * pSA_size, size_t * pRB_size, size_t * pSB_size, size_t * pRC_size, size_t * pSC_size, size_t * pRD_size, size_t * pSD_size, int runNumPartitionsPerRelation);

        };
    };
    namespace sort{
        
        namespace oneArray{
            void SortXStartOneArray(Relation *pR, Relation *pS, size_t *pRA_size , size_t *pSA_size , size_t *pRB_size, size_t *pSB_size,  int runNumPartitions);
            void SortYStartOneArray(Relation *pR, Relation *pS, size_t *pRB_size, size_t *pSB_size,  size_t *pRC_size, size_t *pSC_size,size_t *pRD_size, size_t *pSD_size, int runNumPartitions);
            void SortYStartOneArray2(Relation *pR, Relation *pS, size_t *pRB_size, size_t *pSB_size,  size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size ,int runNumPartitions);

        };
        namespace decomposition{
            void SortXStartOneArray(Relation *pR, Relation *pS, size_t *pRA_size , size_t *pSA_size , size_t *pRB_size, size_t *pSB_size,  int runNumPartitions);
            void SortYStartOneArray(Relation *pR, Relation *pS, size_t *pRB_size , size_t *pSB_size , size_t *pRC_size, size_t *pSC_size, int runNumPartitions);
            void copyDec(Relation *pR, Relation *pS,  vector<ABrec>* pRABdec, vector<ABrec>* pSABdec, vector<Crec> *pRCdec, vector<Crec> *pSCdec, vector<Coord>* pRYEND, vector<Coord>* pSYEND,size_t * pRB_size, size_t * pSB_size, size_t * pRC_size, size_t * pSC_size, int runNumPartitions );
        }
    };
    
    void PartitionTwoDimensional(const Relation& R, const Relation& S, Relation *pR, Relation *pS, size_t *pRA_size, size_t *pSA_size, size_t *pRB_size, size_t *pSB_size, size_t *pRC_size, size_t *pSC_size, size_t *pRD_size, size_t *pSD_size, bool runPlaneSweepOnX, int runNumPartitionsPerRelation);
    void PartitionTwoDimensionalDec (const Relation& R, const Relation& S, Relation *pR, Relation *pS, vector<ABrec>* pRABdec, vector<ABrec>* pSABdec,  vector<Crec> *pRCdec, vector<Crec> *pSCdec, vector<Drec> *pRDdec, vector<Drec> *pSDdec, vector<Coord>* pRYEND, vector<Coord>* pSYEND, size_t * pRA_size, size_t * pSA_size, size_t * pRB_size, size_t * pSB_size, size_t * pRC_size, size_t * pSC_size, size_t * pRD_size, size_t * pSD_size, bool runPlaneSweepOnX, int runNumPartitionsPerRelation);
    void PartitionUniform(const Relation& R, Relation *pR, size_t *pRA_size, size_t *pRB_size, size_t *pRC_size, size_t *pRD_size, int runNumPartitionsPerRelation);
    void PartitionUniform(const Relation& R, Relation *pR, size_t *pR_size, int runNumPartitionsPerRelation);    
};

#endif