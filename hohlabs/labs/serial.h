#pragma once

#include "util/io.h"

namespace serial{

  static inline bool is_transmitter_ready(io_t baseport){
    //insert your code here
    uint8_t line_status = io::read8(baseport, 5);
    if(line_status & 0x08 == 0x00)
    	return false;
    return true;
  }

  static inline void writechar(uint8_t c, io_t baseport){
    //insert your code here
    io::write8(baseport,0,c);
  }

} //end serial
