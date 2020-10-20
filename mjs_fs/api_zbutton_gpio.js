load('api_zbutton.js');

ZenButton.GPIO = {
  _att: ffi('bool mgos_zbutton_gpio_attach(void *, int, void *)'),
  _det: ffi('bool mgos_zbutton_gpio_detach(void *)'),
  _cfgc: ffi('void *mjs_zbutton_gpio_cfg_create(bool)'),
  
  _proto: {
    _button: null,
    _getHandle: function() {
      return this._button.handle;
    },

    // ## **`object.GPIO.attach(pin, cfg)`**
    //
    // Example:
    // ```javascript
    // if (btn.GPIO.attach(4, {activeHigh: false})) {
    //   /* success */
    // } else {
    //   /* error */
    // }
    // ```
    attach: function(pin, cfg) {
      let cfgo = null;
      if (cfg) {
        cfgo = ZenButton.GPIO._cfgc(
          ZenThing._getSafe(cfg.activeHigh, true)
        );
        if (cfgo === null) return null;
      }
      let result = ZenButton.GPIO._att(this._getHandle(), pin, cfgo);
      ZenThing._free(cfgo);
      return result;
    },

    // ## **`object.GPIO.detach()`**
    //
    // Example:
    // ```javascript
    // let success = btn.GPIO.detach();
    // ```
    detach: function() {
      return ZenButton.GPIO._det(this._getHandle());
    },
  }
};

ZenThing._onCreateSub(function(obj) {
  if (obj.type === ZenButton.THING_TYPE) {
    obj.GPIO = Object.create(ZenButton.GPIO._proto);
    obj.GPIO._button = obj;
  }
});