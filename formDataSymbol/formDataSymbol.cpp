//////////////////////////////////////////////////////////////////////
// formDataSymbol.cpp: implementation of the formDataSymbol class.
// Author:  Jie Lei
// Date:    06/03/2024
//////////////////////////////////////////////////////////////////////
#include "formDataSymbol.hpp"


// Load the MAC data for streams
void loadMACData(stream<uint8_t> dataMAC[NUM_UE], uint10_t dataMAC_len[NUM_UE], ap_uint<1> userType, 
                 ap_uint<UE_BITS> ueIdx, ap_uint<MOD_BITS> modUE[NUM_UE], ap_uint<STRM_BITS> strmUE[NUM_STRM], 
                 stream<uint8_t> dataMACOut[NUM_STRM], stream<uint10_t> strmLenOut[NUM_STRM], stream<uint2_t>& modTypeOut)
{
    ap_uint<MOD_BITS> strmMod[NUM_STRM];
    uint8_t tmp_dataMAC;
    ap_uint<UE_BITS> tmp_ueIdx;
    uint10_t strmLen[NUM_STRM];
    ap_uint<STRM_LEN_BITS> strmMaxLen[NUM_STRM];
    uint10_t dataUELen[NUM_UE];
    ap_uint<STRM_BITS> strmUEloc[NUM_STRM];

    // Set the modulation type and length for the streams
    for (int i = 0; i < NUM_STRM; i++) {
        dataUELen[i] = dataMAC_len[i];
        strmUEloc[i] = strmUE[i];
        strmMod[i] = modUE[strmUEloc[i]];
        modTypeOut.write(strmMod[i]);
        if (strmMod[i] == 0) {  // BPSK
            strmMaxLen[i] = MAX_BYTES_BPSK;
        } else if (strmMod[i] == 1) {  // QPSK
            strmMaxLen[i] = MAX_BYTES_QPSK;
        } else {  // 16QAM
            strmMaxLen[i] = MAX_BYTES_16QAM;
        }
        //std::cout << "strmMod[" << i << "] = " << strmMod[i] << std::endl;
    }
    // Set the length of MAC data read for each stream
    for (int i = 0; i < NUM_STRM; i++) {
        tmp_ueIdx = strmUEloc[i]; // Get the UE index for the stream
        if (userType == AP) {  // AP mode
            if (dataUELen[tmp_ueIdx] > strmMaxLen[i]) { // Load the maximum stream length of MAC data from the UE
                strmLen[i] = strmMaxLen[i];
                dataUELen[tmp_ueIdx] -= strmMaxLen[i]; // Update the remaining MAC data length of the UE
            } else { // Load the remaining MAC data length of the UE
                strmLen[i] = dataUELen[tmp_ueIdx];
            }
        } else { // UE mode
            if (ueIdx == tmp_ueIdx) { // Load the MAC data only if the UE index matches the stream UE index
                if (dataUELen[tmp_ueIdx] > strmMaxLen[i]) {
                    strmLen[i] = strmMaxLen[i];
                    dataUELen[tmp_ueIdx] -= strmMaxLen[i];
                } else {
                    strmLen[i] = dataUELen[tmp_ueIdx];
                }
            } else { // Set the stream length to 0 if the UE index does not match the stream UE index
                strmLen[i] = 0;
            }
        }
        strmLenOut[i].write(strmLen[i]);
        //std::cout << "strmLen[" << i << "] = " << strmLen[i] << std::endl;
    }
    // Load the MAC data for each stream
    for (int i = 0; i < NUM_STRM; i++) {
        //std::cout << "strmLen[" << i << "] = " << strmLen[i] << std::endl;
        //std::cout << "strmUEloc[" << i << "] = " << strmUEloc[i] << std::endl;
        for (int j = 0; j < strmLen[i]; j++) { // The length of the MAC data for each stream is strmLen[i]
            tmp_dataMAC = dataMAC[strmUEloc[i]].read(); // The source of the MAC data is the UE index of the stream
            dataMACOut[i].write(tmp_dataMAC);
            //std::cout << "tmp_dataMAC[" << i << "][" << j << "] = " << (int)tmp_dataMAC << std::endl;
        }
    }
}

