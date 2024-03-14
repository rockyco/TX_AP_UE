/////////////////////////////////////////////////////////////////////////////
// formPhyHeader.cpp : implementation file
// Author:  Jie Lei
// Date:    05/03/2024
/////////////////////////////////////////////////////////////////////////////
#include "tx_frame.hpp"

// Generate the PHY header bit for the frame
void formPhyHeader(ap_uint<1> userType, ap_uint<UE_BITS> ueIdx, ap_uint<MOD_BITS> modUE[NUM_UE], ap_uint<STRM_BITS> strmUE[NUM_STRM], 
                ap_uint<STRM_LEN_BITS> strmLen[NUM_STRM], stream<ap_int<PHY_HEADER_BITS>> phyHeader[NUM_STRM])
{
    ap_uint<STRM_BITS> strmMod[NUM_STRM];
    ap_uint<NUM_DATA_SUBC> phyHeaderBit[NUM_STRM];
    ap_int<PHY_HEADER_BITS> phyPilot[NUM_STRM][NUM_PILOTS] = {{-1, -1}, {-1, -1}, {1, 1}, {1, 1}};
    int pilotLoc[NUM_PILOTS] = {0, 15};
    ap_int<PHY_HEADER_BITS> phyHeaderMod[NUM_STRM][NUM_USED_SUBC];
    ap_int<PHY_HEADER_BITS> tmpComp;
    // Set the modulation bits for the streams
    for (int i = 0; i < NUM_STRM; i++) {
        strmMod[i] = modUE[strmUE[i]];
    }
    // Set the PHY header bits
    for (int i = 0; i < NUM_STRM; i++) {
        if (userType == AP) {
            //phyHeaderBit[i] = ((ap_uint<NUM_DATA_SUBC>)strmMod[i] << (STRM_BITS+STRM_LEN_BITS)) | ((ap_uint<NUM_DATA_SUBC>)strmUE[i] << STRM_LEN_BITS) | strmLen[i];
        	phyHeaderBit[i] = (strmMod[i], strmUE[i], strmLen[i]);
        } else {
            if (ueIdx == strmUE[i]) {
                //phyHeaderBit[i] = ((ap_uint<NUM_DATA_SUBC>)strmMod[i] << (STRM_BITS+STRM_LEN_BITS)) | ((ap_uint<NUM_DATA_SUBC>)strmUE[i] << STRM_LEN_BITS) | strmLen[i];
                phyHeaderBit[i] = (strmMod[i], strmUE[i], strmLen[i]);
            } else {
                phyHeaderBit[i] = 0;
            }
        }
    }
    // Get the PHY header modulation bits
    for (int i = 0; i < NUM_STRM; i++) {
        for (int j = DATA_SUBC_OFFSET; j < NUM_DATA_SUBC+DATA_SUBC_OFFSET; j++) {
            if (ueIdx == strmUE[i]) {
                if (phyHeaderBit[i] & (1 << NUM_DATA_SUBC-1)) {
                    phyHeaderMod[i][j] = 1;
                } else {
                    phyHeaderMod[i][j] = -1;
                }
                phyHeaderBit[i] = phyHeaderBit[i] << 1;
            }
        }
    }
    // Get the PHY header pilot bits
    for (int i = 0; i < NUM_STRM; i++) {
        for (int j = 0; j < NUM_PILOTS; j++) {
            phyHeaderMod[i][pilotLoc[j]] = phyPilot[i][j];
        }
    }
    // Write the PHY header bits to the output stream
    for (int i = 0; i < NUM_STRM; i++) {
        for (int j = 0; j < NUM_USED_SUBC; j++) {
            tmpComp = phyHeaderMod[i][j];
            phyHeader[i].write(tmpComp);
        }
    }
}
