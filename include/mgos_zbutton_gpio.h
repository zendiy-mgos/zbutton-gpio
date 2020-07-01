/*
 * Copyright (c) 2020 ZenDIY
 * All rights reserved
 *
 * Licensed under the Apache License, Version 2.0 (the ""License"");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an ""AS IS"" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MGOS_ZBUTTON_GPIO_H_
#define MGOS_ZBUTTON_GPIO_H_

#include <stdio.h>
#include <stdbool.h>
#include "mgos_zbutton.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MGOS_ZBUTTON_GPIO_DEFAULT_DEBOUNCE_TICKS 50 //milliseconds

#define MGOS_ZBUTTON_GPIO_CFG {true, MGOS_ZBUTTON_GPIO_DEFAULT_DEBOUNCE_TICKS} 
struct mgos_zbutton_gpio_cfg {
  bool active_high;
  int debounce_ticks;
};

bool mgos_zbutton_gpio_attach(struct mgos_zbutton *handle, int pin,
                              struct mgos_zbutton_gpio_cfg *cfg);

bool mgos_zbutton_gpio_detach(struct mgos_zbutton *handle);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MGOS_ZBUTTON_GPIO_H_ */
