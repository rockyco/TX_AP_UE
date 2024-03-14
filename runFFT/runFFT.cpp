//////////////////////////////////////////////////////////////////////////////
// This file contains the main function for the inverse FFT. It reads in the
// input data from a file, calls the FFT function, and writes the output data
// to a file.
// Author: Jie Lei
// Data: 08/03/2024
//////////////////////////////////////////////////////////////////////////////
#include "runFFT.hpp"


// Call the FFT function
void runFFT(stream<comp16_t> fdPrecData[NUM_STRM], stream<cmpxDataOut> ofdmSymbol[NUM_STRM]) {
    bool ovflo;
    bool direction = 0; // 0 for IFFT
    cmpxDataIn tmpData;
    bool usedSubc[U_FFT_LENGTH] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1};
    // Create the input and output streams for the FFT
    stream<cmpxDataIn> strmDat2FFT[NUM_STRM];
    // Process all the symbols including LTS, PHY header, and payload
    for (int i = 0; i < NUM_TOTAL_SYMBOLS; i++) {
        // Assemble the input data
        for (int j = 0; j < NUM_STRM; j++) {
            // Assemble the input data
            for (int k = 0; k < U_FFT_LENGTH; k++) {
                if (usedSubc[k]) {
                    tmpData = (cmpxDataIn)fdPrecData[j].read();
                } else {
                    // Write zeros to the unused subcarriers
                    tmpData = cmpxDataIn(0, 0);
                }
                strmDat2FFT[j].write(tmpData);
                //std::cout << "tmpData: " << tmpData << std::endl;
            }
        }
        // Call the FFT function
        for (int j = 0; j < NUM_STRM; j++) {
            fft_top(direction, strmDat2FFT[j], ofdmSymbol[j], &ovflo);
        }
    }
}
