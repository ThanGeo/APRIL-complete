#include "def.h"

namespace spatial_lib 
{
    QueryOutputT g_queryOutput;

    void resetQueryOutput() {
        // result
        g_queryOutput.queryResults = 0;
        // topology relations results
        g_queryOutput.topologyRelationsResultMap.insert(std::make_pair(TR_DISJOINT, 0));
        g_queryOutput.topologyRelationsResultMap.insert(std::make_pair(TR_EQUAL, 0));
        g_queryOutput.topologyRelationsResultMap.insert(std::make_pair(TR_MEET, 0));
        g_queryOutput.topologyRelationsResultMap.insert(std::make_pair(TR_CONTAINS, 0));
        g_queryOutput.topologyRelationsResultMap.insert(std::make_pair(TR_COVERS, 0));
        g_queryOutput.topologyRelationsResultMap.insert(std::make_pair(TR_COVERED_BY, 0));
        g_queryOutput.topologyRelationsResultMap.insert(std::make_pair(TR_INSIDE, 0));
        g_queryOutput.topologyRelationsResultMap.insert(std::make_pair(TR_INTERSECT, 0));
        // statistics
        g_queryOutput.postMBRFilterCandidates = 0;
        g_queryOutput.refinementCandidates = 0;
        g_queryOutput.trueHits = 0;
        g_queryOutput.trueNegatives = 0;
        
        // time
        g_queryOutput.mbrFilterTime = 0;
        g_queryOutput.iFilterTime = 0;
        g_queryOutput.refinementTime = 0;

        
    }

    void countAPRILResult(int result) {
        switch (result) {
            case TRUE_NEGATIVE:
                g_queryOutput.trueNegatives += 1;
                break;
            case TRUE_HIT:
                g_queryOutput.trueHits += 1;
                break;
            case INCONCLUSIVE:
                g_queryOutput.refinementCandidates += 1;
                break;
        }
    }

    void countResult(){
        g_queryOutput.queryResults += 1;
    }

    void countTopologyRelationResult(int relation) {
        g_queryOutput.topologyRelationsResultMap[relation] += 1;
    }
    
    static void deepCopyAprilData(AprilDataT* from, AprilDataT* to) {
        to->numIntervalsALL = from->numIntervalsALL;
        to->intervalsALL = from->intervalsALL;
        to->numIntervalsFULL = from->numIntervalsFULL;
        to->intervalsFULL = from->intervalsFULL;
    }

    void addAprilDataToApproximationDataMap(DatasetT &dataset, uint recID, AprilDataT aprilData) {
        AprilDataT copyAprilData;

        deepCopyAprilData(&aprilData, &copyAprilData);
        
        dataset.aprilData.insert(std::make_pair(recID, copyAprilData));
    }

    AprilDataT* getAprilDataOfObjectFromDatasetMap(Dataset &dataset, uint recID) {
        if (auto it = dataset.aprilData.find(recID); it != dataset.aprilData.end()) {
            return &it->second;
        }
        return NULL;
    }

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

    spatial_lib::bg_polygon loadPolygonFromDiskBoostGeometry(uint recID, std::ifstream &fin, std::unordered_map<uint,unsigned long> &offsetMap) {
        spatial_lib::bg_polygon pol;
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

                pol.outer().push_back(spatial_lib::bg_point_xy(x,y));
            }
        }
        boost::geometry::correct(pol);
        return pol;
    }
}