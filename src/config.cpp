#include "config.h"

/**
 * Global configuration variable
*/
ConfigT g_config;

static std::unordered_map<std::string, spatial_lib::QueryTypeE> queryStringToIntMap = {
    {"range",spatial_lib::RANGE},
    {"intersect",spatial_lib::Q_INTERSECT},
    {"inside",spatial_lib::Q_INSIDE},
    {"disjoint",spatial_lib::Q_DISJOINT},
    {"equal",spatial_lib::Q_EQUAL},
    {"meet",spatial_lib::Q_MEET},
    {"contains",spatial_lib::Q_CONTAINS},
    {"covered_by",spatial_lib::Q_COVERED_BY},
    {"covers",spatial_lib::Q_COVERS},
    {"find_relation",spatial_lib::Q_FIND_RELATION},
    };

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
    spatial_lib::DataspaceInfoT dataspaceInfo;
    dataspaceInfo.xMinGlobal = queryStmt->xMinGlobal;
    dataspaceInfo.yMinGlobal = queryStmt->yMinGlobal;
    dataspaceInfo.xMaxGlobal = queryStmt->xMaxGlobal;
    dataspaceInfo.yMaxGlobal = queryStmt->yMaxGlobal;
    dataspaceInfo.xExtent = dataspaceInfo.xMaxGlobal - dataspaceInfo.xMinGlobal;
    dataspaceInfo.yExtent = dataspaceInfo.yMaxGlobal - dataspaceInfo.yMinGlobal;
    query.dataspaceInfo = dataspaceInfo;
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
    R.dataspaceInfo = query.dataspaceInfo;
    S.dataspaceInfo = query.dataspaceInfo;
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
        g_config.pipeline.iFilterType = (spatial_lib::IntermediateFilterTypeE) it->second;
    } else {
        // no filter
        g_config.pipeline.iFilterType = spatial_lib::IF_NONE;
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
    //read polygons
    for(int j=0; j<polygonCount; j++){
        //read/write the polygon id
        datasetFile.read((char*) &recID, sizeof(uint)); 
        //read the vertex count
        datasetFile.read((char*) &vertexCount, sizeof(int));
        std::vector<spatial_lib::PointT> vertices;
        vertices.reserve(vertexCount);
        // for MBR
        double xMin = std::numeric_limits<int>::max();
		double yMin = std::numeric_limits<int>::max();
		double xMax = -std::numeric_limits<int>::max();
		double yMax = -std::numeric_limits<int>::max();
        // read the points
        for(int i=0; i<vertexCount; i++){
            datasetFile.read((char*) &x, sizeof(double));
            datasetFile.read((char*) &y, sizeof(double));
            spatial_lib::PointT p;
            p.x = x;
            p.y = y;
            vertices.emplace_back(p);
            // store MBR
            xMin = min(xMin, x);
            yMin = min(yMin, y);
            xMax = max(xMax, x);
            yMax = max(yMax, y);
        }
        // create a polygon
        rasterizerlib::polygon2d polygon = rasterizerlib::createPolygon(vertices);

        // get sections for the polygon based on how many have been set (stored in dataset object) 
        std::vector<spatial_lib::SectionT*> sections = spatial_lib::getSectionsOfMBR(dataset, xMin, yMin, xMax, yMax);

        // for each section
        for (auto &section : sections) {      
            // printf("rec %u at section %u\n", recID, section->sectionID);
            // printf("Setion interest area: (%f,%f),(%f,%f) \n", section->interestxMin, section->interestyMin, section->interestxMax, section->interestyMax);  
            // printf("Setion raster area: (%f,%f),(%f,%f) \n", section->rasterxMin, section->rasteryMin, section->rasterxMax, section->rasteryMax);   
            // set rasterizer lib dataspace to the section's data space
            rasterizerlib::setDataspace(section->rasterxMin, section->rasteryMin, section->rasterxMax, section->rasteryMax);

            // generate the raster based on the configuration
            spatial_lib::AprilDataT *aprilData = rasterizerlib::generate(polygon, rasterizerlib::GT_APRIL);

            if (aprilData == NULL) {
                log_err("Rasterization failed.");
                printPolygon(polygon, recID);
                exit(-1);
            }
            
            // save on disk
            APRIL::saveAPRILonDisk(foutALL, foutFULL, recID, section->sectionID, aprilData);
            // free memory
            delete aprilData;
        }
    }

    datasetFile.close();
    foutALL.close();
    foutFULL.close();

    return true;
}


