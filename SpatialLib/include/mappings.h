#ifndef SPATIAL_LIB_MAPPINGS_H
#define SPATIAL_LIB_MAPPINGS_H

#include "def.h"

namespace spatial_lib 
{
    extern std::unordered_map<int,int> queryTypeToNumberOfInputsMap;

    std::string queryTypeIntToText(int val);

    std::string dataTypeIntToText(int val);

    DataTypeE dataTypeTextToInt(std::string val);
}


#endif