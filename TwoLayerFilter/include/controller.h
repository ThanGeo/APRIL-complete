#ifndef TWO_LAYER_CONTROLLER_H
#define TWO_LAYER_CONTROLLER_H

#include "def.h"
#include "partitioning.h"
#include "filter.h"
#include "SpatialLib.h"

namespace two_layer{
    /**
     * GLOBAL VARIABLES
    */
    extern Relation R, S, *pR, *pS;
    extern Relation *pRA, *pSA, *pRB, *pSB, *pRC, *pSC, *pRD, *pSD;
    extern size_t *pRA_size, *pSA_size, *pRB_size, *pSB_size, *pRC_size, *pSC_size, *pRD_size, *pSD_size;
    extern size_t *pR_size ,*pS_size;
    extern int runNumPartitionsPerRelation;
    extern int runProcessingMethod;
    extern int runNumPartitions; 
    extern int NUM_ITERATIONS;
    extern bool runPlaneSweepOnX;

    /**
     * ACCESS METHODS
    */

    /**
     * Requirements: Datasets R and S have been loaded properly.
     * Description: 
     * 1. Initializes environment for the Two-Layer filter to run.
     * 2. Prepares the data (partitioning & sorting) for the two-layer filter.
    */
    void init(double minX, double minY, double maxX, double maxY);

    /**
     * Description: loads a binary dataset from disk. Creates each object's MBR and saves the dataset's min/max coords as well.
     * @input: string &datasetPath : the complete path to the binary dataset
     * @input: bool left : true if its the left relation (R), false if its the right relation (S)
     * @output: none
     * @remarks: crashes if specified datapath is wrong
    */
    void loadBinaryDataset(string &datasetPath, bool left);
    void loadCSVDataset(string &datasetPath);
    /**
     * Requirements: (1) Datasets R and S have been loaded properly and (2) Two-layer environment has been initialized (init()).
     * Description: Evaluates the two-layer filter for datasets R and S.
     * @input: none
     * @output: total results
    */
    unsigned long long evaluateTwoLayer();

    /**
     * Description: Registers a user-defined function as the forwarding function for each result pair of the filter.
     * @input: a void function pointer with two uint arguments for the pair's identifiers (R and S, in that order). 
     * @output: 1 for success, -1 for failure.
    */
    int registerForwardingFunction(void (*forwardingFuncPtr)(uint idR, uint idS));

    /**
     * !UNDER CONSTRUCTION!
     * Description: Registers a user-defined function as the forwarding function for each result pair of the filter. 
     * TODO: Accepts variadic functions to allow more customization.
     * @input: 
     * @output: 1 for success, -1 for failure.
    */
    int registerVoidForwardingFunction(void (*forwardingFuncPtr)(int count, ...));
    /**
     * Description: Sets how many partitions are used per dimension in the grid.
     * @input: int numPartitions : number of partitions per dimension
     * @output: none
     * @remarks: if this is invoked after initialization, environment needs to be initialized again using init().
    */
    void setPartitionsNum(int numPartitions);


    /**
     * Sets the (xMin,yMin),(xMax,yMax) values based on the datasets R,S min/max
     * giving the total boundaries that enclose both datasets
    */
    void getDatasetGlobalBounds(double &xMin, double &yMin, double &xMax, double &yMax);
}

#endif