//////////////////////////////////////////////////////////////////////////////
// This file contains the implementation of the applyPrecoder class.
// The applyPrecoder class applies the
// precoding matrix to the input data.
// Author: Jie Lei
// Data: 08/03/2024
//////////////////////////////////////////////////////////////////////////////
#include "tx_frame.hpp"

// Apply the precoder to the input data
void applyPrecoder(ap_uint<1> userType, ap_uint<UE_BITS> ueIdx, ap_uint<STRM_BITS> strmUE[NUM_STRM],
                   stream<comp16_t> inData[NUM_STRM], stream<std::complex<ap_int<14>>> outData[NUM_ANT]) {
    // Create the precoding matrix
    ap_int<2> precoder[NUM_ANT][NUM_STRM] = {
        {1, 1, 1, 1},
        {1,-1, 1,-1},
        {1, 1,-1,-1},
        {1,-1,-1, 1},
        {1, 1, 1, 1},
        {1,-1, 1,-1},
        {1, 1,-1,-1},
        {1,-1,-1, 1}};
    ap_int<2> precoderRev[NUM_ANT][NUM_STRM];
    // Create the input and output data
    ap_fixed<18, 14> scalarFactor = 5792.6187514802;
    comp16_t tmpData;
    ap_fixed<18, 13> tmpReal, tmpImag;
    std::complex<ap_fixed<18, 14>> scalarDataTmp;
    std::complex<ap_fixed<18, 14>> accCol[NUM_ANT];
    std::complex<ap_int<14>> tmpOutData;
    // Revise the precoding matrix
    for (int i = 0; i < NUM_STRM; i++) {
        for (int j = 0; j < NUM_ANT; j++) {
            if (userType == AP) {
                precoderRev[j][i] = precoder[j][i];
            } else {
                if (ueIdx == strmUE[i]) {
                    precoderRev[j][i] = precoder[j][i];
                } else {
                    precoderRev[j][i] = 0;
                }
            }
        }
    }

    // Apply the precoder to the input data
    for (int i = 0; i < NUM_TOTAL_SYMBOLS; i++) {
        for (int k = 0; k < SYMBOL_LENGTH; k++) {
            // Initialize the accumulator
            for (int l = 0; l < NUM_ANT; l++) {
                accCol[l] = 0;
            }
            for (int j = 0; j < NUM_STRM; j++) {           
                tmpData = inData[j].read();
                //std::cout << "tmpData: " << tmpData << std::endl;
                tmpReal = tmpData.real() * scalarFactor;
                //std::cout << "tmpData.real(): " << tmpData.real() << std::endl;
                //std::cout << "scalarFactor: " << scalarFactor << std::endl;
                //std::cout << "tmpReal: " << tmpReal << std::endl;
                tmpImag = tmpData.imag() * scalarFactor;
                //std::cout << "tmpImag: " << tmpImag << std::endl;
                scalarDataTmp = std::complex<ap_fixed<18, 14>>(tmpReal, tmpImag);
                //std::cout << "scalarDataTmp: " << scalarDataTmp << std::endl;
                for (int l = 0; l < NUM_ANT; l++) {
                    if (precoderRev[l][j] == 1) {
                        accCol[l] = accCol[l] + scalarDataTmp;
                    } else if (precoderRev[l][j] == -1) {
                        accCol[l] = accCol[l] - scalarDataTmp;
                    }
                }
            }
            // Write the output data
            for (int l = 0; l < NUM_ANT; l++) {
                tmpOutData = accCol[l];
                outData[l].write(tmpOutData);
                //std::cout << "tmpOutData: " << tmpOutData << std::endl;
            }
        }
    }
}