static void initializeSections(spatial_lib::QueryT &queryData, spatial_lib::DatasetT &dataset) {
    // partitions/sections per dimension
    uint partitionsNum = dataset.aprilConfig.partitions;
    for (int i=0; i<partitionsNum; i++) {
        for (int j=0; j<partitionsNum; j++) {
            spatial_lib::SectionT section;
            section.i = i;
            section.j = j;
            section.sectionID = spatial_lib::getSectionIDFromIdxs(i, j, partitionsNum);
            section.interestxMin = queryData.dataspaceInfo.xMinGlobal + ((i) * (queryData.dataspaceInfo.xExtent / (double) partitionsNum));
            section.interestxMax = queryData.dataspaceInfo.xMinGlobal + ((i+1) * (queryData.dataspaceInfo.xExtent / (double) partitionsNum));
            section.interestyMin = queryData.dataspaceInfo.yMinGlobal + ((j) * (queryData.dataspaceInfo.yExtent / (double) partitionsNum));
            section.interestyMax = queryData.dataspaceInfo.yMinGlobal + ((j+1) * (queryData.dataspaceInfo.yExtent / (double) partitionsNum));
            section.rasterxMin = section.interestxMin;
            section.rasteryMin = section.interestyMin;
            section.rasterxMax = section.interestxMax;
            section.rasteryMax = section.interestyMax;
            section.objectCount = 0;
            // store to dataset
            dataset.sectionMap.insert(std::make_pair(section.sectionID, section));
            // printf("Initialized section interest area: (%f,%f),(%f,%f) \n", section.interestxMin, section.interestyMin, section.interestxMax, section.interestyMax);  
            
        }
    }
}

static void adjustSections(spatial_lib::DatasetT &dataset, double xMin, double yMin, double xMax, double yMax) {
    std::vector<spatial_lib::SectionT*> sections = spatial_lib::getSectionsOfMBR(dataset, xMin, yMin,xMax, yMax);
    
    for (auto &section : sections) {
        if(xMin < section->interestxMin){
            section->rasterxMin = std::min(section->rasterxMin, xMin);
        }
        if(yMin < section->interestyMin){
            section->rasteryMin = std::min(section->rasteryMin, yMin);
        }
        if(xMax > section->interestxMax){
            section->rasterxMax = std::max(section->rasterxMax, xMax);
        }
        if(yMax > section->interestyMax){
            section->rasteryMax = std::max(section->rasteryMax, yMax);
        }

        // printf("Adjusted section raster area from: (%f,%f),(%f,%f) \n", section->interestxMin, section->interestyMin, section->interestxMax, section->interestyMax);  
        // printf("To: (%f,%f),(%f,%f) \n", section->rasterxMin, section->rasteryMin, section->rasterxMax, section->rasteryMax);  

    }
    
}

/**
 * @brief sets the raster areas of the sections of both datasets to match
 * 
 * @param datasetR 
 * @param datasetS 
 */
static void matchSections(spatial_lib::DatasetT &datasetR, spatial_lib::DatasetT &datasetS) {    
    for(int secID = 0; secID < datasetR.sectionMap.size(); secID++) {
        auto itR = datasetR.sectionMap.find(secID);
        auto itS = datasetS.sectionMap.find(secID);

        double xMin = std::min(itR->second.rasterxMin,itS->second.rasterxMin);
        itR->second.rasterxMin = xMin;
        itS->second.rasterxMin = xMin;
        double yMin = std::min(itR->second.rasteryMin,itS->second.rasteryMin);
        itR->second.rasteryMin = yMin;
        itS->second.rasteryMin = yMin;
        double xMax = std::max(itR->second.rasterxMax,itS->second.rasterxMax);
        itR->second.rasterxMax = xMax;
        itS->second.rasterxMax = xMax;
        double yMax = std::max(itR->second.rasteryMax,itS->second.rasteryMax);
        itR->second.rasteryMax = yMax;
        itS->second.rasteryMax = yMax;
    }
}

