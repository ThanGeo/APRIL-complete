#ifndef MAIN_CONFIG_H
#define MAIN_CONFIG_H

#include "TwoLayer.h"
#include "def.h"
#include "utils.h"

typedef struct ConfigActions {
    bool createApproximations = false;
} ConfigActionsT;

typedef struct Config {
    DirectoryPathT dirPaths;
    ConfigActionsT actions;
    spatial_lib::QueryT queryData;
    PipelineT pipeline;
} ConfigT;

extern ConfigT g_config;


typedef struct QueryStatement
{
    std::string queryType = "";
    std::string datasetPathR = "", datasetPathS = "";
    std::string offsetMapPathR = "", offsetMapPathS = "";
    int datasetTypeCombination = -1;
    int datasetCount = 0;
    spatial_lib::DataTypeE datatypeR, datatypeS;
    bool boundsSet = false;
    double xMinGlobal, yMinGlobal, xMaxGlobal, yMaxGlobal;
}QueryStatementT;

typedef struct iFilterStatement
{
    std::string iFiltertypeStr = "";
    // FOR APRIL
    int N = -1;
    int compression = -1;
    int partitions = -1;
    // FOR OTHERS
} iFilterStatementT;

void printConfig();

bool verifyAndbuildQuery(QueryStatementT *queryStmt);
bool verifyAndBuildPipeline(int mbrFilterEnabled, std::string intermediateFilterType, int refinementEnabled);
bool verifyDirectoryExists(std::string &directoryPath);

/**
 * initializes the environemnt/database based on the parsed configuration
*/
void initConfig();

#endif