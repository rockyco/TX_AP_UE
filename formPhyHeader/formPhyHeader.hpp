#include "tx_frame.hpp"

// Generate the PHY header bit for the frame
void formPhyHeader(ap_uint<1> userType, ap_uint<UE_BITS> ueIdx, ap_uint<MOD_BITS> modUE[NUM_UE], ap_uint<STRM_BITS> strmUE[NUM_STRM], 
                ap_uint<STRM_LEN_BITS> strmLen[NUM_STRM], stream<ap_int<PHY_HEADER_BITS>> phyHeader[NUM_STRM]);