#ifndef APRIL_UTILS_H
#define APRIL_UTILS_H

#include "SpatialLib.h"
#include <fstream>

namespace APRIL
{
    
    std::unordered_map<uint,unsigned long> loadOffsetMap(std::string &offsetMapPath);
    spatial_lib::bg_polygon* loadPolygonFromDiskBoostGeometry(uint recID, std::ifstream &fin, std::unordered_map<uint,unsigned long> &offsetMap);
}


#endif