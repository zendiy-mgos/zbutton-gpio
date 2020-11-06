# ZenButton GPIO
## Overview
Mongoose-OS library for attaching a [ZenButton](https://github.com/zendiy-mgos/zbutton) to a gpio-based pushbutton.
## GET STARTED
Build up your own device in few minutes just starting from the following sample. Start including following libraries into your `mos.yml` file.
```yaml
libs:
  - origin: https://github.com/zendiy-mgos/zbutton-gpio
```
**C/C++ sample code**
```c
#include "mgos.h"
#include "mgos_zbutton_gpio.h"

enum mgos_app_init_result mgos_app_init(void) {
  /* Create button using defualt configuration. */
  struct mgos_zbutton_cfg cfg = MGOS_ZBUTTON_CFG;
  struct mgos_zbutton *btn1 = mgos_zbutton_create("btn1", &cfg);
  
  if (btn1) {
    /* Attach button to GPIO 14. */
    struct mgos_zbutton_gpio_cfg gpio_cfg = MGOS_ZBUTTON_GPIO_CFG;  
    if (mgos_zbutton_gpio_attach(btn1, 14, &gpio_cfg)) {
      return MGOS_APP_INIT_SUCCESS;
    }
    mgos_zbutton_close(btn1);
  }
  return MGOS_APP_INIT_ERROR;
}
```
**JavaScript sample code**

```js
load("api_zbutton_gpio.js")

/* Create button using defualt configuration. */
let btn1 = ZenButton.create('btn1');

if (btn1) {
  /* Attach button to GPIO 14. */
  if (!btn1.GPIO.attach(14)) {
    btn1.close();
  }
}
```
## C/C++ API Reference
### mgos_zbutton_gpio_cfg
```c
struct mgos_zbutton_gpio_cfg {
  bool active_high;
};
```
GPIO configuration settings (e.g.: used by `mgos_zbutton_gpio_attach()`).

|Field||
|--|--|
|active_high|Set to `true` if the GPIO input is high (1) when the button is pressed.|

**Example** - Create and initialize configuration settings.
```c
// create and initialize cfg using defaults
struct mgos_zbutton_gpio_cfg cfg = MGOS_ZBUTTON_GPIO_CFG;
```
### mgos_zbutton_gpio_attach()
```c
bool mgos_zbutton_gpio_attach(struct mgos_zbutton *handle, int pin, struct mgos_zbutton_gpio_cfg *cfg);
```
Attaches the button to the GPIO. Returns `true` on success, `false` otherwise.

|Parameter||
|--|--|
|handle|Button handle.|
|pin|GPIO pin.|
|cfg|Optional. [GPIO configuration](https://github.com/zendiy-mgos/zbutton-gpio#mgos_zbutton_gpio_cfg). If `NULL`, default configuration values are used.|
### mgos_zbutton_gpio_detach()
```c
bool mgos_zbutton_gpio_detach(struct mgos_zbutton *handle);
```
Detaches the button from the GPIO that was previously attached using `mgos_zbutton_gpio_attach()`. Returns `true` on success, `false` otherwise.

|Parameter||
|--|--|
|handle|Button handle.|
## JS API Reference
### .GPIO.attach()
```js
let success = btn.GPIO.attach(pin, cfg);
```
Attaches the button to the GPIO. Returns `true` on success, `false` otherwise.

|Parameter|Type||
|--|--|--|
|pin|numeric|GPIO pin.|
|cfg|object|Optional. GPIO configuration. If missing, default configuration values are used. For more details, see 'GPIO configuration properties' section below.<br><br>{&nbsp;activeHigh: true&nbsp;}|

**GPIO configuration properties**
|Property|Type||
|--|--|--|
|activeHigh|boolean|Optional. Set to `true` if the GPIO input is high (1) when the button is pressed. Default value `true`.|
### .GPIO.detach()
```js
let success = btn.GPIO.detach();
```
Detaches the button from the GPIO that was previously attached using `.GPIO.attach()`. Returns `true` on success, `false` otherwise.
## Additional resources
Take a look to some other samples or libraries.

|Reference|Type||
|--|--|--|
|[zbutton-mqtt](https://github.com/zendiy-mgos/zbutton-mqtt)|Library|Mongoose-OS library for publishing [ZenButton](https://github.com/zendiy-mgos/zbutton) events as MQTT messages.|
|[zbutton-mqtt-demo](https://github.com/zendiy-mgos/zbutton-mqtt-demo)|Firmware|[Mongoose-OS](https://mongoose-os.com/) demo firmware for publishing pushbutton events as MQTT messages.|
