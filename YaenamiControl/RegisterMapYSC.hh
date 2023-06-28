#ifndef REGISTER_MAP_YSC_HH
#define REGISTER_MAP_YSC_HH

namespace LBUS{
  // YaenamiSpiControl (YCS)  -----------------------------------------------------------------
  // Address space 0x0---'---- is reserved by YSC in all the firmwares.
  namespace YSC{
    enum LocalAddress
      {
	kAddrWriteFifo  = 0x0000'0000, // W, [7:0]
	// Adddress range is 0x0000'0000-0x0030'0000. It corresponds to ASIC #0-3
	kAddrBusyFlag   = 0x0200'0000, // R, [3:0] If the bit is high, the register transfer sequence is running.
	// Bit position corresponds to the ASIC number.
      
	kAddrStartCycle = 0x0300'0000  // W, [3:0] Start the register transfer cycle.
	// Bit position corresponds to the ASIC number.
     
      };
  };
};

#endif
