# ZenButton GPIO
## Overview
Mongoose-OS library for attaching ZenButtons to gpio-based pushbuttons.
## GET STARTED
Build up your own device in few minutes just starting from one of the following samples.

|Sample|Notes|
|--|--|
|[zbutton-mqtt-demo](https://github.com/zendiy-mgos/zbutton-mqtt-demo)|Mongoose-OS demo firmware for publishing ZenButtons events as MQTT messages.|
## Usage
Include the library into your `mos.yml` file.
```yaml
libs:
  - origin: https://github.com/zendiy-mgos/zbutton-gpio
```
If you are developing a JavaScript firmware, load `api_zbutton_gpio.js` in your `init.js` file.
```js
load('api_zbutton_gpio.js');
```
## C/C++ API Reference
### mgos_zbutton_gpio_cfg
```c
struct mgos_zbutton_gpio_cfg {
  bool active_high;
  int debounce_ticks;
};
```
GPIO configuration settings (e.g.: used by `mgos_zbutton_gpio_attach()`).

|Field||
|--|--|
|active_high|Set to `true` if the GPIO input is high (1) when the button is pressed.|
|debounce_ticks|Debounce interval in milliseconds. Set to -1 or to MGOS_ZBUTTON_GPIO_DEFAULT_DEBOUNCE_TICKS to use the default interval (50ms).|
### mgos_zbutton_gpio_attach()
```c
bool mgos_zbutton_gpio_attach(struct mgos_zbutton *handle, int pin, struct mgos_zbutton_gpio_cfg *cfg);
```
Attaches the button to the GPIO. Returns `true` on success, `false` otherwise.

|Parameter||
|--|--|
|handle|Button handle.|
|pin|GPIO pin.|
|cfg|Optional. GPIO configuration. If `NULL`, default configuration values are used.|

**Example** - Create a button using default configuration values and attach it to the GPIO 14.
```c
struct mgos_button *btn = mgos_button_create("btn-1", NULL);
// active_high    => equals to true
// debounce_ticks => equals to MGOS_ZBUTTON_GPIO_DEFAULT_DEBOUNCE_TICKS
mgos_zbutton_gpio_attach(btn, 14, NULL);
}
```
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
|cfg|object|Optional. GPIO configuration. If missing, default configuration values are used. For more details see *'GPIO configuration properties'* below.|

**GPIO configuration properties**
```js
{
  activeHigh: true,
  debounceTicks: 50
}
```
|Property|Type||
|--|--|--|
|activeHigh|boolean|Optional. Set to `true` if the GPIO input is high (1) when the button is pressed. Default value `true`.|
|debounceTicks|numeric|Optional. Debounce interval in milliseconds. Default value 50ms.|

**Example** - Create a button using default configuration values and attach it to the GPIO 14.
```js
let btn = ZenButton.create('btn-1');
let success = btn.GPIO.attach(14);
```
### .GPIO.detach()
```js
let success = btn.GPIO.detach();
```
Detaches the button from the GPIO that was previously attached using `.GPIO.attach()`. Returns `true` on success, `false` otherwise.
## Additional resources
Take a look to some other samples or libraries.

|Reference|Type||
|--|--|--|
|[zbutton-mqtt](https://github.com/zendiy-mgos/zbutton-mqtt)|Library|Mongoose-OS library for publishing ZenButtons events as MQTT messages.|
