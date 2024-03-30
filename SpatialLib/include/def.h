#ifndef SPATIAL_LIB_DEF_H
#define SPATIAL_LIB_DEF_H

#include <vector>
#include <sys/types.h>
#include <string>
#include <unordered_map>
#include <fstream>

#include <boost/geometry.hpp>
#include <boost/geometry/algorithms/assign.hpp>
#include <boost/foreach.hpp>
#include <boost/assign.hpp>

namespace spatial_lib
{
    // boost geometry
    typedef boost::geometry::model::d2::point_xy<double> bg_point_xy;
    typedef boost::geometry::model::linestring<bg_point_xy> bg_linestring;
    typedef boost::geometry::model::polygon<bg_point_xy> bg_polygon;

    typedef enum QueryResult {
        TRUE_NEGATIVE,
        TRUE_HIT,
        INCONCLUSIVE,
    } QueryResultT;

    typedef struct QueryOutput {
        // result
        int queryResults;
        // statistics
        int postMBRFilterCandidates;
        int refinementCandidates;
        int trueHits;
        int trueNegatives;
        // times
        double totalTime;
        double mbrFilterTime;
        double intermediateFilterTime;
        double refinementTime;
    } QueryOutputT;

    // global query output variable
    extern QueryOutputT g_queryOutput;

    typedef enum DataType{
        DT_INVALID,
        DT_POINT,
        DT_LINESTRING,
        DT_POLYGON,
    } DataTypeE;

    typedef enum DatasetTypeCombination {
        POLYGON_POLYGON,
        POLYGON_LINESTRING,
    }DatasetTypeCombinationE;

    typedef struct Point {
        double x,y;
    } PointT;

    typedef struct Mbr {
        PointT minPoint;
        PointT maxPoint;
    }MbrT;

    typedef struct Polygon {
        uint recID;
        std::vector<PointT> vertices;
    } PolygonT;

    typedef struct VectorData {
        DataTypeE type;
        void* data;
    } VectorDataT;

    typedef enum ApproximationType{
        AT_NONE,
        // mine
        AT_APRIL,
        AT_RI,
        // competitors
        AT_5CCH,
        AT_RA,
        AT_GEOS,
    } ApproximationTypeE;

    typedef struct AprilData {
        // APRIL data
        uint numIntervalsALL;
        std::vector<uint> intervalsALL;
        uint numIntervalsFULL = 0;
        std::vector<uint> intervalsFULL;
    }AprilDataT;

    /**
     * APRIL CONFIGURATION
    */
    typedef struct AprilConfig {
        // Hilbert curve order
        int N;
        // cells per dimension in Hilbert grid: 2^N
        uint cellsPerDim;
        // compression enabled, disabled
        bool compression;
        // how many partitions in the data space
        int partitions;
        // APRIL data file paths
        std::string ALL_intervals_path;
        std::string FULL_intervals_path;
    } AprilConfigT;

    typedef enum QueryType{
        RANGE,
        Q_INTERSECTION_JOIN,
        Q_WITHIN_JOIN,
        Q_FIND_RELATION,    // find what type of topological relation is there
        Q_RELATE,           // check for a specific relation
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
        std::unordered_map<uint, spatial_lib::AprilDataT*> aprilData;       // map: recID -> april data
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

    AprilDataT createEmptyAprilDataObject();
    void addAprilDataToApproximationDataMap(DatasetT &dataset, uint recID, AprilDataT aprilData);
    AprilDataT* getAprilDataOfObject(Dataset &dataset, uint recID);


    std::unordered_map<uint,unsigned long> loadOffsetMap(std::string &offsetMapPath);
    spatial_lib::bg_polygon* loadPolygonFromDiskBoostGeometry(uint recID, std::ifstream &fin, std::unordered_map<uint,unsigned long> &offsetMap);
}

#endif