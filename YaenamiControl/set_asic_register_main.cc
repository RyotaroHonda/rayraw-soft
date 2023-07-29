#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>

#include "RegisterMapYSC.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"
#include "Utility.hh"

enum kArgIndex{kBin, kIp, kDirPath};

using namespace LBUS;

uint32_t GetAddrOffset(int32_t asic_num)
{
  const int32_t kShiftOffset = 20;
  return static_cast<uint32_t>(asic_num << kShiftOffset);
}

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "Please write how to use here" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip     = argv[kIp];
  std::string reg_dir_path = argv[kDirPath];

  // Read register file //
  std::string file_path = reg_dir_path + "YAENAMI_1-4.txt";
  std::ifstream ifs(file_path.c_str());
  if(!ifs.is_open()){
    std::string message = "No such file: " + file_path;
    Utility::PrintError("", message);
    return -1;
  }

  static const int32_t kNumAsic = 4;
  std::vector<uint8_t> asic_registers[kNumAsic];
    // {
    //   {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9}, // Registers for ASIC0
    //   {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9}, // Registers for ASIC1
    //   {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9}, // Registers for ASIC2
    //   {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9}  // Registers for ASIC3
    // };

  
  std::string reg_str;
  while(std::getline(ifs, reg_str)){
    std::istringstream iss(reg_str);
    uint32_t reg;
    iss >> std::hex >> reg;

    std::cout << std::hex << reg << std::endl;
    asic_registers[0].push_back(static_cast<uint8_t>(reg));
    asic_registers[1].push_back(static_cast<uint8_t>(reg));
    asic_registers[2].push_back(static_cast<uint8_t>(reg));
    asic_registers[3].push_back(static_cast<uint8_t>(reg));
  }

  
  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  uint32_t register_size = asic_registers[0].size();
  std::cout << "#D: Register size: " << std::dec << register_size << std::endl;

  for(int32_t index_asic = 0; index_asic<kNumAsic; ++index_asic){
    std::cout << "#D: Cycle for ASIC" << index_asic << std::endl;
    
    // Write registers to FIFOs
    fpga_module.WriteModule_nByte(YSC::kAddrWriteFifo + GetAddrOffset(index_asic),
				  &(asic_registers[index_asic].at(0)),
				  register_size
				  );

  
    std::cout << "#D: Registers are written into the FIFOs" << std::endl;

    // Select ASIC
    fpga_module.WriteModule(YSC::kAddrChipSelect, (1 << index_asic));
  
    // Send StartCycle signal to the register transfer sequencer.
    fpga_module.WriteModule(YSC::kAddrStartCycle, 1);
    std::cout << "#D: StartCycle signals are sent" << std::endl;

    // Check busy flags. If the read value is not zero, the sequencer is still running.
    while(0 != fpga_module.ReadModule(YSC::kAddrBusyFlag, 1)){
      std::cout << "#D: Sequencer is still running" << std::endl;
      sleep(1);
    };

  }
    
  std::cout << "#D: Everything done" << std::endl;
  
  return 0;

}// main
