//////////////////////////////////////////////////////////////////////////////
// This file is part of the following project:
// 5G Wireless: A High-Level Synthesis Testbench
// Author: Jie Lei
// Date: 10/03/2024
//////////////////////////////////////////////////////////////////////////////
#include "tx_frame.hpp"
#include "applyPrecoder.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <vector>
#include <fstream>

int main(void) {

    ap_uint<UE_BITS> ueIdx = 0;
    ap_uint<STRM_BITS> strmUE[NUM_STRM] = {1, 0, 0, 1};
    // Define the input and output data
    stream<comp16_t> inData[NUM_STRM];
    stream<std::complex<ap_int<14>>> outData[NUM_ANT];
    comp16_t inDataBuf[NUM_STRM][NUM_TOTAL_SYMBOLS][SYMBOL_LENGTH];
    comp16_t tmpInData;
    std::complex<ap_int<14>> outDataBuf[NUM_ANT][NUM_TOTAL_SYMBOLS][SYMBOL_LENGTH];
    std::complex<ap_int<14>> goldenDataBuf[NUM_ANT][NUM_TOTAL_SYMBOLS][SYMBOL_LENGTH];
    float rm, im;
    //Reading the file from .txt file
    FILE *fp_in;
    fp_in = fopen("ofdmData.txt", "r");
    if (fp_in == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }    
    for (int j = 0; j < NUM_TOTAL_SYMBOLS; j++) {
        for (int i = 0; i < NUM_STRM; i++) {
            for (int k = 0; k < SYMBOL_LENGTH; k++) {
                fscanf(fp_in, "%f%fi\t", &rm, &im);
                tmpInData = {rm, im};
                inDataBuf[i][j][k] = tmpInData;
                inData[i].write(tmpInData);
            }
            fscanf(fp_in,"\n");
        }
    }
    fclose(fp_in);
    // Write the input data to a file
    FILE *fp_in_out;
    fp_in_out = fopen("precoderIn.txt", "w");
    if (fp_in_out == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    for (int i = 0; i < NUM_TOTAL_SYMBOLS; i++) {
        for (int k = 0; k < NUM_STRM; k++) {
            for (int j = 0; j < SYMBOL_LENGTH; j++) {            
                comp16_t tmpData = inDataBuf[k][i][j];
                fprintf(fp_in_out, "%f%+fi\t", (float)tmpData.real(), (float)tmpData.imag());
            }
            fprintf(fp_in_out, "\n");
        }
    }
    fclose(fp_in_out);
    // Call the applyPrecoder function
    applyPrecoder(AP, ueIdx, strmUE, inData, outData);

    // Read the golden output data
    FILE *fp_golden;
    fp_golden = fopen("precoderGolden.txt", "r");
    if (fp_golden == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    // Compare the output data with the golden output data
    int err = 0;
    float rm_golden, im_golden;
    for (int k = 0; k < NUM_ANT; k++) {
        for (int i = 0; i < NUM_TOTAL_SYMBOLS; i++) {
            for (int j = 0; j < SYMBOL_LENGTH; j++) {            
                fscanf(fp_golden, "%f%fi\t", &rm_golden, &im_golden);
                std::complex<ap_int<14>> goldenData = {rm_golden, im_golden};
                std::complex<ap_int<14>> tmpData = outData[k].read();
                //std::cout << "goldenData: " << goldenData << std::endl;
                //std::cout << "tmpData: " << tmpData << std::endl;
                outDataBuf[k][i][j] = tmpData;
                goldenDataBuf[k][i][j] = goldenData;
                if ((std::abs(goldenData.real() - tmpData.real()) > 2) || (std::abs(goldenData.imag() - tmpData.imag()) > 2)) {
                    err++;
                }
            }
        }
        fscanf(fp_golden, "\n");
    }
    fclose(fp_golden);

    // Write the output data to a file
    FILE *fp_out;
    fp_out = fopen("precoderOut.txt", "w");
    if (fp_out == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    for (int i = 0; i < NUM_TOTAL_SYMBOLS; i++) {
        for (int j = 0; j < SYMBOL_LENGTH; j++) {
            for (int k = 0; k < NUM_ANT; k++) {
                std::complex<ap_int<14>> tmpData = outDataBuf[k][i][j];
                //std::cout << "tmpData: " << tmpData << std::endl;
                fprintf(fp_out, "%i%+ii\t", (int)tmpData.real(), (int)tmpData.imag());
            }
            fprintf(fp_out, "\n");
        }
    }
    fclose(fp_out);
    // Write the golden output data to a file
    FILE *fp_golden_out;
    fp_golden_out = fopen("precoderGoldenOut.txt", "w");
    if (fp_golden_out == NULL) {
        printf("FILE NOT FOUND\n");// Error, file not found
        return 1;
    }
    for (int i = 0; i < NUM_TOTAL_SYMBOLS; i++) {
        for (int j = 0; j < SYMBOL_LENGTH; j++) {
            for (int k = 0; k < NUM_ANT; k++) {
                std::complex<ap_int<14>> tmpData = goldenDataBuf[k][i][j];
                fprintf(fp_golden_out, "%i%+ii\t", (int)tmpData.real(), (int)tmpData.imag());
            }
            fprintf(fp_golden_out, "\n");
        }
    }
    fclose(fp_golden_out);
    // Print the test result



    if (err == 0) {
        printf("PASSED\n");
    } else {
        printf("FAILED: err = %d\n", err);
    }
    return err;

}
