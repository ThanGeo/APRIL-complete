#include "include/def.h"
#include "include/config.h"
#include "include/parsing.h"

void printResults() {
    spatial_lib::g_queryOutput.queryResults += spatial_lib::g_queryOutput.trueHits;
    printf("Query Results - %d pairs.\n", spatial_lib::g_queryOutput.queryResults);
    printf("Filter Breakdown:\n");
    printf("- True Hits:\t\t %0.2f%\n", spatial_lib::g_queryOutput.trueHits / (double) spatial_lib::g_queryOutput.postMBRFilterCandidates * 100);
    printf("- True Negatives:\t %0.2f%\n", spatial_lib::g_queryOutput.trueNegatives / (double) spatial_lib::g_queryOutput.postMBRFilterCandidates * 100);
    printf("- Inconclusive:\t\t %0.2f%\n", spatial_lib::g_queryOutput.refinementCandidates / (double) spatial_lib::g_queryOutput.postMBRFilterCandidates * 100);
    printf("Total Time: %0.4f sec.\n", spatial_lib::g_queryOutput.totalTime);
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

    // set forwarding function for mbr -> intermediate filter
    two_layer::registerForwardingFunction(&APRIL::intermediateFilterEntrypoint);

    // reset query output
    spatial_lib::resetQueryOutput();

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