#include "include/def.h"
#include "include/config.h"
#include "include/parsing.h"

void printResults() {
    spatial_lib::g_queryOutput.queryResults += spatial_lib::g_queryOutput.trueHits;
    printf("Total Time     :\t %0.4f sec.\n", spatial_lib::g_queryOutput.totalTime);
    printf("Post MBR filter:\t %d pairs.\n", spatial_lib::g_queryOutput.postMBRFilterCandidates);
    printf("Refinement Candidates:\t %d pairs (%0.2f%).\n", spatial_lib::g_queryOutput.refinementCandidates, spatial_lib::g_queryOutput.refinementCandidates / (double) spatial_lib::g_queryOutput.postMBRFilterCandidates * 100);
    switch (g_config.queryData.type) {
        case spatial_lib::Q_INTERSECTION_JOIN:
        case spatial_lib::Q_WITHIN_JOIN:
            printf("Filter Breakdown:\n");
            printf("- True Hits:\t\t %0.2f%\n", spatial_lib::g_queryOutput.trueHits / (double) spatial_lib::g_queryOutput.postMBRFilterCandidates * 100);
            printf("- True Negatives:\t %0.2f%\n", spatial_lib::g_queryOutput.trueNegatives / (double) spatial_lib::g_queryOutput.postMBRFilterCandidates * 100);
            printf("- Inconclusive:\t\t %0.2f%\n", spatial_lib::g_queryOutput.refinementCandidates / (double) spatial_lib::g_queryOutput.postMBRFilterCandidates * 100);
            printf("\n");
            printf("Query Results  :\t %d pairs.\n", spatial_lib::g_queryOutput.queryResults);
            break;
        case spatial_lib::Q_FIND_RELATION:
            printf("Query Results\n");
            printf("- Disjoint  :\t\t %d\n", spatial_lib::g_queryOutput.topologyRelationsResultMap[spatial_lib::TR_DISJOINT]);
            printf("- Intersects:\t\t %d\n", spatial_lib::g_queryOutput.topologyRelationsResultMap[spatial_lib::TR_INTERSECTS]);
            printf("- Meet      :\t\t %d\n", spatial_lib::g_queryOutput.topologyRelationsResultMap[spatial_lib::TR_MEET]);
            printf("- Contains  :\t\t %d\n", spatial_lib::g_queryOutput.topologyRelationsResultMap[spatial_lib::TR_CONTAINS]);
            printf("- Inside    :\t\t %d\n", spatial_lib::g_queryOutput.topologyRelationsResultMap[spatial_lib::TR_INSIDE]);
            printf("- Covers    :\t\t %d\n", spatial_lib::g_queryOutput.topologyRelationsResultMap[spatial_lib::TR_COVERS]);
            printf("- Covered by:\t\t %d\n", spatial_lib::g_queryOutput.topologyRelationsResultMap[spatial_lib::TR_COVERED_BY]);
            printf("- Equal     :\t\t %d\n", spatial_lib::g_queryOutput.topologyRelationsResultMap[spatial_lib::TR_EQUAL]);

            break;
    }
}

void freeMemory() {
    for (auto &it : g_config.queryData.R.aprilData) {
        delete it.second;
    }
    g_config.queryData.R.aprilData.clear();
    for (auto &it : g_config.queryData.R.aprilData) {
        delete it.second;
    }
    g_config.queryData.S.aprilData.clear();
}

int main(int argc, char *argv[]) {

    // args and configuration file
    parseArgumentsAndConfigurationFile(argc, argv);
    // print config
    // printConfig();

    // init
    initConfig();

    // begin evaluation
    g_timer = clock();
    two_layer::evaluateTwoLayer();
    spatial_lib::g_queryOutput.totalTime = (clock() - g_timer) / (double) CLOCKS_PER_SEC;

    // print results
    printResults();

    // free any memory
    freeMemory();

    success_text("Finished successfuly");
    return 0;
}