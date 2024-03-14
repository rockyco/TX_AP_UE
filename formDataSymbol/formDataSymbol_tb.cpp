//////////////////////////////////////////////////////////////////////
// This is the testbench for testing the formDataSymbol module.
// Author: Jie Lei
// Date: 14/03/2024
//////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <fstream>
#include <complex>
#include <iostream>
#include "formDataSymbol.hpp"

// The formDataSymbol_tb function is used to test the formDataSymbol module.
int main(void) {
    // Define the input and output streams
    stream<uint8_t> dataMAC[NUM_UE];
    uint10_t dataMAC_len[NUM_UE] = {112, 112, 0, 0};
    stream<comp17_t> dataSymOut[NUM_STRM];
    ap_uint<1> userType = AP;
    ap_uint<UE_BITS> ueIdx = 0;
    ap_uint<MOD_BITS> modUE[NUM_UE] = {1, 1, 1, 1};
    ap_uint<STRM_BITS> strmUE[NUM_STRM] = {1, 0, 0, 1};
    comp17_t outDataBuf[NUM_STRM][NUM_PAYLOAD][NUM_USED_SUBC];
    comp17_t refDataBuf[NUM_STRM][NUM_PAYLOAD][NUM_USED_SUBC];

    // Reading the input data from .txt file
    FILE *fp_in;
    fp_in = fopen("dataMAC.txt", "r");
    if (fp_in == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    uint8_t data;
    for (int i = 0; i < NUM_STRM; i++) {
        for (int j = 0; j < dataMAC_len[i]; j++) {
            fscanf(fp_in, "%hhu\t", &data);
            dataMAC[i].write(data);
            //std::cout << (int)data << std::endl;
        }
        fscanf(fp_in,"\n");
    }
    fclose(fp_in);
    // Call the formDataSymbol function
    formDataSymbol(userType, ueIdx, modUE, strmUE, dataMAC, dataMAC_len, dataSymOut);
    // Write the output data to .txt file
    FILE *fp_out;
    fp_out = fopen("dataSymOut.txt", "w");
    if (fp_out == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    comp17_t dataSym;
    for (int j = 0; j < NUM_PAYLOAD; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                dataSym = dataSymOut[i].read();
                outDataBuf[i][j][k] = dataSym;
                fprintf(fp_out, "%f%+fi\t", (float)dataSym.real(), (float)dataSym.imag());
            }
            fprintf(fp_out,"\n");
        }
    }
    fclose(fp_out);
    // Read the reference data from .txt file
    FILE *fp_ref;
    float rmRef, imRef;
    fp_ref = fopen("data4phaseRot.txt", "r"); // use data4phaseRot.txt as reference data with modificiation of deleting the first four lines
    if (fp_ref == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    for (int j = 0; j < NUM_PAYLOAD; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                fscanf(fp_ref, "%f%fi\t", &rmRef, &imRef);
                refDataBuf[i][j][k] = comp17_t(rmRef, imRef);
            }
            fscanf(fp_ref,"\n");
        }
    }
    fclose(fp_ref);
    // Compare the generated data with the reference data
    int errCnt = 0;
    for (int j = 0; j < NUM_PAYLOAD; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                if ((std::abs((float)outDataBuf[i][j][k].real() - (float)refDataBuf[i][j][k].real()) > 0.001) || (std::abs((float)outDataBuf[i][j][k].imag() - (float)refDataBuf[i][j][k].imag()) > 0.001)) {
                    errCnt++;
                    printf("Error: outDataBuf[%d][%d][%d] = %f%+fi, refDataBuf[%d][%d][%d] = %f%+fi\n", 
                            i, j, k, (float)outDataBuf[i][j][k].real(), (float)outDataBuf[i][j][k].imag(), 
                            i, j, k, (float)refDataBuf[i][j][k].real(), (float)refDataBuf[i][j][k].imag());
                }
            }
        }
    }
    if (errCnt == 0) {
        printf("Test passed\n");
    } else {
        printf("Test failed\n");
    }
    return errCnt;
}