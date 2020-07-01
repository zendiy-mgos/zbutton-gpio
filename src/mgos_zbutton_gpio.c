#include "mgos.h"
#include "mgos_gpio.h"
#include "common/queue.h"
#include "mgos_zbutton_gpio.h"


extern int mg_zbutton_debounce_ticks_reset(struct mgos_zbutton *handle);
extern bool mg_zbutton_debounce_ticks_set(struct mgos_zbutton *handle, int debounce_ticks);

struct mg_zbutton_gpio_entry {
  struct mgos_zbutton *handle;
  int pin;
  int btn_debounce_ticks;
  struct mgos_zbutton_gpio_cfg cfg;
  SLIST_ENTRY(mg_zbutton_gpio_entry) entry;
};

/* Execution context */
struct mg_zbutton_gpio_ctx {
  int version;
  SLIST_HEAD(entries, mg_zbutton_gpio_entry) entries;
};

/* Excecution context instance */
static struct mg_zbutton_gpio_ctx *s_context = NULL;


void mg_zbutton_gpio_button_handler_cb(int pin, void *arg) {
  struct mg_zbutton_gpio_entry *entry = (struct mg_zbutton_gpio_entry *)arg;
  bool gpio_val = mgos_gpio_read(pin);  
  if (entry->cfg.active_high) {
    LOG(LL_DEBUG, ("Triggering button %s on pin %d.", gpio_val ? "DOWN" : "UP", pin));
    mgos_event_trigger(gpio_val ? MGOS_EV_ZBUTTON_DOWN : MGOS_EV_ZBUTTON_UP, entry->handle);
  } else {
    LOG(LL_DEBUG, ("Triggering button %s on pin %d.", gpio_val ? "UP" : "DOWM", pin));
    mgos_event_trigger(gpio_val ? MGOS_EV_ZBUTTON_UP : MGOS_EV_ZBUTTON_DOWN, entry->handle);
  }
}

struct mg_zbutton_gpio_entry *mg_zbutton_gpio_entry_get(struct mgos_zbutton *handle) {
  struct mg_zbutton_gpio_entry *e;
  SLIST_FOREACH(e, &s_context->entries, entry) {
    if (((void *)e->handle) == ((void *)handle)) return e;
  }
  return NULL;
}

bool mg_zbutton_gpio_entry_reset(struct mg_zbutton_gpio_entry *entry) {
  // Remove interrupt's handler
  mgos_gpio_remove_int_handler(entry->pin, NULL, NULL);
  // Clear interrupt
  mgos_gpio_clear_int(entry->pin);
  return true;
}

bool mg_zbutton_gpio_entry_set(struct mg_zbutton_gpio_entry *entry) {
  bool success = mgos_gpio_set_button_handler(entry->pin, 
    (entry->cfg.active_high ? MGOS_GPIO_PULL_DOWN: MGOS_GPIO_PULL_UP),
    MGOS_GPIO_INT_EDGE_ANY, entry->cfg.debounce_ticks,
    mg_zbutton_gpio_button_handler_cb, entry);

  if (!success) {
    mg_zbutton_gpio_entry_reset(entry);
    LOG(LL_ERROR, ("Error registering gpio button handler for '%s'.", entry->handle->id));
  } else {
    LOG(LL_INFO, ("Button handler on pin %d(pulled %s) for '%s' successfully configured.",
      entry->pin, (entry->cfg.active_high ? "DOWN" : "UP"), entry->handle->id));
  }
  return success;
}

bool mgos_zbutton_gpio_attach(struct mgos_zbutton *handle, int pin,
                              struct mgos_zbutton_gpio_cfg *cfg) {
  if (handle == NULL || pin < 0) return false;
  struct mg_zbutton_gpio_entry *e = calloc(1,
    sizeof(struct mg_zbutton_gpio_entry));
  if (e != NULL) {
    e->handle = handle;
    e->cfg.active_high = (cfg == NULL ? true : cfg->active_high);
    e->cfg.debounce_ticks = (cfg == NULL ? MGOS_ZBUTTON_GPIO_DEFAULT_DEBOUNCE_TICKS : cfg->debounce_ticks);
    e->pin = pin;
    
    if (mg_zbutton_gpio_entry_set(e)) {
      SLIST_INSERT_HEAD(&s_context->entries, e, entry);
      LOG(LL_INFO, ("Button '%s' successfully attacched to GPIO pin %d.",
        e->handle->id, e->pin));
    } else {
      free(e);
      e = NULL;
    }
  }
  
  return (e != NULL);
}

bool mgos_zbutton_gpio_detach(struct mgos_zbutton *handle) {
  struct mg_zbutton_gpio_entry *e = mg_zbutton_gpio_entry_get(handle);
  if (e != NULL && mg_zbutton_gpio_entry_reset(e)) {
    SLIST_REMOVE(&s_context->entries, e, mg_zbutton_gpio_entry, entry);
    free(e);
    return true;
  }
  return false;
}

#ifdef MGOS_HAVE_MJS

struct mgos_zbutton_gpio_cfg *mjs_zbutton_gpio_cfg_create(bool active_high,
                                                          int debounce_ticks) {
  struct mgos_zbutton_gpio_cfg *cfg = calloc(1,
    sizeof(struct mgos_zbutton_gpio_cfg));
  cfg->active_high = active_high;
  cfg->debounce_ticks = (debounce_ticks < 0 ? MGOS_ZBUTTON_GPIO_DEFAULT_DEBOUNCE_TICKS : debounce_ticks); 
  return cfg;
}

#endif /* MGOS_HAVE_MJS */


bool mgos_zbutton_gpio_init() {
  /* Create the context */
  s_context = calloc(1, sizeof(struct mg_zbutton_gpio_ctx));
  if (!s_context) return false;
  
  /* Initialize the context */
  s_context->version = 1;
  SLIST_INIT(&s_context->entries);
  
  return true;
}