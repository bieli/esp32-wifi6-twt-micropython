#include "py/runtime.h"
#include "py/obj.h"
#include "esp_wifi.h"
#include "esp_pm.h"
#include "esp_log.h"

// Refactored TWT setup function using the modern esp_wifi_twt_setup_param_t structure
STATIC mp_obj_t esp32_twt_setup(size_t n_args, const mp_obj_t *args) {
    // Expected arguments from MicroPython:
    // args[0] = exponent (int)
    // args[1] = mantissa (int)
    // args[2] = flow_type (int: 0 = Announced, 1 = Unannounced)
    // args[3] = trigger (int: 0 = Disabled, 1 = Enabled)
    
    uint8_t wake_int_exponent = mp_obj_get_int(args[0]);
    uint16_t wake_int_mantissa = mp_obj_get_int(args[1]);
    uint8_t flow_type = mp_obj_get_int(args[2]);
    uint8_t trigger_en = mp_obj_get_int(args[3]);

    // 1. Enable automatic modem sleep (Modem Sleep / Light Sleep)
    #if CONFIG_PM_ENABLE
    esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = 40, // Downclock CPU during idle states
        .light_sleep_enable = true
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    #endif

    // 2. Configure modern TWT parameters based on ESP-IDF v5.x specifications
    esp_wifi_twt_setup_param_t twt_setup_param = {
        .twt_id = 1,                                            // Session ID Identifier
        .flow_type = (flow_type == 0) ? TWT_FLOW_TYPE_ANNOUNCED : TWT_FLOW_TYPE_UNANNOUNCED,
        .trigger = (trigger_en == 1) ? TWT_TRIGGER_EN : TWT_TRIGGER_DISABLE,                
        .wake_invl_exp = wake_int_exponent,                     // Wake interval exponent
        .wake_invl_mant = wake_int_mantissa,                    // Wake interval mantissa
        .minimum_wake_duration = 255,                           // Minimum wake duration window
    };

    // 3. Register and send the negotiated TWT request to the Wi-Fi 6 AP
    esp_err_t err = esp_wifi_twt_setup(&twt_setup_param);
    if (err != ESP_OK) {
        mp_raise_ValueError("Modern TWT setup failed. Verify ESP-IDF version compatibility and router AX support.");
    }

    mp_print_str(&mp_plat_print, "Modern TWT setup request accepted and sent to AP.\n");
    return mp_const_none;
}
// Define the MicroPython function object accepting between 2 and 4 arguments
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_twt_setup_obj, 4, 4, esp32_twt_setup);

// Map constants to allow clean MicroPython scripts without magic numbers
STATIC const mp_rom_map_elem_t esp32_twt_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_esp32_twt) },
    { MP_ROM_QSTR(MP_QSTR_setup), MP_ROM_PTR(&esp32_twt_setup_obj) },
    // Flow Type Constants
    { MP_ROM_QSTR(MP_QSTR_FLOW_ANNOUNCED), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_FLOW_UNANNOUNCED), MP_ROM_INT(1) },
    // Trigger Constants
    { MP_ROM_QSTR(MP_QSTR_TRIGGER_DISABLE), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_TRIGGER_ENABLE), MP_ROM_INT(1) },
};
STATIC MP_DEFINE_CONST_DICT(esp32_twt_module_globals, esp32_twt_module_globals_table);

// Define the module structure
const mp_obj_module_t esp32_twt_user_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&esp32_twt_module_globals,
};

// Register the module under the namespace "esp32_twt"
MP_REGISTER_MODULE(MP_QSTR_esp32_twt, esp32_twt_user_module);
