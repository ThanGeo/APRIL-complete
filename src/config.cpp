#include "config.h"

/**
 * Global configuration variable
*/
ConfigT g_config;

static std::unordered_map<std::string, int> queryStringToIntMap = {
    {"range",spatial_lib::RANGE},
    {"intersection_join",spatial_lib::Q_INTERSECTION_JOIN},
    {"within_join",spatial_lib::Q_WITHIN_JOIN},
    {"find_relation",spatial_lib::Q_FIND_RELATION},
    {"relate",spatial_lib::Q_RELATE}};

static std::unordered_map<std::string, int> intermediateFilterStringToIntMap = {
    {"APRIL",spatial_lib::AT_APRIL},
    {"RI",spatial_lib::AT_RI},
    {"5CCA",spatial_lib::AT_5CCH},
    {"GEOS",spatial_lib::AT_GEOS},
    {"RA",spatial_lib::AT_RA}
    };

static std::string iFilterTypeIntToText(int val){
    switch(val) {
        case spatial_lib::AT_APRIL: return "APRIL";
        case spatial_lib::AT_5CCH: return "5C+CH";
        case spatial_lib::AT_RI: return "RI";
        case spatial_lib::AT_GEOS: return "GEOS";
        case spatial_lib::AT_RA: return "RA";
        case spatial_lib::AT_NONE: return "0";
    }
}

static std::string dataComboIntToText(int val){
    switch(val) {
        case spatial_lib::POLYGON_POLYGON: return "POLYGON_POLYGON";
        case spatial_lib::POLYGON_LINESTRING: return "POLYGON_LINESTRING";
    }
}

void printConfig() {
    std::cout << "---------- Configuration ----------" << std::endl;
    std::cout << "Configuration file path: \n\t" << g_config.dirPaths.configFilePath << std::endl;
    std::cout << "Query: " << std::endl;
    std::cout << "\ttype: " << queryTypeIntToText(g_config.queryData.type) << std::endl;
    std::cout << "\tR: " << g_config.queryData.R.path << " (" << dataTypeIntToText(g_config.queryData.R.dataType) <<  ")" << std::endl;
    if (g_config.queryData.numberOfDatasets == 2) {
        std::cout << "\tS: " << g_config.queryData.S.path << " (" << dataTypeIntToText(g_config.queryData.S.dataType) <<  ")" << std::endl;
    }
    std::cout << "Pipeline: " << std::endl;
    std::cout << "\tMBR Filter: " << g_config.pipeline.MBRFilterEnabled << std::endl;
    std::cout << "\tIntermediate Filter: " << iFilterTypeIntToText(g_config.pipeline.iFilterType) << std::endl;
    std::cout << "\tRefinement: " << g_config.pipeline.RefinementEnabled << std::endl;

    std::cout << "------------------------------------" << std::endl;
}

static bool verifyIntermediateFilterType(std::string queryType) {
    if (auto it = queryStringToIntMap.find(queryType) != queryStringToIntMap.end()) {
        return true;
    }
    return false;
}


static bool verifyQuery(QueryStatementT *queryStmt) {
    if (queryStmt->queryType == "") {
        log_err("No query type specified.");
        return false;
    }
    
    auto itqt = queryStringToIntMap.find(queryStmt->queryType);
    // verify type
    if (itqt == queryStringToIntMap.end()) {
        log_err("Failed when verifying query type.");
        // todo: print available query types strings
        return false;
    }
    // verify number of inputs datasets
    auto itnr = spatial_lib::queryTypeToNumberOfInputsMap.find(itqt->second);
    if (itnr == spatial_lib::queryTypeToNumberOfInputsMap.end()) {
        log_err("Missing query type in number of inputs map.");
        // todo: print relevant message
        return false;
    }
    if (itnr->second != queryStmt->datasetCount) {
        log_err("Invalid number of datasets for specified query type.");
        // todo: print relevant message
        return false;
    }

    return true;
}

