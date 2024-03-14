//////////////////////////////////////////////////////////////////////////////
// This is a testbench for the formPhyHeader function
// Author: Jie Lei
// Date: 05/03/2024
//////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>
#include "tx_frame.hpp"
#include "formPhyHeader.hpp"

// This is the testbench for testing the formPhyHeader module

int main(void) {
    ap_uint<1> userType = UE;
    ap_uint<UE_BITS> ueIdx = 0;
    ap_uint<MOD_BITS> modUE[NUM_UE] = {1, 1, 1, 1};
    ap_uint<STRM_BITS> strmUE[NUM_STRM] = {0, 1, 0, 1};
    ap_uint<STRM_LEN_BITS> strmLen[NUM_STRM] = {56, 0, 56, 0};
    ap_int<PHY_HEADER_BITS> phyHeaderMod[NUM_STRM][NUM_USED_SUBC];
    stream<ap_int<PHY_HEADER_BITS>> phyHeader[NUM_STRM];
    // Read the input data from the file

    // Call the formPhyHeader function
    formPhyHeader(userType, ueIdx, modUE, strmUE, strmLen, phyHeader);
    // Read the output data from the stream
    for (int i = 0; i < NUM_STRM; i++) {
        for (int j = 0; j < NUM_USED_SUBC; j++) {
            phyHeaderMod[i][j] = phyHeader[i].read();
        }
    }
    // Write the output data to the file
    FILE *fp_A;
    fp_A = fopen("phyHeaderMod.txt", "w");
    for (int i = 0; i < NUM_STRM; i++) {
        for (int j = 0; j < NUM_USED_SUBC; j++) {
            fprintf(fp_A,"%i\n", (int)phyHeaderMod[i][j]);
        }
    }
    fclose(fp_A);
    return 0;
}