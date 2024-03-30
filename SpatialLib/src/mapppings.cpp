#include "mappings.h"

namespace spatial_lib
{
    std::unordered_map<int,int> queryTypeToNumberOfInputsMap = {
        {spatial_lib::RANGE, 2},
        {spatial_lib::Q_INTERSECTION_JOIN, 2},
        {spatial_lib::Q_WITHIN_JOIN, 2},
        {spatial_lib::Q_FIND_RELATION, 2},
        {spatial_lib::Q_RELATE, 2}
        };

    std::string queryTypeIntToText(int val){
        switch(val) {
            case spatial_lib::Q_INTERSECTION_JOIN: return "INTERSECTION_JOIN";
            case spatial_lib::Q_WITHIN_JOIN: return "WITHIN_JOIN";
            case spatial_lib::Q_FIND_RELATION: return "FIND_RELATION";
            case spatial_lib::Q_RELATE: return "RELATE";
        }
    }

    std::string dataTypeIntToText(int val){
        switch(val) {
            case spatial_lib::DT_POLYGON: return "POLYGON";
            case spatial_lib::DT_POINT: return "POINT";
            case spatial_lib::DT_LINESTRING: return "LINESTRING";
        }
    }

    DataTypeE dataTypeTextToInt(std::string val){
        if (val == "POLYGON") return spatial_lib::DT_POLYGON;
        if (val == "POINT") return spatial_lib::DT_POINT;
        if (val == "LINESTRING") return spatial_lib::DT_LINESTRING;


        return DT_INVALID;
    }
}