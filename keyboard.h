#pragma once

#include "generated/lpc_kbd.dev.h"

namespace lpc_kbd{

  static inline bool has_key(lpc_kbd_t& dev){
  	// the type of status reg is .... and dev is mem location of the device
  	lpc_kbd_status_t value = lpc_kbd_status_rd(&dev);
  	if (lpc_kbd_status_obf_extract(value)) return true;
  	else return false;
  }

  static inline uint8_t get_key(lpc_kbd_t& dev){
  	// the key we get here is the mem location of input buffer which is where wehave stored input data
  	uint8_t key = lpc_kbd_input_rd(&dev);
  	return key;
  }

}// namespace lpc_kbd

