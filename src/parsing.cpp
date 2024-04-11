#include "parsing.h"

// property tree var
static boost::property_tree::ptree system_config_pt;
static boost::property_tree::ptree dataset_config_pt;

static void parseDatasetOptions(QueryStatementT *queryStmt) {
    // check if datasets.ini file exists
    FILE *fptr = fopen(g_config.dirPaths.datasetsConfigPath.c_str(), "r");
    if (fptr == NULL) {
        std::cout << g_config.dirPaths.datasetsConfigPath << " doesnt exist" << std::endl;
        log_err("Dataset configuration file 'dataset.ini' missing from Database directory. Please refer to the README file.");
        return;
    }
    fclose(fptr);

    // cmd line argument
    std::string nicknameR = queryStmt->datasetPathR;
    std::string nicknameS = queryStmt->datasetPathS;

    // if not specified
    if (nicknameR == "" || nicknameS == "") {
        // run default scenario (dev only)
        queryStmt->datasetPathR = dataset_config_pt.get<std::string>("T1NA.path");
        queryStmt->datasetPathS = dataset_config_pt.get<std::string>("T2NA.path");
        nicknameR = "T1NA";
        nicknameR = "T2NA";
    } else {
        queryStmt->datasetPathR = dataset_config_pt.get<std::string>(nicknameR+".path");
        queryStmt->datasetPathS = dataset_config_pt.get<std::string>(nicknameS+".path");
    }

    // dataset count (todo, make more flexible)
    queryStmt->datasetCount = 2;
    
    // dataset data types
    spatial_lib::DataTypeE datatypeR = spatial_lib::dataTypeTextToInt(dataset_config_pt.get<std::string>(nicknameR+".datatype"));
    if (datatypeR == spatial_lib::DT_INVALID) {
        log_err("Unknown data type for dataset R.");
        exit(-1);
    }
    spatial_lib::DataTypeE datatypeS = spatial_lib::dataTypeTextToInt(dataset_config_pt.get<std::string>(nicknameS+".datatype"));
    if (datatypeS == spatial_lib::DT_INVALID) {
        log_err("Unknown data type for dataset S.");
        exit(-1);
    }
    queryStmt->datatypeR = datatypeR;
    queryStmt->datatypeS = datatypeS;
    // datatype combination
    if(datatypeR == spatial_lib::DT_POLYGON && datatypeS == spatial_lib::DT_POLYGON) {
        queryStmt->datasetTypeCombination = spatial_lib::POLYGON_POLYGON;
    } else {
        log_err("Dataset data type combination not yet supported.");
        exit(-1);
    }
    
    // offset maps
    queryStmt->offsetMapPathR = dataset_config_pt.get<std::string>(nicknameR+".offsetMapPath");
    if (queryStmt->offsetMapPathR == "") {
        log_err("Missing offset map path for dataset R.");
    }
    queryStmt->offsetMapPathS = dataset_config_pt.get<std::string>(nicknameS+".offsetMapPath");
    if (queryStmt->offsetMapPathS == "") {
        log_err("Missing offset map path for dataset S.");
    }

    // hardcoded bounds
    double xMinR = std::numeric_limits<int>::max();
    double yMinR = std::numeric_limits<int>::max();
    double xMaxR = -std::numeric_limits<int>::min();
    double yMaxR = -std::numeric_limits<int>::max();
    if(dataset_config_pt.get<int>(nicknameR+".bounds")) {
        xMinR = dataset_config_pt.get<double>(nicknameR+".xMin");
        yMinR = dataset_config_pt.get<double>(nicknameR+".yMin");
        xMaxR = dataset_config_pt.get<double>(nicknameR+".xMax");
        yMaxR = dataset_config_pt.get<double>(nicknameR+".yMax");
        queryStmt->boundsSet = true;
        success_text("Set hardcoded bounds for R");
    }
    double xMinS = std::numeric_limits<int>::max();
    double yMinS = std::numeric_limits<int>::max();
    double xMaxS = -std::numeric_limits<int>::min();
    double yMaxS = -std::numeric_limits<int>::max();
    if(dataset_config_pt.get<int>(nicknameS+".bounds")) {
        xMinS = dataset_config_pt.get<double>(nicknameS+".xMin");
        yMinS = dataset_config_pt.get<double>(nicknameS+".yMin");
        xMaxS = dataset_config_pt.get<double>(nicknameS+".xMax");
        yMaxS = dataset_config_pt.get<double>(nicknameS+".yMax");
        queryStmt->boundsSet = true;
        success_text("Set hardcoded bounds for S");
    }
    // if they have different hardcoded bounds, assign as global the outermost ones (min of min, max of max)
    queryStmt->xMinGlobal = min(xMinR, xMinS);
    queryStmt->yMinGlobal = min(yMinR, yMinS);
    queryStmt->xMaxGlobal = max(xMaxR, xMaxS);
    queryStmt->yMaxGlobal = max(yMaxR, yMaxS);
}

static void parseQueryOptions(QueryStatementT *queryStmt) {
    
    // query statement is used to verify and build the query
    if (!verifyAndbuildQuery(queryStmt)){
        log_err("Failed while verifying and building query config");
        exit(-1);
    }
    
    
}

