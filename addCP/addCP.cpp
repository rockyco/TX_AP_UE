/////////////////////////////////////////////////////////////////////////////
// This file contains the implementation of the class addCP. This class
// adds a cyclic prefix to the input data.
// Author: Jie Lei
// Data: 08/03/2024
/////////////////////////////////////////////////////////////////////////////
#include "addCP.hpp"

// Add cyclic prefix to the input data
void addCP(stream<cmpxDataIn> inData[NUM_STRM], stream<comp16_t> outData[NUM_STRM]) {
    // Create the FFT data 
    cmpxDataIn fftData[NUM_STRM][FFT_LENGTH];
    comp16_t tmpData;

    // Add the cyclic prefix to all the symbols
    for (int i = 0; i < NUM_TOTAL_SYMBOLS; i++) {
        for (int j = 0; j < NUM_STRM; j++) {
            // Read the input data
            for (int k = 0; k < FFT_LENGTH; k++) {
                tmpData = (comp16_t)inData[j].read();
                fftData[j][k] = tmpData;
                if (k >= (FFT_LENGTH - CP_LENGTH)) {
                    // Add the cyclic prefix
                    outData[j].write(tmpData);
                }
            }
            // Write the output data
            for (int k = 0; k < FFT_LENGTH; k++) {
                outData[j].write(fftData[j][k]);
            }
        }
    }
}