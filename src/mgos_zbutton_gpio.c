#include "mgos.h"
#include "mgos_gpio.h"
#include "common/queue.h"
#include "mgos_zbutton_gpio.h"


struct mg_zbutton_gpio_entry {
  struct mgos_zbutton *handle;
  int pin;
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

bool mg_zbutton_gpio_cfg_set(struct mgos_zbutton_gpio_cfg *cfg_src,
                             struct mgos_zbutton_gpio_cfg *cfg_dest) {
  if (!cfg_dest) return false;
  if (cfg_src != NULL) {
    cfg_dest->active_high = cfg_src->active_high;
  } else {
    cfg_dest->active_high = true;
  }
  return true;
}

void mg_zbutton_gpio_button_handler_cb(int pin, void *arg) {
  struct mg_zbutton_gpio_entry *entry = (struct mg_zbutton_gpio_entry *)arg;
  bool gpio_val = mgos_gpio_read(pin);  
  if (entry->cfg.active_high) {
    mgos_zbutton_push_state_set(entry->handle, (gpio_val ? ZBUTTON_STATE_DOWN : ZBUTTON_STATE_UP));
    LOG(LL_DEBUG, ("Triggering button %s on pin %d ('%s').", gpio_val ? "DOWN" : "UP", pin, entry->handle->id));
  } else {
    mgos_zbutton_push_state_set(entry->handle, (gpio_val ? ZBUTTON_STATE_UP : ZBUTTON_STATE_DOWN));
    LOG(LL_DEBUG, ("Triggering button %s on pin %d ('%s').", gpio_val ? "UP" : "DOWM", pin, entry->handle->id));
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
  struct mgos_zbutton_cfg btn_cfg;
  mgos_zbutton_cfg_get(entry->handle, &btn_cfg);

  bool success = mgos_gpio_set_button_handler(entry->pin, 
    (entry->cfg.active_high ? MGOS_GPIO_PULL_DOWN: MGOS_GPIO_PULL_UP),
    MGOS_GPIO_INT_EDGE_ANY, btn_cfg.debounce_ticks,
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
    e->pin = pin;
    
    if (mg_zbutton_gpio_cfg_set(cfg, &e->cfg) && mg_zbutton_gpio_entry_set(e)) {
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

struct mgos_zbutton_gpio_cfg *mjs_zbutton_gpio_cfg_create(bool active_high) {
  struct mgos_zbutton_gpio_cfg cfg_src = {
    active_high
  };
  struct mgos_zbutton_gpio_cfg *cfg_dest = calloc(1, sizeof(struct mgos_zbutton_gpio_cfg));
  if (mg_zbutton_gpio_cfg_set(&cfg_src, cfg_dest)) return cfg_dest;
  free(cfg_dest);
  return NULL;
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