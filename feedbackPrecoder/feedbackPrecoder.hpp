#include "tx_frame.hpp"

// The feedbackPrecoder function is used to apply feedback precoding to the
// transmitted data.
void feedbackPrecoder(stream<comp17_t>& matFDPrec, stream<comp16_t> phaseRotData[NUM_STRM], stream<comp16_t> fdPrecData[NUM_STRM]);