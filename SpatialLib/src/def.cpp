#include "def.h"

namespace spatial_lib 
{
    QueryOutputT g_queryOutput;

    void resetQueryOutput() {
        // result
        g_queryOutput.queryResults = 0;
        // statistics
        g_queryOutput.postMBRFilterCandidates = 0;
        g_queryOutput.refinementCandidates = 0;
        g_queryOutput.trueHits = 0;
        g_queryOutput.trueNegatives = 0;
        
        // time
        g_queryOutput.mbrFilterTime = 0;
        g_queryOutput.intermediateFilterTime = 0;
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
    
    static void deepCopyAprilData(AprilDataT* from, AprilDataT* to) {
        to->numIntervalsALL = from->numIntervalsALL;
        to->intervalsALL = from->intervalsALL;
        to->numIntervalsFULL = from->numIntervalsFULL;
        to->intervalsFULL = from->intervalsFULL;
    }

    AprilDataT createEmptyAprilDataObject() {
        AprilDataT aprilData;
        aprilData.numIntervalsALL = 0;
        aprilData.numIntervalsFULL = 0;
        return aprilData;
    }

    void addAprilDataToApproximationDataMap(DatasetT &dataset, uint recID, AprilDataT aprilData) {
        AprilDataT* copyAprilData = new AprilData;

        deepCopyAprilData(&aprilData, copyAprilData);
        
        dataset.aprilData.insert(std::make_pair(recID, copyAprilData));
    }

    AprilDataT* getAprilDataOfObject(Dataset &dataset, uint recID) {
        if (auto it = dataset.aprilData.find(recID); it != dataset.aprilData.end()) {
            return it->second;
        }
        return NULL;
    }
}