static void parseAPRILoptions(iFilterStatementT *iFilterStmt) {
    int N, compression, partitions;
    // N
    if (iFilterStmt->N == -1) {
        N = system_config_pt.get<int>("APRIL.N");
        if (N <= 1 || N > 16) {
            log_err("Error parsing N from config file. N has to be in [2,16].");
            exit(-1);
        }
    }
    uint cellsPerDimension = pow(2,N);
    // compression
    if (iFilterStmt->compression == -1) {
        compression = system_config_pt.get<int>("APRIL.compression");
        if (compression < 0 || compression > 1) {
            log_err("Error parsing compression setting from config file. Has to be 0 or 1.");
            exit(-1);
        }
    }
    // partitions
    if (iFilterStmt->partitions == -1) {
        // if not set by argument, read default from ini file
        partitions = system_config_pt.get<int>("APRIL.partitions");
        if (partitions <= 0 || partitions > 32) {
            log_err("Error parsing partition setting from config file. Has to be in [1,32].");
            exit(-1);
        }
    } else {
        // set from argument 
        partitions = iFilterStmt->partitions;
    }

    // set APRIL path in config
    g_config.dirPaths.approximationPath = system_config_pt.get<string>("APRIL.dir");
    if (!verifyDirectoryExists(g_config.dirPaths.approximationPath)) {
        log_err("Cannot open directory.");
        printf("\t%s\n", g_config.dirPaths.approximationPath.c_str());
        exit(-1);
    }

    // SAME FOR BOTH INPUT DATASETS
    // remember to delete on program exit
    // R
    spatial_lib::AprilConfigT aprilConfigR;
    aprilConfigR.N = N;
    aprilConfigR.cellsPerDim = cellsPerDimension;
    aprilConfigR.compression = compression;
    aprilConfigR.partitions = partitions;
    aprilConfigR.ALL_intervals_path =  getAPRILFilePath(g_config.queryData.R.datasetName, g_config.dirPaths.approximationPath, "ALL", N, partitions, compression);
    aprilConfigR.FULL_intervals_path =  getAPRILFilePath(g_config.queryData.R.datasetName, g_config.dirPaths.approximationPath, "FULL", N, partitions, compression);
    g_config.queryData.R.aprilConfig = aprilConfigR;
    g_config.queryData.R.approxType =  spatial_lib::AT_APRIL;
    
    // S
    spatial_lib::AprilConfigT aprilConfigS;
    aprilConfigS.N = N;
    aprilConfigS.cellsPerDim = cellsPerDimension;
    aprilConfigS.compression = compression;
    aprilConfigS.partitions = partitions;
    aprilConfigS.ALL_intervals_path =  getAPRILFilePath(g_config.queryData.S.datasetName, g_config.dirPaths.approximationPath, "ALL", N, partitions, compression);
    aprilConfigS.FULL_intervals_path =  getAPRILFilePath(g_config.queryData.S.datasetName, g_config.dirPaths.approximationPath, "FULL", N, partitions, compression);
    g_config.queryData.S.aprilConfig = aprilConfigS;
    g_config.queryData.S.approxType =  spatial_lib::AT_APRIL;
}

static void parseIntermediateFilterOptions(iFilterStatementT *iFilterStmt) {
    switch (g_config.pipeline.iFilterType) {
        case spatial_lib::AT_APRIL:
            parseAPRILoptions(iFilterStmt);
            break;
        case spatial_lib::AT_NONE:
            break;
    }
}

static void parsePipelineOptions(std::string &iFilterTypeStr) {
    // verify and build pipeline
    if (iFilterTypeStr == "") {
        // if argument didnt specify, read from config file
        iFilterTypeStr = system_config_pt.get<std::string>("Pipeline.IntermediateFilterType");
    }

    if (!verifyAndBuildPipeline(system_config_pt.get<int>("Pipeline.mbrFilter"), iFilterTypeStr, system_config_pt.get<int>("Pipeline.refinement"))) {
        log_err("Failed while verifying and building pipeline config");
        exit(-1);
    }
}

void parseArgumentsAndConfigurationFile(int argc, char *argv[]) {
    char c;
    QueryStatementT queryStmt;
    iFilterStatementT iFilterStmt;
    // check If config file does exist
    FILE *fptr = fopen(g_config.dirPaths.configFilePath.c_str(), "r");
    if (fptr == NULL) {
        std::cout << g_config.dirPaths.configFilePath << " doesnt exist" << std::endl;
        log_err("Configuration file 'config.ini' missing from Database directory.");
        return;
    }
    fclose(fptr);

    // read arguments
    while ((c = getopt(argc, argv, "p:cf:q:R:S:?")) != -1)
    {
        switch (c)
        {
            case 'c':
                g_config.actions.createApproximations = true;
                break;
            case 'q':
                // Query Type
                queryStmt.queryType = std::string(optarg);
                break;   
            case 'f':
                // Intermediate Filter Type
                iFilterStmt.iFiltertypeStr = std::string(optarg);
                break;
            case 'R':
                // Dataset R path
                queryStmt.datasetPathR = std::string(optarg);
                queryStmt.datasetCount++;
                break;
            case 'S':
                // Dataset S path
                queryStmt.datasetPathS = std::string(optarg);
                queryStmt.datasetCount++;
                break;
            case 'p':
                iFilterStmt.partitions = atoi(optarg);
                break;
            default:
                exit(-1);
                break;
        }
    }
    // parse configuration files
    boost::property_tree::ini_parser::read_ini(g_config.dirPaths.configFilePath, system_config_pt);
    boost::property_tree::ini_parser::read_ini(g_config.dirPaths.datasetsConfigPath, dataset_config_pt);

    // parse pipeline options FIRST
    parsePipelineOptions(iFilterStmt.iFiltertypeStr);

    // parse dataset options
    parseDatasetOptions(&queryStmt);

    // parse query options
    parseQueryOptions(&queryStmt);

    // parse intermediate filter options
    parseIntermediateFilterOptions(&iFilterStmt);

}