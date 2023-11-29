#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <fstream>
#include <map>
#include <math.h>
#include <fcntl.h>
#include <vector>
#include <iterator>

#include "431project.h"
#include <string.h>
#include <bitset>

using namespace std;

/*
 * Enter your PSU IDs here to select the appropriate scanning order.
 */
#define PSU_ID_SUM (970487013+970409549)
// Samarth Tehri and Aly Ghallab

/*
 * Some global variables to track heuristic progress.
 * 
 * Feel free to create more global variables to track progress of your
 * heuristic.
 */
unsigned int currentlyExploringDim = 0;
bool currentDimDone = false;
bool isDSEComplete = false;

// The values of the index in the array of indexes (the 18dimensional array)
int width[4] = {1,2,4,8};
//# In-order core of Out-of-Order core
//scheduling=( "-issue:inorder true -issue:wrongpath false" "-issue:inorder false -issue:wrongpath true" )
int l1block[4] = {8,16,32,64};
int dl1sets[9] = {32,64,128,256,512,1024,2048,4096,8192};
int dl1assoc[3] = {1,2,4};
int il1sets[9] = {32,64,128,256,512,1024,2048,4096,8192};
int il1assoc[3] = {1,2,4};
int ul2sets[10] = {256,512,1024,2048,4096,8192,16384,32768,65536,131072};
int ul2block[4] = {16,32,64,128};
int ul2assoc[5] = {1,2,4,8,16};
//char replacepolicy[3] = {"l","f","r"};
int fpwidth[4] = {1,2,4,8};

//# Choice of branch predictor
//branchsettings=("-bpred nottaken" \
//               "-bpred bimod -bpred:bimod 2048" \
//                "-bpred 2lev -bpred:2lev 1 1024 8 0" \
//                "-bpred 2lev -bpred:2lev 4 256 8 0" \
//                "-bpred comb -bpred:comb 1024")

int ras[4] = {1,2,4,8};
//int btb = ("128 16" "256 8" "512 4" "1024 2" "2048 1")
int dl1lat[10] = {1,2,3,4,5,6,7,8,9,10};
int il1lat[10] = {1,2,3,4,5,6,7,8,9,10};
int ul2lat[10] = {5,6,7,8,9,10,11,12,13,14};

/*
 * Given a half-baked configuration containing cache properties, generate
 * latency parameters in configuration string. You will need information about
 * how different cache paramters affect access latency.
 * 
 * Returns a string similar to "1 1 1"
 */
std::string generateCacheLatencyParams(string halfBackedConfig) {

	string latencySettings = "";

	//
	//YOUR CODE BEGINS HERE
	//

	// Extract the valid related data drom halfbacked config

	int L1block = extractConfigPararm(halfBackedConfig,0);
	int Dl1sets = extractConfigPararm(halfBackedConfig,1);
	int Dl1assoc = extractConfigPararm(halfBackedConfig,2);
	int Il1sets = extractConfigPararm(halfBackedConfig,3);
	int Il1assoc = extractConfigPararm(halfBackedConfig,4);
	int Ul2sets = extractConfigPararm(halfBackedConfig,5);
	int Ul2block = extractConfigPararm(halfBackedConfig,6);
	int Ul2assoc = extractConfigPararm(halfBackedConfig,7);

	//Default value setup
	int Dl1lat = 1;
	int Il1lat = 1;
	int Ul2lat = 1;

	// Calculate block size total for the caches
	int Il1 = l1block[L1block] * il1sets[Il1sets] * il1assoc[Il1assoc];
	int Dl1 = l1block[L1block] * dl1sets[Dl1sets] * dl1assoc[Dl1assoc]; //assuming same I1block size
	int Ul2 = ul2block[Ul2block] * ul2sets[Ul2sets] * ul2assoc[Ul2assoc];

	// Fast simple switch case, related to 8.3 in pdf: for related cache latency
	switch(Il1) {
		case 2048: Il1lat = 1;
		break;
		case 4096: Il1lat = 2;
		break;
		case 8192: Il1lat = 3;
		break;
		case 16384: Il1lat = 4;
		break;
		case 32768: Il1lat = 5;
		break;
		case 65536: Il1lat = 6;
		break;
		default: Il1lat = 1;
	}
	// Calculation of latency with respect to association 1/2/4 way etc.
	Il1lat += int(il1assoc[Il1assoc] / 2);

	// Similar to above extraction.
	switch(Dl1) {
		case 2048: Dl1lat = 1;
		break;
		case 4096: Dl1lat = 2;
		break;
		case 8192: Dl1lat = 3;
		break;
		case 16384: Dl1lat = 4;
		break;
		case 32768: Dl1lat = 5;
		break;
		case 65536: Dl1lat = 6;
		break;
		default: Dl1lat = 1;
	}
	Dl1lat += int(dl1assoc[Dl1assoc] / 2);

	switch(Ul2) {
		case 32768: Ul2lat = 5;
		break;
		case 65536: Ul2lat = 6;
		break;
		case 131072: Ul2lat = 7;
		break;
		case 262144: Ul2lat = 8;
		break;
		case 524288: Ul2lat = 9;
		break;
		case 1048576: Ul2lat = 10;
		break;
		default: Ul2lat = 5;
	}
	Ul2lat += int(ul2assoc[Ul2assoc] / 2);

	// index manipulation, to fix the key-value sift
	Dl1lat -= 1;
	Il1lat -= 1;
	Ul2lat -= 5;

	// This is a dumb implementation.
	//latencySettings = "1 1 1";

	
	// String manipulation typecasting. took 2 hours
	// Injects the int to a string temp, which is concatenated with latencysettings string.
	char temp[5];
	sprintf(temp, "%d", Dl1lat);
	latencySettings += std::string(temp);
	latencySettings += " ";
	sprintf(temp, "%d", Il1lat);
	latencySettings += std::string(temp);
	latencySettings += " ";
	sprintf(temp, "%d", Ul2lat);
	latencySettings += std::string(temp);

	// Segfaulted code, due to wrong typecasting errors.
	/*
	latencySettings.append((char *)Dl1lat);
	latencySettings.append(" ");
	latencySettings.append((char *)Il1lat);
	latencySettings.append(" ");
	latencySettings.append((char *)Ul2lat);
	latencySettings.append('\0');
	latencySettings = latencySettings.c_str();
	/*
	latencySettings = (char) Dl1lat; 
	latencySettings += " ";
	latencySettings += (char) Il1lat;
	latencySettings += " ";
	latencySettings += (char) Ul2lat;

	/*
	*/
	//
	//YOUR CODE ENDS HERE
	//

	return latencySettings;
}

