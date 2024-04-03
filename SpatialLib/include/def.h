#ifndef SPATIAL_LIB_DEF_H
#define SPATIAL_LIB_DEF_H

#include <vector>
#include <sys/types.h>
#include <string>
#include <unordered_map>
#include <fstream>

#include "data.h"

namespace spatial_lib
{
    typedef enum QueryResult {
        TRUE_NEGATIVE,
        TRUE_HIT,
        INCONCLUSIVE,
    } QueryResultT;

    typedef struct QueryOutput {
        // for regular query rsesults
        int queryResults;
        // for topology relations results
        std::unordered_map<int,uint> topologyRelationsResultMap;
        // statistics
        int postMBRFilterCandidates;
        int refinementCandidates;
        int trueHits;
        int trueNegatives;
        // times
        double totalTime;
        double mbrFilterTime;
        double iFilterTime;
        double refinementTime;
    } QueryOutputT;

    // global query output variable
    extern QueryOutputT g_queryOutput;

    typedef enum QueryType{
        RANGE,
        Q_INTERSECT,
        Q_INSIDE,
        Q_DISJOINT,
        Q_EQUAL,
        Q_MEET,
        Q_CONTAINS,
        Q_COVERS,
        Q_COVERED_BY,
        Q_FIND_RELATION,    // find what type of topological relation is there
    }QueryTypeE;

    typedef struct Dataset{
        spatial_lib::DataTypeE dataType;
        std::string path;
        std::string offsetMapPath;
        // derived from the path
        std::string datasetName;
        // map: recID -> vector data (polygon, linestring etc.)
        std::unordered_map<uint, spatial_lib::VectorDataT> vectorData;
        /**
         * Approximations
        */
        ApproximationTypeE approxType;
        // APRIL
        spatial_lib::AprilConfigT aprilConfig;
        std::unordered_map<uint, spatial_lib::AprilDataT> aprilData;       // map: recID -> april data
    }DatasetT;

    typedef struct Query{
        spatial_lib::QueryTypeE type;
        int numberOfDatasets;
        spatial_lib::DatasetT R;         // R: left dataset
        spatial_lib::DatasetT S;         // S: right dataset
        bool boundsSet = false;
        double xMinGlobal, yMinGlobal, xMaxGlobal, yMaxGlobal;  // global bounds based on dataset bounds
    }QueryT;


    void resetQueryOutput();
    void countAPRILResult(int result);
    void countResult();
    void countTopologyRelationResult(int relation);

    void addAprilDataToApproximationDataMap(DatasetT &dataset, uint recID, AprilDataT aprilData);
    AprilDataT* getAprilDataOfObjectFromDatasetMap(Dataset &dataset, uint recID);

    std::unordered_map<uint,unsigned long> loadOffsetMap(std::string &offsetMapPath);
    spatial_lib::bg_polygon loadPolygonFromDiskBoostGeometry(uint recID, std::ifstream &fin, std::unordered_map<uint,unsigned long> &offsetMap);
}

#endif