// Split the MAC data for one stream
void splitOneStream(uint2_t strmMod, uint10_t strmLen, stream<uint8_t>& dataMAC, stream<uint4_t>& dataBits)
{
    uint8_t tmp_dataMAC;
    uint4_t tmp_dataBits;
    uint4_t times;
    uint8_t mask;
    uint4_t offset;
    uint4_t offsetInit;
    uint4_t stride;
    ap_uint<STRM_LEN_BITS> strmMaxLen;

    if (strmMod == 0) {  // BPSK
        times = 8;
        mask = 0x1;
        offsetInit = 7;
        stride = 1;
        strmMaxLen = MAX_BYTES_BPSK;
    } else if (strmMod == 1) {  // QPSK
        times = 4;
        mask = 0x3;
        offsetInit = 6;
        stride = 2;
        strmMaxLen = MAX_BYTES_QPSK;
    } else {  // 16QAM
        times = 2;
        mask = 0xF;
        offsetInit = 4;
        stride = 4;
        strmMaxLen = MAX_BYTES_16QAM;
    }
    // Process the MAC data
    for (int i = 0; i < strmMaxLen; i++) {
        if (i < strmLen) {
            tmp_dataMAC = dataMAC.read();
        } else {
            tmp_dataMAC = 0;
        }
        //std::cout << "tmp_dataMAC = " << (int)tmp_dataMAC << std::endl;
        offset = offsetInit;
        for (int j = 0; j < times; j++) {
            tmp_dataBits = (tmp_dataMAC >> offset) & mask;
            dataBits.write(tmp_dataBits);
            offset -= stride;
            //std::cout << "tmp_dataBits = " << (int)tmp_dataBits << std::endl;
        }
    }
}
// Split the MAC data into bits
void splitMACData(stream<uint8_t> dataMAC[NUM_STRM], stream<uint10_t> strmLen[NUM_STRM], stream<uint2_t>& modTypeIn, 
                  stream<uint4_t> dataBits[NUM_STRM], stream<uint2_t>& modTypeOut)
{
    uint8_t tmp_dataMAC;
    uint8_t tmp_dataBits;
    ap_uint<MOD_BITS> strmMod[NUM_STRM];
    ap_uint<STRM_LEN_BITS> strmLenIn[NUM_STRM];

    // Get the modulation type and maximum length for the streams
    for (int i = 0; i < NUM_STRM; i++) {
        strmMod[i] = modTypeIn.read();
        strmLenIn[i] = strmLen[i].read();
        modTypeOut.write(strmMod[i]);
        //std::cout << "strmMod[" << i << "] = " << strmMod[i] << std::endl;
        //std::cout << "strmLenIn[" << i << "] = " << strmLenIn[i] << std::endl;
    }
    // Process all the MAC data streams

    for (int i = 0; i < NUM_STRM; i++) {
        splitOneStream(strmMod[i], strmLenIn[i], dataMAC[i], dataBits[i]);
    }
}
// Modulate the data bits to symbols
void modulateData(stream<uint2_t>& modType, stream<uint4_t> dataBit[NUM_STRM], stream<comp17_t> dataSymbol[NUM_STRM])
{
    comp17_t dataPilot_Pos[NUM_STRM][NUM_PILOTS] = {{comp17_t(-0.70709228515625, 0.70709228515625), comp17_t(-0.70709228515625, 0.70709228515625)}, 
                                                    {comp17_t(-0.70709228515625, -0.70709228515625), comp17_t(-0.70709228515625, -0.70709228515625)}, 
                                                    {comp17_t(0.70709228515625, 0.70709228515625), comp17_t(0.70709228515625, 0.70709228515625)}, 
                                                    {comp17_t(0.70709228515625, -0.70709228515625), comp17_t(0.70709228515625, -0.70709228515625)}};

    comp17_t dataPilot_Neg[NUM_STRM][NUM_PILOTS] = {{comp17_t(0.70709228515625, -0.70709228515625), comp17_t(0.70709228515625, -0.70709228515625)},
                                                    {comp17_t(0.70709228515625, 0.70709228515625), comp17_t(0.70709228515625, 0.70709228515625)},
                                                    {comp17_t(-0.70709228515625, -0.70709228515625), comp17_t(-0.70709228515625, -0.70709228515625)},
                                                    {comp17_t(-0.70709228515625, 0.70709228515625), comp17_t(-0.70709228515625, 0.70709228515625)}};
    comp17_t dataBPSK[2] = {comp17_t(-1.0, 0.0), comp17_t(1.0, 0.0)};
    comp17_t dataQPSK[4] = {comp17_t(-0.70709228515625, -0.70709228515625), comp17_t(-0.70709228515625, 0.70709228515625), comp17_t(0.70709228515625, -0.70709228515625), comp17_t(0.70709228515625, 0.70709228515625)};
    comp17_t data16QAM[16] = {comp17_t(-0.9487, -0.9487), comp17_t(-0.9487, -0.3162), comp17_t(-0.9487, 0.9487), comp17_t(-0.9487, 0.3162), 
                              comp17_t(-0.3162, -0.9487), comp17_t(-0.3162, -0.3162), comp17_t(-0.3162, 0.9487), comp17_t(-0.3162, 0.3162), 
                              comp17_t(0.9487, -0.9487), comp17_t(0.9487, -0.3162), comp17_t(0.9487, 0.9487), comp17_t(0.9487, 0.3162), 
                              comp17_t(0.3162, -0.9487), comp17_t(0.3162, -0.3162), comp17_t(0.3162, 0.9487), comp17_t(0.3162, 0.3162)};
    comp17_t tmp_datPilot;
    comp17_t tmp_datSymbol;
    uint4_t tmp_datBit;
    int pilotLoc[NUM_PILOTS] = {0, 15};
    int dataLoc[NUM_DATA_SUBC] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
    ap_int<2> pnSeq[64] = {1, 1, 1,-1,-1,-1, 1, -1,-1,-1,-1, 1, 1,-1, 1, -1,
                          -1, 1, 1,-1, 1, 1,-1,  1, 1, 1, 1, 1, 1,-1, 1,  1, 
                           1,-1, 1, 1,-1,-1, 1,  1, 1,-1, 1,-1,-1,-1, 1, -1, 
                           1,-1,-1, 1,-1,-1, 1,  1, 1, 1, 1,-1,-1, 1, 1, -1};
    ap_uint<MOD_BITS> strmMod[NUM_STRM];

    // Get the modulation type
    for (int i = 0; i < NUM_STRM; i++) {
        strmMod[i] = modType.read();
    }

    // Process all the payload streams
    for (int i = 0; i < NUM_PAYLOAD; i++) {
        for (int j = 0; j < NUM_STRM; j++) {
            for (int k = 0; k < NUM_USED_SUBC; k++) {
                if (k == pilotLoc[0]) {
                    if (pnSeq[i] == 1) {
                        tmp_datPilot = dataPilot_Pos[j][0];
                    } else {
                        tmp_datPilot = dataPilot_Neg[j][0];                       
                    }
                    dataSymbol[j].write(tmp_datPilot);
                } else if (k == pilotLoc[1]) {
                    if (pnSeq[i] == 1) {
                        tmp_datPilot = dataPilot_Pos[j][1];
                    } else {
                        tmp_datPilot = dataPilot_Neg[j][1];                        
                    }
                    dataSymbol[j].write(tmp_datPilot);
                } else {
                    tmp_datBit = dataBit[j].read();
                    //std::cout << "tmp_datBit = " << tmp_datBit << std::endl;
                    if (strmMod[j] == 0) {  // BPSK
                        tmp_datSymbol = dataBPSK[tmp_datBit];
                    } else if (strmMod[j] == 1) {  // QPSK
                        tmp_datSymbol = dataQPSK[tmp_datBit];
                    } else {  // 16QAM
                        tmp_datSymbol = data16QAM[tmp_datBit];
                    }
                    dataSymbol[j].write(tmp_datSymbol);                    
                }
            }
        }
    }
}

// Generate the data symbol for the frame
void formDataSymbol(ap_uint<1> userType, ap_uint<UE_BITS> ueIdx, ap_uint<MOD_BITS> modUE[NUM_UE], ap_uint<STRM_BITS> strmUE[NUM_STRM], 
                stream<uint8_t> dataMAC[NUM_UE], uint10_t dataMAC_len[NUM_UE], stream<comp17_t> dataSymbol[NUM_STRM])
{
    stream<uint8_t> dataMACOut[NUM_STRM];
    stream<uint10_t> strmLenOut[NUM_STRM];
    stream<uint2_t> modType2Split;
    stream<uint2_t> modType2Mod;
    stream<uint4_t> dataBits[NUM_STRM];

    loadMACData(dataMAC, dataMAC_len, userType, ueIdx, modUE, strmUE, 
                dataMACOut, strmLenOut, modType2Split);   
    splitMACData(dataMACOut, strmLenOut, modType2Split, dataBits, modType2Mod);
    modulateData(modType2Mod, dataBits, dataSymbol);
}