/*
 * Returns 1 if configuration is valid, else 0
 */
int validateConfiguration(std::string configuration) {

	// FIXME - YOUR CODE HERE

	// The below is a necessary, but insufficient condition for validating a
	// configuration.

	// get all parameters from the configuration code.
	int Width = extractConfigPararm(configuration,0);
	int Scheduling = extractConfigPararm(configuration,1);
	int L1block = extractConfigPararm(configuration,2);
	int Dl1sets = extractConfigPararm(configuration,3);
	int Dl1assoc = extractConfigPararm(configuration,4);
	int Il1sets = extractConfigPararm(configuration,5);
	int Il1assoc = extractConfigPararm(configuration,6);
	int Ul2sets = extractConfigPararm(configuration,7);
	int Ul2block = extractConfigPararm(configuration,8);
	int Ul2assoc = extractConfigPararm(configuration,9);
	int Replacepolicy = extractConfigPararm(configuration,10);
	int Fpwidth = extractConfigPararm(configuration,11);
	int Breanchsettings = extractConfigPararm(configuration,12);
	int Ras = extractConfigPararm(configuration,13);
	int Btb = extractConfigPararm(configuration,14);
	int Dl1lat = extractConfigPararm(configuration,15);
	int Il1lat = extractConfigPararm(configuration,16);
	int Ul2lat = extractConfigPararm(configuration,17);

	// Gets the appropriate Ifq from the width parameter, in calculation with l1block cache
	int Ifq = width[Width] * 8;

	// cutoff case
	if (l1block[L1block] < Ifq) {
		return 0;
	}

	//size calculation. Used index to access arrays to calculate size accurately.
	int Il1 = l1block[L1block] * il1sets[Il1sets] * il1assoc[Il1assoc];
	int Dl1 = l1block[L1block] * dl1sets[Dl1sets] * dl1assoc[Dl1assoc]; //assuming same I1block size
	int Ul2 = ul2block[Ul2block] * ul2sets[Ul2sets] * ul2assoc[Ul2assoc];


	// Test case for elimination. via return 0
	if (Il1 < 2048 || Il1 > 65536 || Dl1 < 2048 || Dl1 > 65536 || Ul2 < 32768 || Ul2 > 1048576) {
		return 0;
	}
	if (Dl1!=Il1 || (Ul2 < 2*(Il1+Dl1)) || ul2block[Ul2block] < 2* l1block[L1block] || ul2block[Ul2block] > 131072) {
		return 0;
	}
	return isNumDimConfiguration(configuration);
}

/*
 * Given the current best known configuration, the current configuration,
 * and the globally visible map of all previously investigated configurations,
 * suggest a previously unexplored design point. You will only be allowed to
 * investigate 1000 design points in a particular run, so choose wisely.
 *
 * In the current implementation, we start from the leftmost dimension and
 * explore all possible options for this dimension and then go to the next
 * dimension until the rightmost dimension.
 */
