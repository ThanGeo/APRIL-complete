#include "utils.h"

namespace APRIL
{
    std::unordered_map<uint,unsigned long> loadOffsetMap(std::string &offsetMapPath){
        unsigned long offset;
        uint lineCounter = 0;
        uint recID;
        std::ifstream fin(offsetMapPath, std::fstream::in | std::ios_base::binary);
        std::unordered_map<uint,unsigned long> offset_map;
        int totalLines;

        //read total lines
        fin.read((char*) &totalLines, sizeof(int));

        while(lineCounter < totalLines){
            //read rec id
            fin.read((char*) &recID, sizeof(int));
            //read byte offset
            fin.read((char*) &offset, sizeof(unsigned long));

            offset_map.insert(std::make_pair(recID, offset));		
            lineCounter++;
        }
        fin.close();

        return offset_map;
    }

    spatial_lib::bg_polygon* loadPolygonFromDiskBoostGeometry(uint recID, std::ifstream &fin, std::unordered_map<uint,unsigned long> &offsetMap) {
        spatial_lib::bg_polygon* pol = new spatial_lib::bg_polygon;
        int readID;
        int vertexCount, polygonCount;
        double x,y;
        //search the map for the specific polygon offset
        std::unordered_map<uint,unsigned long>::const_iterator got = offsetMap.find(recID);
        if(got != offsetMap.end()){ 
            //set read offset
            fin.seekg(got->second-fin.tellg(), fin.cur);		
            //read rec ID
            fin.read((char*) &readID, sizeof(int));
            //read vertex count
            fin.read((char*) &vertexCount, sizeof(int));
            for(int i=0; i<vertexCount; i++){
                fin.read((char*) &x, sizeof(double));
                fin.read((char*) &y, sizeof(double));

                pol->outer().push_back(spatial_lib::bg_point_xy(x,y));
            }
        }
        boost::geometry::correct(*pol);
        return pol;
    }
}