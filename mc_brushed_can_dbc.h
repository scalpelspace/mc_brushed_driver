/*******************************************************************************
 * @file mc_brushed_can_dbc.h
 * @brief Auto-generated CAN message definitions from DBC file.
 *******************************************************************************
 */

#ifndef MC_BRUSHED_CAN_DBC_H
#define MC_BRUSHED_CAN_DBC_H

/** Includes. *****************************************************************/

#include "can_driver/can_driver.h"

/** CPP guard open. ***********************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/** DBC message index enum. **************************************************/

typedef enum {
  MC_BRUSHED_CAN_DBC_IDX_STATE = 0,
  MC_BRUSHED_CAN_DBC_IDX_COMMAND_BRUSHED_TORQUE = 1,
  MC_BRUSHED_CAN_DBC_IDX_COMMAND_BRUSHED_VELOCITY = 2,
  MC_BRUSHED_CAN_DBC_IDX_COMMAND_BRUSHED_POSITION = 3,
  MC_BRUSHED_CAN_DBC_IDX_ENCODER = 4,
  MC_BRUSHED_CAN_DBC_IDX_CONTROLS_TUNE = 5,
  MC_BRUSHED_CAN_DBC_IDX_CONTROLS_TUNE_GET = 6,
  MC_BRUSHED_CAN_DBC_IDX_CONTROLS_TUNE_GET_RESPONSE = 7,
  MC_BRUSHED_CAN_DBC_IDX_CONTROLS_DIAGNOSTIC = 8,
  MC_BRUSHED_CAN_DBC_IDX_DATETIME_SET = 9,
  MC_BRUSHED_CAN_DBC_IDX_DATETIME_GET = 10,
  MC_BRUSHED_CAN_DBC_IDX_DATETIME_GET_RESPONSE = 11,
  MC_BRUSHED_CAN_DBC_IDX_VERSION_GET = 12,
  MC_BRUSHED_CAN_DBC_IDX_VERSION_GET_RESPONSE = 13,

  MC_BRUSHED_CAN_DBC_IDX_COUNT // Total message count.
} mc_brushed_can_dbc_index_t;

/** Public variables. *********************************************************/

extern const can_message_t dbc_messages[];

/** CPP guard close. **********************************************************/

#ifdef __cplusplus
}
#endif

#endif // MC_BRUSHED_CAN_DBC_H
