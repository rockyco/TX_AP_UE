///////////////////////////////////////////////////////////////////////////////////////
// This file is the testbench for testing the feedbackPrecoder module.
// Author: Jie Lei
// Date: 07/03/2024
///////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <fstream>
#include <complex>
#include <iostream>
#include "feedbackPrecoder.hpp"

// The feedbackPrecoder_tb function is used to test the feedbackPrecoder module.
int main(void) {
    // Define the input and output streams
    stream<comp16_t> phaseRotData[NUM_STRM];
    stream<comp16_t> fdPrecData[NUM_STRM];
    stream<comp17_t> matFDPrec;
    comp17_t matFDPrecDat[NUM_STRM][NUM_STRM] = {
        {comp17_t(1.0, 0.0), comp17_t(0.0, 0.0), comp17_t(0.0, 0.0), comp17_t(0.0, 0.0)},
        {comp17_t(0.0, 0.0), comp17_t(1.0, 0.0), comp17_t(0.0, 0.0), comp17_t(0.0, 0.0)},
        {comp17_t(0.0, 0.0), comp17_t(0.0, 0.0), comp17_t(1.0, 0.0), comp17_t(0.0, 0.0)},
        {comp17_t(0.0, 0.0), comp17_t(0.0, 0.0), comp17_t(0.0, 0.0), comp17_t(1.0, 0.0)}
    };
    comp17_t outDataBuf[NUM_STRM][NUM_TOTAL_SYMBOLS][NUM_USED_SUBC];
    comp17_t refDataBuf[NUM_STRM][NUM_TOTAL_SYMBOLS][NUM_USED_SUBC];

    // Initialize the input streams
    for (int i = 0; i < NUM_STRM; i++) {
        for (int j = 0; j < NUM_STRM; j++) {
            matFDPrec.write(matFDPrecDat[i][j]);
        }
    }
    // Reading the input data from .txt file
    FILE *fp_in;
    fp_in = fopen("refData_phaseRot.txt", "r");
    if (fp_in == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    float rm, im;
    for (int j = 0; j < (NUM_PAYLOAD + NUM_PHY_HEADER); j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                fscanf(fp_in, "%f%fi\t", &rm, &im);
                rm /= 2.0;
                im /= 2.0;
                phaseRotData[i].write(comp16_t(rm, im));
            }
            fscanf(fp_in,"\n");
        }
    }
    fclose(fp_in);


    // Call the feedbackPrecoder function
    feedbackPrecoder(matFDPrec, phaseRotData, fdPrecData);

    // Write the output data to the .txt file
    FILE *fp_out;
    fp_out = fopen("fdPrecData.txt", "w");
    if (fp_out == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    fixed17_t tmpReal, tmpImag;
    for (int j = 0; j < NUM_TOTAL_SYMBOLS; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                comp17_t tmp = (comp17_t)fdPrecData[i].read();
                tmpReal = tmp.real() * 2;
                tmpImag = tmp.imag() * 2;
                tmp.real(tmpReal);
                tmp.imag(tmpImag);
                outDataBuf[i][j][k] = tmp;
                fprintf(fp_out, "%f%+fi\t", (float)tmp.real(), (float)tmp.imag());
            }
            fprintf(fp_out,"\n");
        }
    }
    fclose(fp_out);
    // Reading the reference data from .txt file
    FILE *fp_ref;
    fp_ref = fopen("refData_fdPrec.txt", "r");
    if (fp_ref == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    for (int j = 0; j < NUM_TOTAL_SYMBOLS; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                fscanf(fp_ref, "%f%fi\t", &rm, &im);
                refDataBuf[i][j][k] = comp17_t(rm, im);
            }
            fscanf(fp_ref,"\n");
        }
    }
    fclose(fp_ref);
    // Compare the output data with the reference data
    int err_cnt = 0;
    for (int j = 0; j < NUM_TOTAL_SYMBOLS; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                if (std::abs((float)outDataBuf[i][j][k].real() - (float)refDataBuf[i][j][k].real()) > 0.001 || std::abs((float)outDataBuf[i][j][k].imag() - (float)refDataBuf[i][j][k].imag()) > 0.001) {
                    err_cnt++;
                    std::cout << "Mismatch detected on output data: outDataBuf[" << i << "][" << j << "][" << k << "] = " << outDataBuf[i][j][k] << ", refDataBuf[" << i << "][" << j << "][" << k << "] = " << refDataBuf[i][j][k] << std::endl;
                }
            }
        }
    }
    if (err_cnt == 0) {
        std::cout << "Test passed!" << std::endl;
    } else {
        std::cout << "Test failed!" << std::endl;
    }
    return err_cnt;
}