static void initAPRIL() {

    // create if requested
    if (g_config.actions.createApproximations) {
        // init rasterizer lib
        rasterizerlib::init(g_config.queryData.dataspaceInfo.xMinGlobal, g_config.queryData.dataspaceInfo.yMinGlobal, g_config.queryData.dataspaceInfo.xMaxGlobal, g_config.queryData.dataspaceInfo.yMaxGlobal);
        // printf("Initialized rasterizer to (%f,%f),(%f,%f)\n",g_config.queryData.dataspaceInfo.xMinGlobal, g_config.queryData.dataspaceInfo.yMinGlobal, g_config.queryData.dataspaceInfo.xMaxGlobal, g_config.queryData.dataspaceInfo.yMaxGlobal);
        // create APRIL for R
        clock_t timer = clock();
        log_task("Creating APRIL for R...");
        bool ret = createAPRIL(g_config.queryData.R);
        if (!ret) {
            log_err("Create APRIL failed for dataset R.");
            exit(-1);
        }
        success_text_with_time("Finished in ", spatial_lib::time::getElapsedTime(timer));

        // create APRIL for S
        timer = clock();
        log_task("Creating APRIL for S...");
        ret = createAPRIL(g_config.queryData.S);
        if (!ret) {
            log_err("Create APRIL failed for dataset S.");
            exit(-1);
        }

        success_text_with_time("Finished in ", spatial_lib::time::getElapsedTime(timer));

    }

    // verify paths
    if (!verifyFileExists(g_config.queryData.R.aprilConfig.ALL_intervals_path)) {
        log_err_w_text("Error verifying APRIL filepath",g_config.queryData.R.aprilConfig.ALL_intervals_path);
        exit(-1);
    }
    if (!verifyFileExists(g_config.queryData.R.aprilConfig.FULL_intervals_path)) {
        log_err_w_text("Error verifying APRIL filepath",g_config.queryData.R.aprilConfig.FULL_intervals_path);
        exit(-1);
    }
    if (!verifyFileExists(g_config.queryData.S.aprilConfig.ALL_intervals_path)) {
        log_err_w_text("Error verifying APRIL filepath",g_config.queryData.S.aprilConfig.ALL_intervals_path);
        exit(-1);
    }
    if (!verifyFileExists(g_config.queryData.S.aprilConfig.FULL_intervals_path)) {
        log_err_w_text("Error verifying APRIL filepath",g_config.queryData.S.aprilConfig.FULL_intervals_path);
        exit(-1);
    }

    // load APRIL from disk
    clock_t timer = clock();
    APRIL::loadAPRILfromDisk(g_config.queryData.R);
    APRIL::loadAPRILfromDisk(g_config.queryData.S);
    success_text_with_time("Loaded APRIL", spatial_lib::time::getElapsedTime(timer));

    // setup filter (same april config for both datasets)
    // todo: extend to allow different april configs for R and S
    APRIL::setupAPRILIntermediateFilter(&g_config.queryData);

    success_text("APRIL intermediate filter set.");
}

