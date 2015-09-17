#include "experiments/realworld-queries/spam-bin.hpp"
#include "experiments/realworld-queries/spam-csv-cached-columnar.hpp"
#include "experiments/realworld-queries/spam-json-cached.hpp"
#include "experiments/realworld-queries/spam-bin-csv.hpp"
#include "experiments/realworld-queries/spam-bin-json.hpp"
#include "experiments/realworld-queries/spam-csv-json.hpp"
#include "experiments/realworld-queries/spam-bin-csv-json-v2.hpp"


int main()	{
	cout << "[ViDa] Execution - Entire Workload" << endl;
	map<string,dataset> datasetCatalog;
	symantecBinSchema(datasetCatalog);
	symantecCSVSchema(datasetCatalog);
	symantecCoreIDDatesSchema(datasetCatalog);


	cout << "SYMANTEC BIN 1" << endl;
	symantecBin1(datasetCatalog);
	cout << "SYMANTEC BIN 2" << endl;
	symantecBin2(datasetCatalog);
	cout << "SYMANTEC BIN 3" << endl;
	symantecBin3(datasetCatalog);
	cout << "SYMANTEC BIN 4" << endl;
	symantecBin4(datasetCatalog);
	cout << "SYMANTEC BIN 5" << endl;
	symantecBin5(datasetCatalog);
	cout << "SYMANTEC BIN 6" << endl;
	symantecBin6(datasetCatalog);
	cout << "**************" << endl;
	cout << "SYMANTEC BIN 7" << endl;
	symantecBin7(datasetCatalog);
	cout << "**************" << endl;
	cout << "SYMANTEC BIN 8" << endl;
	symantecBin8(datasetCatalog);
	cout << "**************" << endl;


	cout << "SYMANTEC CSV 1 (+Caching)" << endl;
	symantecCSV1Caching(datasetCatalog);
	cout << "**************" << endl;
	cout << "SYMANTEC CSV 2 (+Caching)" << endl;
	symantecCSV2Caching(datasetCatalog);
	cout << "**************" << endl;
	cout << "SYMANTEC CSV 3" << endl;
	symantecCSV3(datasetCatalog);
	cout << "**************" << endl;
	cout << "SYMANTEC CSV 4" << endl;
	symantecCSV4(datasetCatalog);
	cout << "**************" << endl;
	cout << "SYMANTEC CSV 5" << endl;
	symantecCSV5(datasetCatalog);
	cout << "**************" << endl;
	cout << "SYMANTEC CSV 6" << endl;
	symantecCSV6(datasetCatalog);
	cout << "**************" << endl;
	cout << "SYMANTEC CSV 7" << endl;
	symantecCSV7(datasetCatalog);
	cout << "**************" << endl;


	cout << "SYMANTEC JSON 1" << endl;
	symantecJSON1Caching(datasetCatalog);
	cout << "SYMANTEC JSON 2" << endl;
	symantecJSON2(datasetCatalog);
	cout << "SYMANTEC JSON 3" << endl;
	symantecJSON3(datasetCatalog);
	cout << "SYMANTEC JSON 4" << endl;
	symantecJSON4(datasetCatalog);
	cout << "SYMANTEC JSON 5" << endl;
	symantecJSON5(datasetCatalog);
	cout << "SYMANTEC JSON 6" << endl;
	symantecJSON6(datasetCatalog);
	cout << "SYMANTEC JSON 7" << endl;
	symantecJSON7(datasetCatalog);
	cout << "SYMANTEC JSON 9" << endl;
	symantecJSON9(datasetCatalog);
	cout << "SYMANTEC JSON 10" << endl;
	symantecJSON10(datasetCatalog);
	cout << "SYMANTEC JSON 11" << endl;
	symantecJSON11(datasetCatalog);


	cout << "SYMANTEC BIN-CSV 1" << endl;
	symantecBinCSV1(datasetCatalog);
	cout << "SYMANTEC BIN-CSV 2" << endl;
	symantecBinCSV2(datasetCatalog);
	cout << "SYMANTEC BIN-CSV 3" << endl;
	symantecBinCSV3(datasetCatalog);
	cout << "SYMANTEC BIN-CSV 4" << endl;
	symantecBinCSV4(datasetCatalog);
	cout << "SYMANTEC BIN-CSV 5" << endl;
	symantecBinCSV5(datasetCatalog);


	cout << "SYMANTEC BIN-JSON 1" << endl;
	symantecBinJSON1(datasetCatalog);
	cout << "SYMANTEC BIN-JSON 2" << endl;
	symantecBinJSON2(datasetCatalog);
	cout << "SYMANTEC BIN-JSON 3" << endl;
	symantecBinJSON3(datasetCatalog);
	cout << "SYMANTEC BIN-JSON 4" << endl;
	symantecBinJSON4(datasetCatalog);
	cout << "SYMANTEC BIN-JSON 5" << endl;
	symantecBinJSON5(datasetCatalog);

	cout << "SYMANTEC CSV-JSON 1" << endl;
	symantecCSVJSON1(datasetCatalog);
	cout << "SYMANTEC CSV-JSON 2" << endl;
	symantecCSVJSON2(datasetCatalog);
	cout << "SYMANTEC CSV-JSON 3" << endl;
	symantecCSVJSON3(datasetCatalog);
	cout << "SYMANTEC CSV-JSON 4" << endl;
	symantecCSVJSON4(datasetCatalog);
	cout << "SYMANTEC CSV-JSON 5" << endl;
	symantecCSVJSON5(datasetCatalog);


	cout << "SYMANTEC BIN-CSV-JSON 1" << endl;
	symantecBinCSVJSON1(datasetCatalog);
	cout << "SYMANTEC BIN-CSV-JSON 2" << endl;
	symantecBinCSVJSON2(datasetCatalog);
	cout << "SYMANTEC BIN-CSV-JSON 3" << endl;
	symantecBinCSVJSON3(datasetCatalog);
	cout << "SYMANTEC BIN-CSV-JSON 4" << endl;
	symantecBinCSVJSON4(datasetCatalog);
	cout << "SYMANTEC BIN-CSV-JSON 5" << endl;
	symantecBinCSVJSON5(datasetCatalog);
	cout << "SYMANTEC BIN-CSV-JSON 6" << endl;
	symantecBinCSVJSON6(datasetCatalog);
	cout << "SYMANTEC BIN-CSV-JSON 7" << endl;
	symantecBinCSVJSON7(datasetCatalog);
	cout << "SYMANTEC BIN-CSV-JSON 8" << endl;
	symantecBinCSVJSON8(datasetCatalog);
	cout << "SYMANTEC BIN-CSV-JSON 9" << endl;
	symantecBinCSVJSON9(datasetCatalog);
	cout << "SYMANTEC BIN-CSV-JSON 10" << endl;
	symantecBinCSVJSON10(datasetCatalog);
}