bool verifyAndbuildQuery(QueryStatementT *queryStmt) {
    // verify
    if (!verifyQuery(queryStmt)) {
        log_err("Failed when verifying query config.");
        return false;
    }

    // build
    auto it = queryStringToIntMap.find(queryStmt->queryType);
    spatial_lib::QueryT query;
    query.type = (spatial_lib::QueryTypeE) it->second;
    query.numberOfDatasets = queryStmt->datasetCount;
    query.boundsSet = queryStmt->boundsSet;
    query.xMinGlobal = queryStmt->xMinGlobal;
    query.yMinGlobal = queryStmt->yMinGlobal;
    query.xMaxGlobal = queryStmt->xMaxGlobal;
    query.yMaxGlobal = queryStmt->yMaxGlobal;
    // printf("Global bounds: (%f,%f),(%f,%f)\n", query.xMinGlobal, query.yMinGlobal, query.xMaxGlobal, query.yMaxGlobal);
    
    // verify dataset paths
    if (!verifyDatasetPaths(queryStmt->datasetPathR) ){
        log_err("Failed when verifying dataset R path.");
        return false;
    }
    if (!verifyDatasetPaths(queryStmt->datasetPathS) ){
        log_err("Failed when verifying dataset S path.");
        return false;
    }
    // verify offset map paths
    if (!verifyDatasetPaths(queryStmt->offsetMapPathR) ){
        log_err("Failed when verifying offset map path of R.");
        return false;
    }
    if (!verifyDatasetPaths(queryStmt->offsetMapPathS) ){
        log_err("Failed when verifying offset map path of S.");
        return false;
    }

    // build dataset objects
    spatial_lib::DatasetT R,S;
    R.dataType = queryStmt->datatypeR;
    S.dataType = queryStmt->datatypeS;
    R.path = queryStmt->datasetPathR;
    S.path = queryStmt->datasetPathS;
    R.offsetMapPath = queryStmt->offsetMapPathR;
    S.offsetMapPath = queryStmt->offsetMapPathS;
    R.datasetName = getDatasetNameFromPath(R.path);
    S.datasetName = getDatasetNameFromPath(S.path);

    // add datasets to query
    query.R = R;
    query.S = S;

    // add query to global config
    g_config.queryData = query;

    // all is well
    return true;
}

static bool verifyPipeline(std::string intermediateFilterType) {
    auto it = intermediateFilterStringToIntMap.find(intermediateFilterType);
    if (it == intermediateFilterStringToIntMap.end()) {
        log_err("Invalid selection of intermediate filter type.");
        // todo: print availables
        return false;
    }
    return true;
}

bool verifyAndBuildPipeline(int mbrFilterEnabled, std::string intermediateFilterType, int refinementEnabled){
    if (intermediateFilterType != "") {
        // verify
        if (!verifyPipeline(intermediateFilterType)) {
            log_err("Failed when verifying pipeline config.");
            return false;
        }  
        // build
        auto it = intermediateFilterStringToIntMap.find(intermediateFilterType);
        g_config.pipeline.iFilterType = (spatial_lib::ApproximationTypeE) it->second;
    } else {
        // no filter
        g_config.pipeline.iFilterType = spatial_lib::AT_NONE;
    }

    g_config.pipeline.MBRFilterEnabled = mbrFilterEnabled;
    g_config.pipeline.RefinementEnabled = refinementEnabled;

    return true;
}

/**
 * creates APRIL for a dataset using the rasterizer lib
 * returns true if completed successsfully
*/
static bool createAPRIL(spatial_lib::DatasetT &dataset) {
    uint polygonCount;
    int vertexCount;
    double x,y;
    uint recID;
    // check dataset file path
    ifstream datasetFile(dataset.path, fstream::in | ios_base::binary);
    if(!datasetFile) {
        log_err("Cannot open dataset file.");
        return false;
    }
    // check approximation file paths
    ofstream foutALL(dataset.aprilConfig.ALL_intervals_path, fstream::out | ios_base::binary);
    if(!foutALL) {
        log_err("Cannot open output approximation file.");
        return false;
    }
    ofstream foutFULL(dataset.aprilConfig.FULL_intervals_path, fstream::out | ios_base::binary);
    if(!foutFULL) {
        log_err("Cannot open output approximation file.");
        return false;
    }
    
    //first read the total polygon count
    datasetFile.read((char*) &polygonCount, sizeof(int));
    // write it to the out files
    foutALL.write((char*)&polygonCount, sizeof(uint));
    foutFULL.write((char*)&polygonCount, sizeof(uint));
    // std::cout << "Polygon count: " << polygonCount << std::endl;
    //read polygons
    for(int j=0; j<polygonCount; j++){
        //read/write the polygon id
        datasetFile.read((char*) &recID, sizeof(uint)); 
        // printf("polygon %d, id: %d\n", j, recID);
        //read the vertex count
        datasetFile.read((char*) &vertexCount, sizeof(int));
        std::vector<spatial_lib::PointT> vertices;
        vertices.reserve(vertexCount);
        // read the points
        for(int i=0; i<vertexCount; i++){
            datasetFile.read((char*) &x, sizeof(double));
            datasetFile.read((char*) &y, sizeof(double));
            spatial_lib::PointT p;
            p.x = x;
            p.y = y;
            vertices.emplace_back(p);
        }
        // create a polygon
        rasterizerlib::polygon2d polygon = rasterizerlib::createPolygon(vertices);

        // generate the raster based on the configuration
        spatial_lib::AprilDataT *aprilData = rasterizerlib::generate(polygon, rasterizerlib::GT_APRIL);

        if (aprilData == NULL) {
            log_err("Rasterization failed.");
            printPolygon(polygon, recID);
            exit(-1);
        }
        
        // save on disk
        APRIL::saveAPRILonDisk(foutALL, foutFULL, recID, 0, aprilData);

        // free memory
        delete aprilData;
    }

    datasetFile.close();
    foutALL.close();
    foutFULL.close();

    return true;
}

