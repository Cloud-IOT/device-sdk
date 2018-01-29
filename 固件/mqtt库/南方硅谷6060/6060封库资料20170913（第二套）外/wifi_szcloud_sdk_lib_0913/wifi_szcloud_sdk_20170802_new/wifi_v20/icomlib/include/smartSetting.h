
#ifndef _SMARTSETTING_H_
#define _SMARTSETTING_H_

#include "ssv_types.h"

void register_smart_feature(U8 slink_mode);
void init_smart_feature(U8 slink_mode);
void clear_smart_feature_buf(U8 slink_mode);
void smart_feature_rxdata(U8 slink_mode, U8 *data, U32 len) ATTRIBUTE_SECTION_SRAM;;

struct smart_feature {
  char *name;
  /** Initialize the smart feature */
  void (* init)(void);

  /** clear smart feature use buffer  */
  void (* clear)(void);

  /** rece raw packet */
  void (* rece)(U8 *data, U32 len);
};

#endif