/*
 * US配列のキーマップでJP配列のPCへ接続できるようにする
 */

#pragma once

#include <stdint.h>
#include "action.h"

bool process_usjp(uint16_t keycode, keyrecord_t* record);