std::string generateNextConfigurationProposal(std::string currentconfiguration,
		std::string bestEXECconfiguration, std::string bestEDPconfiguration,
		int optimizeforEXEC, int optimizeforEDP) {

	//
	// Some interesting variables in 431project.h include:
	//
	// 1. GLOB_dimensioncardinality 
	// const unsigned int GLOB_dimensioncardinality[NUM_DIMS] = { 4, 2, 4, 9, 3, 9, 3, 10, 4, 5, 3, 4, 5, 4, 5, 10, 10, 10 };
	// 2. GLOB_baseline
	// const std::string GLOB_baseline = "0 0 0 5 0 5 0 2 2 2 0 1 0 1 2 2 2 5";
	// 3. NUM_DIMS
	// const int NUM_DIMS = 18;
	// 4. NUM_DIMS_DEPENDENT
	// const int NUM_DIMS_DEPENDENT = 3;
	// 5. GLOB_seen_configurations
	// extern std::map<std::string, unsigned int> GLOB_seen_configurations;

	std::string nextconfiguration = currentconfiguration;
	// Continue if proposed configuration is invalid or has been seen/checked before.

	// index hardcode, from the project1 instruction order: table 1 exporation order no.2
	// 1st: BP, 2nd: Core, 3rd: Cache, 4th: FPU
	// therefore by index ordering, 
	int order[15] = {12,13,14,0,1,2,3,4,5,6,7,8,9,10,11};
	int burnout = 0; //temp burnout flag to notify infinite loop case - when it has reached the best possible value.

	// Loop to test simplescalar validation.
	while (!validateConfiguration(nextconfiguration) ||
		GLOB_seen_configurations[nextconfiguration]) {

		// Check if DSE has been completed before and return current
		// configuration.
		if(isDSEComplete) {
			// Instead of returning the current configuration, due to custom start and end, loop it.
			currentlyExploringDim = 0;
			isDSEComplete = false; 
			//^ reset for pseduo infinite loop, best case optimization
		}

		// String stream
		std::stringstream ss;

		string bestConfig;
		if (optimizeforEXEC == 1)
			bestConfig = bestEXECconfiguration;

		if (optimizeforEDP == 1)
			bestConfig = bestEDPconfiguration;


		// Temp string placeholder
		std::string temp = bestConfig; //std:string because thats whats used here
		// Fill in the dimensions already-scanned with the already-selected best value
		for (int dim = 0; dim < currentlyExploringDim; ++dim) {
			temp[order[dim]*2] = bestConfig[order[dim]*2]; //array index x2 manipulation due to spaces. and OurHeuristic method.
		}

		// Handling for currently exploring dimension. This is a very dumb
		// implementation.------------------------------------------------------------------------------------------------------
		int nextValue = extractConfigPararm(nextconfiguration,order[currentlyExploringDim]) + 1; //from the certain part of the code

		if (nextValue >= GLOB_dimensioncardinality[order[currentlyExploringDim]]) { //inject to certain pos from order array OurHeuristic
			nextValue = GLOB_dimensioncardinality[order[currentlyExploringDim]] - 1;
			currentDimDone = true;

			// Array manipulation as mentioned above.
			temp[order[(currentlyExploringDim+1)%15]*2] = '0'; //mod15 parameters math and skip spaces.
		}

		char nextChar = '0' + nextValue; //using this methodology of char concatenation to increment by 0 in free para.
		temp[order[currentlyExploringDim]*2] = nextChar;

		// Last NUM_DIMS_DEPENDENT3 configuration parameters are not independent.
		// They depend on one or more parameters already set. Determine the
		// remaining parameters based on already decided independent ones.
		//
		string configSoFar = temp;

		// Populate this object using corresponding parameters from config.
		temp.replace(15*2,5,generateCacheLatencyParams(configSoFar)); //mathematical operation to quick inject/replace value at specified array
		// ^ to include the last 3 para

		// Configuration is ready now.
		nextconfiguration = temp; 

		// Make sure we start exploring next dimension in next iteration.
		if (currentDimDone) {
			currentlyExploringDim++;
			currentDimDone = false;
		}

		// Signal that DSE is complete after this configuration.
		if (currentlyExploringDim == (NUM_DIMS - NUM_DIMS_DEPENDENT))
			isDSEComplete = true;
		
		//from runprojectsuite.sh, calculated total dims is 104, therefore if loops are more than 104, then it achieve peak optimization.
		if (burnout > 104) { //absolute worse case scenario, including changes in validation cache stuff
			return currentconfiguration;
		}
		burnout++; // calculated '104' from runprojectsuite.sh file - taking account of all possible total elements in the multidimensional 18digit array
	}
	return nextconfiguration;
}

