#ifndef SPATIAL_LIB_DATA_H
#define SPATIAL_LIB_DATA_H

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

    // topological
    typedef enum TopologyRelation {
        TR_DISJOINT,
        TR_EQUAL,
        TR_INSIDE,
        TR_CONTAINS,
        TR_MEET,
        TR_COVERS,
        TR_COVERED_BY,
        TR_INTERSECTS,
        // specific refinement cases
        REFINE_CONTAIN_PLUS = 1000,
        REFINE_INSIDE_PLUS,
        REFINE_CONTAINMENT_ONLY,
        REFINE_NO_CONTAINMENT,
        REFINE_ALL_NO_EQUAL,
        REFINE_ALL,
    } TopologyRelationE;

    // spatial data types
    typedef enum DataType{
        DT_INVALID,
        DT_POINT,
        DT_LINESTRING,
        DT_POLYGON,
    } DataTypeE;

    // query data type combinations
    typedef enum DatasetTypeCombination {
        POLYGON_POLYGON,
        POLYGON_LINESTRING,
    }DatasetTypeCombinationE;

    /**
     * spatial data structs
     */
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

    // spatial approximations
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

    typedef enum IntermediateFilterType{
        IF_NONE,
        IF_APRIL,
    } IntermediateFilterTypeE;

    // APRIL data
    typedef struct AprilData {
        // APRIL data
        uint numIntervalsALL;
        std::vector<uint> intervalsALL;
        uint numIntervalsFULL = 0;
        std::vector<uint> intervalsFULL;
    }AprilDataT;

    // APRIL configuration
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

    // types of possible relationships between interval lists (IL)
    typedef enum IntervalListsRelationship {
        IL_DISJOINT,        // no containment, no intersection
        IL_INTERSECT,       // no containment, yes intersection
        IL_R_INSIDE_S,
        IL_S_INSIDE_R,
        IL_MATCH,           // match == symmetrical containment
    } IntervalListsRelationshipE;

    AprilDataT createEmptyAprilDataObject();
}

#endif