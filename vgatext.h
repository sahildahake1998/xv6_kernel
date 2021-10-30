#include "util/io.h"
#include <stdlib.h>

namespace vgatext{

    static inline void writechar(int loc, uint8_t c, uint8_t bg, uint8_t fg, addr_t base){
		// mmio::write8 (base,loc,c);
		bg = bg * 16;
		uint8_t colour = bg + fg;
		// printf(bg);
		mmio::write8 (base,2*loc,c);
		mmio::write8 (base,2*loc + 1,colour);
    }
}//namespace vgatext
