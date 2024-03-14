#include "tx_frame.hpp"

// Add phase rotation to the PHY header and Payload data after modulation
void phaseRotation(stream<ap_int<PHY_HEADER_BITS>> phyHeader[NUM_STRM], stream<comp17_t> payloadData[NUM_STRM],
                   stream<comp16_t> phaseRotData[NUM_STRM]);