#include "tx_frame.hpp"
#include "fft_top.h"

// Add CP to the IFFT data
void addCP(stream<cmpxDataIn> inData[NUM_STRM], stream<comp16_t> outData[NUM_STRM]);