#include "tx_frame.hpp"
#include "fft_top.h"

// Run the FFT on the input data
void runFFT(stream<comp16_t> fdPrecData[NUM_STRM], stream<cmpxDataOut> ofdmSymbol[NUM_STRM]);