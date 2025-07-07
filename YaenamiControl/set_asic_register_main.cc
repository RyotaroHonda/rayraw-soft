#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <string_view>
#include <sstream>
#include <fstream>

#include "RegisterMapYSC.hh"
#include "FPGAModule.hh"
#include "UDPRBCP.hh"
#include "Utility.hh"

#define DEBUG 1

enum kArgIndex{kBin, kIp, kDirPath, kMode, kSizeArg};

using namespace LBUS;

uint32_t GetAddrOffset(int32_t asic_num)
{
  const int32_t kShiftOffset = 20;
  return static_cast<uint32_t>(asic_num << kShiftOffset);
}

std::string GenFilePath(std::string dir_path, std::string file_name)
{
  std::string file_path = dir_path + (dir_path.back() == '/' ? "" : "/");
  file_path = file_path + file_name;
  return file_path;
}

void
ExecSpiSequence(std::string board_ip, uint32_t chip_select, std::string file_path)
{
  std::ifstream ifs(file_path.c_str());
  if(!ifs.is_open()){
    std::string message = "No such file: " + file_path;
    Utility::PrintError("", message);
    std::exit(-1);
  }

  std::vector<uint8_t> asic_registers;
  
  std::string reg_str;
  while(std::getline(ifs, reg_str)){
    std::istringstream iss(reg_str);
    uint32_t reg;
    
#ifdef DEBUG
    iss >> std::hex >> reg;
#endif

    std::cout << std::hex << reg << std::endl;
    asic_registers.push_back(static_cast<uint8_t>(reg));
  }
  
  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  uint32_t register_size = asic_registers.size();
  std::cout << "#D: Register size: " << std::dec << register_size << std::endl;

  std::cout << "#D: Start a SPI transmission cycle" << std::endl;
    
  // Write registers to FIFOs
  fpga_module.WriteModule_nByte(YSC::kAddrWriteFifo,
				&(asic_registers.at(0)),
				register_size
				);

  
  std::cout << "#D: Registers are written into the FIFOs" << std::endl;

  // Select ASIC
  fpga_module.WriteModule(YSC::kAddrChipSelect, chip_select);
  
  // Send StartCycle signal to the register transfer sequencer.
  fpga_module.WriteModule(YSC::kAddrStartCycle, 1);
  std::cout << "#D: StartCycle signals are sent" << std::endl;

  // Check busy flags. If the read value is not zero, the sequencer is still running.
  while(0 != fpga_module.ReadModule(YSC::kAddrBusyFlag, 1)){
    std::cout << "#D: Sequencer is still running " << std::endl;
    sleep(1);
  };

  fpga_module.WriteModule(YSC::kAddrChipSelect, 0);

  return;
}

int main(int argc, char* argv[])
{
  if(kSizeArg != argc){
    std::cout << "Usage\n";
    std::cout << "set_asic_register [ip address] [Register file path] [Mode]" << std::endl;
    std::cout << "Mode:" << std::endl;
    std::cout << "- initialize : Execute the ASIC initialization process" << std::endl;
    std::cout << "- normal     : Change ASIC register setting" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip     = argv[kIp];
  std::string reg_dir_path = argv[kDirPath];
  std::string control_mode = argv[kMode];

  std::string_view kNormalMode = "normal";
  std::string_view kInitMode   = "initialize";
  if(true
     && control_mode != kNormalMode
     && control_mode != kInitMode
      )
    {
      std::string message = "No such the mode: " + control_mode;
      Utility::PrintError("", message);
      return -1;
    }

  // Normail mode //
  const int32_t kNumAsic = 4;
  if(control_mode == kNormalMode){
    const std::string file_name[kNumAsic] =
      {
	"YAENAMI_1-3.txt","YAENAMI_2-3.txt","YAENAMI_3-3.txt","YAENAMI_4-3.txt"
      };
    
    for( int32_t i = 0; i<kNumAsic; ++i){
      ExecSpiSequence(board_ip, (1 << i), GenFilePath(reg_dir_path, file_name[i]) );
    }
  }

  // ASIC initialize mode //
  const int32_t kNumInitSeq = 4;
  if(control_mode == kInitMode){
    const std::string file_name[kNumInitSeq] =
      {
	"YAENAMI_1-1.txt","YAENAMI_1-2.txt","YAENAMI_1-1.txt","YAENAMI_1-3.txt"
      };
    
    for( int32_t i = 0; i<kNumInitSeq; ++i){
      ExecSpiSequence(board_ip, 0xf, GenFilePath(reg_dir_path, file_name[i]) );
      sleep(1);
    }
  }

  std::cout << "#D: Everything done" << std::endl;
  
  return 0;

}// main