static void initAPRIL() {
    // create if requested
    if (g_config.actions.createApproximations) {
        // init rasterizer lib
        rasterizerlib::init(g_config.queryData.xMinGlobal, g_config.queryData.yMinGlobal, g_config.queryData.xMaxGlobal, g_config.queryData.yMaxGlobal);

        // create APRIL for R
        g_timer = clock();
        log_task("Creating APRIL for R...");
        bool ret = createAPRIL(g_config.queryData.R);
        if (!ret) {
            log_err("Create APRIL failed for dataset R.");
            exit(-1);
        }
        success_text_with_time("Finished in ", g_timer);

        // create APRIL for S
        g_timer = clock();
        log_task("Creating APRIL for S...");
        ret = createAPRIL(g_config.queryData.S);
        if (!ret) {
            log_err("Create APRIL failed for dataset S.");
            exit(-1);
        }

        success_text_with_time("Finished in ", g_timer);
    }

    // load APRIL from disk
    g_timer = clock();
    APRIL::loadAPRILfromDisk(g_config.queryData.R);
    APRIL::loadAPRILfromDisk(g_config.queryData.S);
    success_text_with_time("Loaded APRIL", g_timer);

    // setup filter (same april config for both datasets)
    // todo: extend to allow different april configs for R and S
    APRIL::setupAPRILIntermediateFilter(&g_config.queryData);

    // set forwarding function for mbr -> intermediate filter
    two_layer::registerForwardingFunction(&APRIL::intermediateFilterEntrypoint);

    success_text("APRIL intermediate filter set.");
}

void initConfig() {
    log_task("Initializing...");  
    // MBR filter, if enabled
    if (g_config.pipeline.MBRFilterEnabled) {
        // load Datasets
        two_layer::loadBinaryDataset(g_config.queryData.R.path, true);
        two_layer::loadBinaryDataset(g_config.queryData.S.path, false);
        // check bounds
        if (!g_config.queryData.boundsSet) {
            two_layer::getDatasetGlobalBounds(g_config.queryData.xMinGlobal, g_config.queryData.yMinGlobal, g_config.queryData.xMaxGlobal, g_config.queryData.yMaxGlobal);
            g_config.queryData.boundsSet = true;
            success_text("Calculated global bounds based on dataset bounds.");
        }
        // initialize (partitioning and sorting)
        two_layer::init(g_config.queryData.xMinGlobal, g_config.queryData.yMinGlobal, g_config.queryData.xMaxGlobal, g_config.queryData.yMaxGlobal);
    }

    // initialize intermediate filter, if any
    switch (g_config.pipeline.iFilterType) {
        case spatial_lib::AT_APRIL:
            initAPRIL();
            break;
        case spatial_lib::AT_NONE:
            break;
        default:
            log_err("Unrecognized intermediate filter.");
            return;
    }

    // initialize refinement, if enabled
    if (g_config.pipeline.RefinementEnabled) {
        // setup refinement
        spatial_lib::setupRefinement(g_config.queryData);
       
        switch(g_config.pipeline.iFilterType) {
            case spatial_lib::AT_NONE:
                // set refinement after MBR filter
                two_layer::registerForwardingFunction(&spatial_lib::refinementEntrypoint);
                break;
            case spatial_lib::AT_APRIL:
                // register refinement function intermediate filter -> refinement
                APRIL::registerRefinementFunction(&spatial_lib::refineJoin);
                break;
            default:
                break;
        }
    }
    // reset query output
    spatial_lib::resetQueryOutput();

    success_text("Init Done!");
}