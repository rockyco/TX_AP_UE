#include "tx_frame.hpp"

// Apply the precoder to the data
void applyPrecoder(ap_uint<1> userType, ap_uint<UE_BITS> ueIdx, ap_uint<STRM_BITS> strmUE[NUM_STRM],
                   stream<comp16_t> inData[NUM_STRM], stream<std::complex<ap_int<14>>> outData[NUM_ANT]);