static void loadDataset(std::string &filepath, bool left) {
    int polygonCount;
    int vertexCount;
    int recID;
    Coord x,y;

    ifstream file( filepath, fstream::in | ios_base::binary);
    if(!file)
    {
        cerr << "Cannot open the File : " << filepath << endl;
        exit(1);
    }

    //first read the total polygon count
    file.read((char*) &polygonCount, sizeof(int));
    //read polygons
    for(int j=0; j<polygonCount; j++){

        Coord minXmbr, minYmbr, maxXmbr, maxYmbr;
        minXmbr = std::numeric_limits<Coord>::max();
        maxXmbr = -std::numeric_limits<Coord>::max();
        minYmbr = std::numeric_limits<Coord>::max();
        maxYmbr = -std::numeric_limits<Coord>::max();

        //read/write the polygon id
        file.read((char*) &recID, sizeof(int)); 

        //read the vertex count
        file.read((char*) &vertexCount, sizeof(int));

        for(int i=0; i<vertexCount; i++){
            file.read((char*) &x, sizeof(double));
            file.read((char*) &y, sizeof(double));

            minXmbr = std::min(minXmbr, x);
            maxXmbr = std::max(maxXmbr, x);
            minYmbr = std::min(minYmbr, y);
            maxYmbr = std::max(maxYmbr, y);
            // if (recID == 59241 || recID == 1012118) {
            //     printf("(%f,%f),",x,y);
            // }
        }

        // if (recID == 59241 || recID == 1012118) {
        //     printf("\n\n");    
        // }
        // add to relation for the MBR filter
        two_layer::addObjectToDataset(left, recID, minXmbr, minYmbr, maxXmbr, maxYmbr);

        // if APRIL is set
        if(g_config.pipeline.iFilterType == spatial_lib::IF_APRIL) {
            // adjust sections
            if (left) {
                adjustSections(g_config.queryData.R, minXmbr, minYmbr, maxXmbr, maxYmbr);
            } else {
                adjustSections(g_config.queryData.S, minXmbr, minYmbr, maxXmbr, maxYmbr);
            }
        }
    }

    file.close();
}

static void setupDataspace(std::string &Rfilepath, std::string &Sfilepath) {
    // if APRIL is set
    if(g_config.pipeline.iFilterType == spatial_lib::IF_APRIL) {
        // create sections
        initializeSections(g_config.queryData, g_config.queryData.R);
        initializeSections(g_config.queryData, g_config.queryData.S);
    }
    // load datasets
    loadDataset(Rfilepath, true);
    loadDataset(Sfilepath, false);

    // if APRIL is set
    if(g_config.pipeline.iFilterType == spatial_lib::IF_APRIL) {
        // match sections built by R and S
        matchSections(g_config.queryData.R, g_config.queryData.S);
    }
    
    success_text("Setup data space.");
}

void initConfig() {
    log_task("Initializing..."); 
    // MBR filter, if enabled
    if (g_config.pipeline.MBRFilterEnabled) {
        // set up the dataspace
        setupDataspace(g_config.queryData.R.path, g_config.queryData.S.path);
        // check bounds
        if (!g_config.queryData.boundsSet) {
            two_layer::getDatasetGlobalBounds(g_config.queryData.dataspaceInfo.xMinGlobal, g_config.queryData.dataspaceInfo.yMinGlobal, g_config.queryData.dataspaceInfo.xMaxGlobal, g_config.queryData.dataspaceInfo.yMaxGlobal);
            g_config.queryData.boundsSet = true;
            success_text("Calculated global bounds based on dataset bounds.");
        }
        // initialize (partitioning and sorting)
        // two_layer::init(g_config.pipeline.iFilterType, g_config.queryData.xMinGlobal, g_config.queryData.yMinGlobal, g_config.queryData.xMaxGlobal, g_config.queryData.yMaxGlobal);
        two_layer::initTwoLayer(1000);
    } else {
        log_err("Error: An MBR filter is mandatory but was not set.");
        exit(-1);
    }

    // initialize intermediate filter, if any
    switch (g_config.pipeline.iFilterType) {
        case spatial_lib::IF_APRIL:
            // init APRIL
            initAPRIL();
            // set as next stage after two layer MBR filter
            two_layer::setNextStage(spatial_lib::IF_APRIL);
            break;
        case spatial_lib::IF_NONE:
            // set refinement as next stage
            two_layer::setNextStage(spatial_lib::IF_NONE);
            break;
        default:
            log_err("Unrecognized intermediate filter.");
            return;
    }

    // initialize refinement, if enabled
    if (g_config.pipeline.RefinementEnabled) {
        // setup refinement
        spatial_lib::setupRefinement(g_config.queryData);
    }
    // reset query output
    spatial_lib::resetQueryOutput();

    success_text("Init Done!");
}