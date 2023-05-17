// EPANET_Cmake.cpp : Defines the entry point for the application.
//

#include "EPANET_Cmake.h"

// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


void runHydraulics(EN_Project ph, int* res) {
    int errorcode;
    errorcode = EN_openH(ph);
    if (errorcode)
        return;

    errorcode = EN_initH(ph, 10);
    if (errorcode)
        return;

    bool scheduled;
    int currentTime = 0;
    int timeIndex = 0;
    long t = 0;
    long tStep = 0;

    long hStep;
    errorcode = EN_gettimeparam(ph, EN_REPORTSTEP, &hStep);
    errorcode = EN_gettimeparam(ph, EN_HYDSTEP, &hStep);
    errorcode = EN_settimeparam(ph, EN_REPORTSTEP, hStep);
    //std::cout << "Hydraulic time step is " << hStep << " " << std::endl;
    if (errorcode)
        return;

    long duration;
    errorcode = EN_gettimeparam(ph, EN_DURATION, &duration);
    //std::cout << "Duration of the simulation is " << duration <<" " << std::endl;
    if (errorcode)
        return;

    long lTimestepCount = (duration / hStep) + 1; // the number of expected timesteps we will store.
    int nNodes;
    errorcode = EN_getcount(ph, EN_NODECOUNT, &nNodes);

    std::vector<std::vector<double>> pressures;

    // keeps track of whether any of the timesteps have failed
    bool solutionFailure = false;

    // temp plot patterns
   /* int nPatt;
    double val;
    errorcode = EN_getcount(ph, EN_PATCOUNT, &nPatt);
    for (int pattIdx = 1; pattIdx <= nPatt; ++pattIdx) {
        int nPeriods;
        std::cout << "Pattern n " << pattIdx << std::endl;
        errorcode = EN_getpatternlen(ph, pattIdx, &nPeriods);
        for (int tIdx = 1; tIdx <= nPeriods; ++tIdx) {
            errorcode = EN_getpatternvalue(ph, pattIdx, tIdx, &val);
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }*/

    do {
        currentTime += tStep;
        /* if (currentTime > timeIndex * hStep) {
             std::cout << "this shouldn't happen" << "ct: " << currentTime << "t: " <<t << "ideal: " << timeIndex * hStep << std::endl;
         }*/
         // check if this is a timestep that we are expecting or which has been inserted due to e.g. tank status changing
        scheduled = (currentTime != 0) && (currentTime % hStep == 0);

        // run this timestep
        errorcode = EN_runH(ph, &t);
        _ASSERT(errorcode < 100);

        if (errorcode)
            if (errorcode >= 100)
                solutionFailure = true;

        // move the simulator onto the next timestep
        errorcode = EN_nextH(ph, &tStep);
        _ASSERT(errorcode < 100);


        // check to see if this was a scheduled timestep - 
        //  if not then we shouldn't be saving the results and this block will be skipped, unless I have energy for pumps or this type of data to pull out 
        if (scheduled || (currentTime == 0 && tStep == hStep)) {
            // @Dennis - this is where you store your results.
            // use EN_getnodevalue/EN_getlinkvalue to pull the data out of the model
            // the variable "timeIndex" can be used to identify which timestep this is.
            std::vector<double> tmp_pressures(nNodes);
            for (int nodeIdx = nNodes; nodeIdx; --nodeIdx) {
                errorcode = EN_getnodevalue(ph, nodeIdx, EN_PRESSURE, &tmp_pressures[nodeIdx - 1]);
                _ASSERT(errorcode < 100);
            }

            pressures.push_back(tmp_pressures);

            // e.g.
            // error= ENgetnodevalue(nodeIndex, EN_PRESSURE, &pressure[timeIndex]);
            ++timeIndex;
        }


    } while (tStep);

    *res = 0;
    // check condition 
    for (int timeIdx = 0; timeIdx < lTimestepCount; ++timeIdx) {
        for (int nodeIdx = 0; nodeIdx < 19; ++nodeIdx) {
            if (pressures[timeIdx][nodeIdx] <= 0) {
                *res = 1;
                return;
            }
        }
    }

    errorcode = EN_close(ph);
    if (errorcode)
        return;
}

void modifyNetwork(int code, EN_Project ph) {
    int error;

    int idx;

    if (code == 5) {
        //use original
    }
    else if (code == 4) {
        // pipe 29 diam 24
        error = EN_getlinkindex(ph, "29", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 24);
        // pattern 2 all 1
        error = EN_getpatternindex(ph, "2", &idx);
        double ptt[24] = { 0 };
        for (int tIdx = 0; tIdx < 24; ++tIdx) {
            ptt[tIdx] = 1;
        }
        error = EN_setpattern(ph, idx, ptt, 24);// I know it is 24 long 
        // pipe 39 diam 24 
        error = EN_getlinkindex(ph, "39", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 24);
    }
    else if (code == 3) {
        // pattern 3 1 fino alle 6 e dopo le 13 
        error = EN_getpatternindex(ph, "3", &idx);
        double ptt[24] = { 0 };
        for (int tIdx = 0; tIdx < 6; ++tIdx) {
            ptt[tIdx] = 1;
        }
        for (int tIdx = 12; tIdx < 24; ++tIdx) {
            ptt[tIdx] = 1;
        }
        error = EN_setpattern(ph, idx, ptt, 24);// I know it is 24 long 
        // pipe 2 diam 42 
        error = EN_getlinkindex(ph, "2", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 42);
        // pipe 23 diam 24
        error = EN_getlinkindex(ph, "23", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 24);
        // pipe 21 diam 26
        error = EN_getlinkindex(ph, "21", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 26);
    }
    else if (code == 2) {
        // pipe 26 diam 34
        error = EN_getlinkindex(ph, "26", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 34);
        // pipe 17 diam 18
        error = EN_getlinkindex(ph, "17", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 18);
    }
    else if (code == 1) {
        // pipe 110 diam 10
        error = EN_getlinkindex(ph, "110", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 10);
        // pipe 113 diam 10
        error = EN_getlinkindex(ph, "113", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 10);
        // pipe 114 diam 6
        error = EN_getlinkindex(ph, "114", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 6);
        // pipe 115 diam 14
        error = EN_getlinkindex(ph, "115", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 14);
        // pipe 116 diam 16
        error = EN_getlinkindex(ph, "116", &idx);
        error = EN_setlinkvalue(ph, idx, EN_DIAMETER, 16);
    }
}

int main()
{
    std::cout << "Hello World!\n";
    int errorcode;

    //load the network
    EN_Project ph;
    EN_createproject(&ph);
    errorcode = EN_open(ph, "D:/data/anytown.inp", "reportfile.txt", "outfile.txt");
    if (errorcode)
        return errorcode;
    /*
        std::cout << "Pre-network modification";
        int idx;
        double par;
        errorcode = EN_getlinkindex(ph, "29", &idx);
        EN_getlinkvalue(ph, idx, EN_DIAMETER, &par);
        std::cout << par <<" " << std::endl;
        modifyNetwork(4, ph);

        errorcode = EN_getlinkindex(ph, "29", &idx);
        EN_getlinkvalue(ph, idx, EN_DIAMETER, &par);
        if (par == 24)
            std::cout << "Successfull changed the diameter" << std::endl; */
            //for 5 times do 
    for (int i = 5; i; --i) {
        // modify the network 
        modifyNetwork(i, ph);
        // simulate the network
        runHydraulics(ph, &errorcode);
        std::cout << i << " res: " << errorcode << std::endl;
        // compute average pressure
    }

    EN_deleteproject(ph);
    return errorcode;
}
