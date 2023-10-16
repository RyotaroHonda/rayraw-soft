#include <iostream>
#include <unistd.h>

#include "RegisterMap.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"
#include "MhTdcFuncs.hh"
#include "AdcFuncs.hh"
#include "DaqFuncs.hh"

enum kArgIndex{kBin, kIp, kRunNo, kNumEvent, kWinMax, kWinMin};

using namespace LBUS;

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "run_daq [IP address] [Run No] [Num of events] [Tdc window max] [Tdc window min]" << std::endl;
    std::cout << " - Run No         : Run number" << std::endl;
    std::cout << " - Num of events  : Max event number. DAQ will stop at this event number or by Ctrl-C." << std::endl;
    std::cout << " - Tdc window max : Max TDC window value. (10 ns step)" << std::endl;
    std::cout << " - Tdc window min : Min TDC window value. (10 ns step)" << std::endl;
    
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip   = argv[kIp];
  int32_t     run_no     = atoi(argv[kRunNo]);
  int32_t     num_event  = atoi(argv[kNumEvent]);
  int32_t     window_max = atoi(argv[kWinMax]);
  int32_t     window_min = atoi(argv[kWinMin]);

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  // Release AdcRo reset
  if(1 == fpga_module.ReadModule(ADC::kAddrAdcRoReset)){
    fpga_module.WriteModule(ADC::kAddrAdcRoReset, 0);
  }

  // Set trigger path //
  uint32_t reg_trg = TRM::kRegL1Ext;
  fpga_module.WriteModule(TRM::kAddrSelectTrigger, reg_trg);

  // Set NIM-IN //
  fpga_module.WriteModule(IOM::kAddrExtL1, IOM::kReg_i_Nimin1);

  // Set TDC window //
  HUL::DAQ::SetTdcWindow(window_max, window_min, fpga_module);

  // Enable TDC block //
  uint32_t en_block = TDC::kEnLeading | TDC::kEnTrailing;
  fpga_module.WriteModule(TDC::kAddrEnableBlock, en_block);

  // Set ADC window (same as TDC) //
  HUL::DAQ::SetAdcWindow(window_max, window_min, fpga_module);

  // Resest event counter //
  fpga_module.WriteModule(DCT::kAddrResetEvb, 0);

  // AdcRo initialize status
  std::cout << "#D: AdcRo IsReady status: " << std::hex << fpga_module.ReadModule(ADC::kAddrAdcRoIsReady) << std::dec << std::endl;

  // Event Read Cycle //
  HUL::DAQ::DoTrgDaq(board_ip, run_no, num_event, DCT::kAddrDaqGate);
  
  return 0;

}// main
