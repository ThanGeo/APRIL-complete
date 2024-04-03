#include "controller.h"


namespace two_layer
{
    int partitionsPerDimension = 1000;
    int runNumPartitions = partitionsPerDimension * partitionsPerDimension;
    Relation R, S, *pR, *pS;
    Relation *pRA, *pSA, *pRB, *pSB, *pRC, *pSC, *pRD, *pSD;
    size_t *pRA_size, *pSA_size, *pRB_size, *pSB_size, *pRC_size, *pSC_size, *pRD_size, *pSD_size;
    size_t *pR_size ,*pS_size;
    vector<ABrec> *pRABdec , *pSABdec;
    vector<Crec> *pRCdec, *pSCdec;
    vector<Drec> *pRDdec, *pSDdec;
    vector<Coord> *pRYEND, *pSYEND;     

    unsigned long long result = 0;
    
    void initTwoLayer(uint partitionsPerDimension) {
        // get global boundaries of datasets
        Coord minX = min(R.minX, S.minX);
        Coord maxX = max(R.maxX, S.maxX);
        Coord minY = min(R.minY, S.minY);
        Coord maxY = max(R.maxY, S.maxY);
        Coord diffX = maxX - minX;
        Coord diffY = maxY - minY;
        Coord maxExtend = (diffX<diffY)?diffY:diffX;
        //normalize for MBR filter
        R.normalize(minX, maxX, minY, maxY, maxExtend);
        S.normalize(minX, maxX, minY, maxY, maxExtend);

        //PREPARE MBR FILTER
        runNumPartitions = partitionsPerDimension * partitionsPerDimension;
        pRA_size = new size_t[runNumPartitions];
        pRB_size = new size_t[runNumPartitions];
        pRC_size = new size_t[runNumPartitions];
        pRD_size = new size_t[runNumPartitions];

        pSA_size = new size_t[runNumPartitions];
        pSB_size = new size_t[runNumPartitions];
        pSC_size = new size_t[runNumPartitions];
        pSD_size = new size_t[runNumPartitions];

        memset(pRA_size, 0, runNumPartitions*sizeof(size_t));
        memset(pSA_size, 0, runNumPartitions*sizeof(size_t));
        memset(pRB_size, 0, runNumPartitions*sizeof(size_t));
        memset(pSB_size, 0, runNumPartitions*sizeof(size_t));
        memset(pRC_size, 0, runNumPartitions*sizeof(size_t));
        memset(pSC_size, 0, runNumPartitions*sizeof(size_t));
        memset(pRD_size, 0, runNumPartitions*sizeof(size_t));
        memset(pSD_size, 0, runNumPartitions*sizeof(size_t));

        pR = new Relation[runNumPartitions];
        pS = new Relation[runNumPartitions];
        //MBR FILTER PRE-PROCESSING (partitioning and sorting)
        fs_2d::single::PartitionTwoDimensional(R, S, pR, pS, pRA_size, pSA_size, pRB_size, pSB_size, pRC_size, pSC_size, pRD_size, pSD_size, partitionsPerDimension);
        fs_2d::single::sort::oneArray::SortYStartOneArray(pR, pS, pRB_size, pSB_size, pRC_size, pSC_size , pRD_size, pSD_size, runNumPartitions);
    }

    void initTwoLayer(uint partitionsPerDimension, double xMin, double yMin, double xMax, double yMax) {
        //fix the relation data space to be the same as our Hilbert data space
        Coord minX = xMin;
        Coord maxX = xMax;
        Coord minY = yMin;
        Coord maxY = yMax;
        Coord diffX = maxX - minX;
        Coord diffY = maxY - minY;
        Coord maxExtend = (diffX<diffY)?diffY:diffX;
        //normalize for MBR filter
        R.normalize(minX, maxX, minY, maxY, maxExtend);
        S.normalize(minX, maxX, minY, maxY, maxExtend);

        //PREPARE MBR FILTER
        runNumPartitions = partitionsPerDimension * partitionsPerDimension;
        pRA_size = new size_t[runNumPartitions];
        pRB_size = new size_t[runNumPartitions];
        pRC_size = new size_t[runNumPartitions];
        pRD_size = new size_t[runNumPartitions];

        pSA_size = new size_t[runNumPartitions];
        pSB_size = new size_t[runNumPartitions];
        pSC_size = new size_t[runNumPartitions];
        pSD_size = new size_t[runNumPartitions];

        memset(pRA_size, 0, runNumPartitions*sizeof(size_t));
        memset(pSA_size, 0, runNumPartitions*sizeof(size_t));
        memset(pRB_size, 0, runNumPartitions*sizeof(size_t));
        memset(pSB_size, 0, runNumPartitions*sizeof(size_t));
        memset(pRC_size, 0, runNumPartitions*sizeof(size_t));
        memset(pSC_size, 0, runNumPartitions*sizeof(size_t));
        memset(pRD_size, 0, runNumPartitions*sizeof(size_t));
        memset(pSD_size, 0, runNumPartitions*sizeof(size_t));

        pR = new Relation[runNumPartitions];
        pS = new Relation[runNumPartitions];
        //MBR FILTER PRE-PROCESSING (partitioning and sorting)
        fs_2d::single::PartitionTwoDimensional(R, S, pR, pS, pRA_size, pSA_size, pRB_size, pSB_size, pRC_size, pSC_size, pRD_size, pSD_size, partitionsPerDimension);
        fs_2d::single::sort::oneArray::SortYStartOneArray(pR, pS, pRB_size, pSB_size, pRC_size, pSC_size , pRD_size, pSD_size, runNumPartitions);
    }

    /**
     * Sets the (xMin,yMin),(xMax,yMax) values based on the datasets R,S min/max
     * giving the total boundaries that enclose both datasets
    */
    void getDatasetGlobalBounds(double &xMin, double &yMin, double &xMax, double &yMax) {
        xMin = min(R.minX, S.minX);
        yMin = min(R.minY, S.minY);
        xMax = max(R.maxX, S.maxX);
        yMax = max(R.maxY, S.maxY);
    }

    void loadDatasets(std::string &Rpath, std::string &Spath) {
        // Load inputs (creates MBRs from geometry files)
        #pragma omp parallel sections
        {
            #pragma omp section
            {
                R.load(Rpath);
            }
            #pragma omp section
            {
                S.load(Spath);
            }
        }
        cout << "R size: " << R.size() << endl;
        cout << "S size: " << S.size() << endl;
    }

    void setNextStage(spatial_lib::IntermediateFilterTypeE iFilterType) {
        g_iFilterType = iFilterType;
    }


    unsigned long long evaluateTwoLayer() {
        return ForwardScanBased_PlaneSweep_CNT_Y_Less(pR, pS, pRA_size, pSA_size, pRB_size, pSB_size, pRC_size, pSC_size, pRD_size, pSD_size, runNumPartitions);
    }


    
}