#include "tx_frame.hpp"

// The formDataSymbol function is used to form the data symbols for transmission.
void loadMACData(stream<uint8_t> dataMAC[NUM_UE], uint10_t dataMAC_len[NUM_UE], ap_uint<1> userType, 
                 ap_uint<UE_BITS> ueIdx, ap_uint<MOD_BITS> modUE[NUM_UE], ap_uint<STRM_BITS> strmUE[NUM_STRM], 
                 stream<uint8_t> dataMACOut[NUM_STRM], stream<uint10_t> strmLenOut[NUM_STRM], stream<uint2_t>& modTypeOut);

void splitOneStream(uint2_t strmMod, uint10_t strmLen, stream<uint8_t>& dataMAC, stream<uint4_t>& dataBits);

void splitMACData(stream<uint8_t> dataMAC[NUM_STRM], stream<uint10_t> strmLen[NUM_STRM], stream<uint2_t>& modTypeIn, 
                  stream<uint4_t> dataBits[NUM_STRM], stream<uint2_t>& modTypeOut);

void modulateData(stream<uint2_t>& modType, stream<uint4_t> dataBit[NUM_STRM], stream<comp17_t> dataSymbol[NUM_STRM]);

void formDataSymbol(ap_uint<1> userType, ap_uint<UE_BITS> ueIdx, ap_uint<MOD_BITS> modUE[NUM_UE], ap_uint<STRM_BITS> strmUE[NUM_STRM], 
                stream<uint8_t> dataMAC[NUM_UE], uint10_t dataMAC_len[NUM_UE], stream<comp17_t> dataSymbol[NUM_STRM]);