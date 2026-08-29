#include "py/runtime.h"
#include "py/obj.h"
#include "esp_wifi.h"
#include "esp_pm.h"
#include "esp_log.h"

// Function to enable TWT and configure sleep parameters
STATIC mp_obj_t esp32_twt_setup(mp_obj_t wake_int_exponent_obj, mp_obj_t wake_int_mantissa_obj) {
    // Extract integer values passed from MicroPython arguments
    uint8_t wake_int_exponent = mp_obj_get_int(wake_int_exponent_obj);
    uint16_t wake_int_mantissa = mp_obj_get_int(wake_int_mantissa_obj);

    // 1. Enable automatic modem sleep (Modem Sleep / Light Sleep)
    #if CONFIG_PM_ENABLE
    esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = 40, // Reduce CPU frequency during idle states
        .light_sleep_enable = true
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    #endif

    // 2. Configure the Target Wake Time (TWT) setup structure
    // Wake interval formula: (2^exponent) * mantissa in microseconds
    wifi_twt_setup_config_t twt_config = {
        .setup_cmd = TWT_SETUP_REQUEST,
        .flow_id = 0, // Data stream identifier
        .twt_id = 0,
        .wake_invl_exp = wake_int_exponent,
        .wake_invl_mantis = wake_int_mantissa,
        .wake_duration = 255, // Nominal wake duration slot (in TU units ~1024us)
    };

    // 3. Send the TWT setup request frame to the Access Point (AP)
    esp_err_t err = esp_wifi_twt_setup(&twt_config);
    if (err != ESP_OK) {
        mp_raise_ValueError("TWT configuration failed. Verify that the router supports Wi-Fi 6 (802.11ax).");
    }

    mp_print_str(&mp_plat_print, "TWT setup request successfully sent to the AP.\n");
    return mp_const_none;
}
// Define the MicroPython function object accepting 2 positional arguments
STATIC MP_DEFINE_CONST_FUN_OBJ_2(esp32_twt_setup_obj, esp32_twt_setup);

// Map C functions to names visible within the MicroPython script environment
STATIC const mp_rom_map_elem_t esp32_twt_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_esp32_twt) },
    { MP_ROM_QSTR(MP_QSTR_setup), MP_ROM_PTR(&esp32_twt_setup_obj) },
};
STATIC MP_DEFINE_CONST_DICT(esp32_twt_module_globals, esp32_twt_module_globals_table);

// Define the module structure
const mp_obj_module_t esp32_twt_user_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&esp32_twt_module_globals,
};

// Register the module under the namespace "esp32_twt"
MP_REGISTER_MODULE(MP_QSTR_esp32_twt, esp32_twt_user_module);
