#include "util/io.h"

namespace vgatext{

   static inline void writechar(int loc, uint8_t c, uint8_t bg, uint8_t fg, addr_t base){
     //your code goes here
   	 uint8_t col_value = (bg << 4) + fg;
   	 mmio::write8(base,(2*loc),c);
   	 mmio::write8(base,2*loc+1,col_value);
   }
}//namespace vgatext
