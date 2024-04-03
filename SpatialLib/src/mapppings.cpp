#include "mappings.h"

namespace spatial_lib
{
    std::string queryTypeIntToText(int val){
        switch(val) {
            case spatial_lib::Q_INTERSECT: return "intersect";
            case spatial_lib::Q_INSIDE: return "inside";
            case spatial_lib::Q_FIND_RELATION: return "find_relation";
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