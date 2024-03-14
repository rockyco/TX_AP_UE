////////////////////////////////////////////////////////////////////////////////////
// This file is generated. Do not modify.
// Path: phaseRotation/phaseRotation_tb.cpp
// Author: Jie Lei
// Date: 12/03/2024
////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <fstream>
#include <complex>
#include <iostream>
#include "phaseRotation.hpp"

// This is the testbench for testing the phaseRotation module
int main(void) {
    // Define the input and output data
    stream<ap_int<PHY_HEADER_BITS>> phyHeader[NUM_STRM];
    stream<comp17_t> payloadData[NUM_STRM];
    stream<comp16_t> phaseRotData[NUM_STRM];
    comp17_t inputDataBuf[NUM_STRM][NUM_PAYLOAD+NUM_PHY_HEADER][NUM_USED_SUBC];
    comp17_t phaseRotDataBuf[NUM_STRM][NUM_PAYLOAD+NUM_PHY_HEADER][NUM_USED_SUBC];
    ap_int<PHY_HEADER_BITS> tmpBits;
    comp17_t tmpInComp;
    comp17_t tmpComp;
    // Reading the file from .txt file
    FILE *fp_in;
    fp_in = fopen("data4phaseRot.txt", "r");
    if (fp_in == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    float rm, im;
    for (int j = 0; j < NUM_PAYLOAD+NUM_PHY_HEADER; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                fscanf(fp_in, "%f%fi\t", &rm, &im);
                tmpInComp = {rm, im};
                inputDataBuf[i][j][k] = tmpInComp;
                if (j == 0) {
                    phyHeader[i].write((ap_int<PHY_HEADER_BITS>)tmpInComp.real());
                } else {
                    payloadData[i].write((comp17_t)tmpInComp);
                }
            }
            fscanf(fp_in,"\n");
        }
    }
    fclose(fp_in);
    // Call the phaseRotation function
    phaseRotation(phyHeader, payloadData, phaseRotData);
    // Write the output data to the .txt file
    FILE *fp_out;
    fp_out = fopen("phaseRotData.txt", "w");
    if (fp_out == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    fixed17_t tmpReal, tmpImag;
    for (int j = 0; j < NUM_PAYLOAD+NUM_PHY_HEADER; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                tmpComp = phaseRotData[i].read();
                //if (j == 0) {
                    tmpReal = tmpComp.real() * 2;
                    tmpImag = tmpComp.imag() * 2;
                    tmpComp.real(tmpReal);
                    tmpComp.imag(tmpImag);
                //}
                phaseRotDataBuf[i][j][k] = tmpComp;
                fprintf(fp_out, "%f%+fi\t", (float)tmpComp.real(), (float)tmpComp.imag());
            }
            fprintf(fp_out,"\n");
        }
    }
    fclose(fp_out);
    // Read the reference data from the .txt file
    FILE *fp_ref;
    fp_ref = fopen("refData_phaseRot.txt", "r");
    if (fp_ref == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    comp17_t refData[NUM_STRM][NUM_PAYLOAD+NUM_PHY_HEADER][NUM_USED_SUBC];
    float refRm, refIm;
    for (int j = 0; j < NUM_PAYLOAD+NUM_PHY_HEADER; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                fscanf(fp_ref, "%f%fi\t", &refRm, &refIm);
                refData[i][j][k] = {refRm, refIm};
            }
            fscanf(fp_ref,"\n");
        }
    }
    fclose(fp_ref);
    // Compare the generated data with the reference data
    int err_cnt = 0;
    for (int j = 0; j < NUM_PAYLOAD+NUM_PHY_HEADER; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                if ((std::abs((float)phaseRotDataBuf[i][j][k].real() - (float)refData[i][j][k].real()) > 0.001) || (std::abs((float)phaseRotDataBuf[i][j][k].imag() - (float)refData[i][j][k].imag()) > 0.001)) {
                    err_cnt++; 
                    printf("Error: phaseRotDataBuf[%d][%d][%d] = %f%+fi, refData[%d][%d][%d] = %f%+fi\n", i, j, k, (float)phaseRotDataBuf[i][j][k].real(), (float)phaseRotDataBuf[i][j][k].imag(), i, j, k, (float)refData[i][j][k].real(), (float)refData[i][j][k].imag());                   
                }
            }
        }
    }
    return err_cnt;

}