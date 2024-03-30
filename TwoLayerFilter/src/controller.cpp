#include "controller.h"

namespace two_layer{
    /**
     * GLOBAL VARIABLES
    */
    Relation R, S, *pR, *pS;
    Relation *pRA, *pSA, *pRB, *pSB, *pRC, *pSC, *pRD, *pSD;
    size_t *pRA_size, *pSA_size, *pRB_size, *pSB_size, *pRC_size, *pSC_size, *pRD_size, *pSD_size;
    size_t *pR_size ,*pS_size;
    int runNumPartitionsPerRelation = -1;
    int runProcessingMethod = -1;
    int runNumPartitions = -1; 
    int NUM_ITERATIONS = -1;
    bool runPlaneSweepOnX = false;;

    /**
     * ACCESS METHODS
    */

    /**
     * Requirements: Datasets R and S have been loaded properly.
     * Description: 
     * 1. Initializes the environment for the Two-Layer filter to run.
     * 2. Prepares the data (partitioning & sorting) for the two-layer filter.
     * 3. Sets the global bound coordinates (return arguments)
    */
    void init(double minX, double minY, double maxX, double maxY) {
        Coord diffX = maxX - minX;
        Coord diffY = maxY - minY;
        Coord maxExtend = (diffX<diffY)?diffY:diffX;

        //normalize for MBR filter
        R.normalize(minX, maxX, minY, maxY, maxExtend);
        S.normalize(minX, maxX, minY, maxY, maxExtend);  

        //PREPARE MBR FILTER
        runNumPartitions = runNumPartitionsPerRelation * runNumPartitionsPerRelation;
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

        // partitioning and sorting
        PartitionTwoDimensional(R, S, pR, pS, pRA_size, pSA_size, pRB_size, pSB_size, pRC_size, pSC_size, pRD_size, pSD_size,runPlaneSweepOnX, runNumPartitionsPerRelation);
        sort::oneArray::SortYStartOneArray(pR, pS, pRB_size, pSB_size, pRC_size, pSC_size , pRD_size, pSD_size, runNumPartitions);

    }

    /**
     * Description: loads a binary dataset from disk. Creates each object's MBR and saves the dataset's min/max coords as well.
     * @input: string &datasetPath : the complete path to the binary dataset
     * @input: bool left : true if its the left relation (R), false if its the right relation (S)
     * @output: none
     * @remarks: crashes if specified datapath is wrong
    */
    void loadBinaryDataset(string &datasetPath, bool left) {
        if (left) {
            // load R
            R.load(datasetPath);
        } else {
            // load S
            S.load(datasetPath);
        }
    }

    void loadCSVDataset(string &datasetPath) {
        cout << "NOT IMPLEMENTED YET" << endl;
        exit(-1);
    }

    /**
     * Requirements: (1) Datasets R and S have been loaded properly and (2) Two-layer environment has been initialized (init()).
     * Description: Evaluates the two-layer filter for datasets R and S.
     * @input: none
     * @output: total results
    */
    unsigned long long evaluateTwoLayer() {
        return ForwardScanBased_PlaneSweep_CNT_Less(pR, pS, pRA_size, pSA_size, pRB_size, pSB_size, pRC_size, pSC_size, pRD_size, pSD_size, runPlaneSweepOnX, runNumPartitionsPerRelation);
    }

    /**
     * Description: Registers a user-defined function as the forwarding function for each result pair of the filter.
     * @input: a void function pointer with two uint arguments for the pair's identifiers (R and S, in that order). 
     * @output: 1 for success, -1 for failure.
    */
    int registerForwardingFunction(void (*forwardingFuncPtr)(uint idR, uint idS)) {
        try {
            g_pairIdForwardingFunctionPtr = forwardingFuncPtr;
            return 1;
        }
        catch (int e) {
            cout << "Two-layer: An exception occurred during function forwarding registration. Exception Nr. " << e << '\n';
            return -1;
        }
    }

    /**
     * !UNDER CONSTRUCTION!
     * Description: Registers a user-defined function as the forwarding function for each result pair of the filter. 
     * TODO: Accepts variadic functions to allow more customization.
     * @input: 
     * @output: 1 for success, -1 for failure.
    */
    int registerVoidForwardingFunction(void (*forwardingFuncPtr)(int count, ...)) {
        cout << "ERROR: FUNCTION UNDER CONSTRUCTION" << endl;
        return -1;
        try {
            g_pairIdForwardingVoidFunctionPtr = forwardingFuncPtr;
            return 1;
        }
        catch (int e) {
            cout << "Two-layer: An exception occurred during function forwarding registration. Exception Nr. " << e << '\n';
            return -1;
        }
    }

    /**
     * Description: Sets how many partitions are used per dimension in the grid.
     * @input: int numPartitions : number of partitions per dimension
     * @output: none
     * @remarks: if this is invoked after initialization, environment needs to be initialized again using init().
    */
    void setPartitionsNum(int numPartitions) {
        runNumPartitionsPerRelation = numPartitions;
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

}
