#include "py/runtime.h"
#include "py/obj.h"
#include "esp_wifi.h"
#include "esp_pm.h"
#include "esp_log.h"

// Function to setup TWT using standard 'static' keyword compatible with modern MicroPython
static mp_obj_t esp32_twt_setup(size_t n_args, const mp_obj_t *args) {
    uint8_t wake_int_exponent = mp_obj_get_int(args[0]);
    uint16_t wake_int_mantissa = mp_obj_get_int(args[1]);
    uint8_t flow_type = mp_obj_get_int(args[2]);
    uint8_t trigger_en = mp_obj_get_int(args[3]);

    #if CONFIG_PM_ENABLE
    esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = 40,
        .light_sleep_enable = true
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    #endif

    esp_wifi_twt_setup_param_t twt_setup_param = {
        .twt_id = 1,
        .flow_type = (flow_type == 0) ? TWT_FLOW_TYPE_ANNOUNCED : TWT_FLOW_TYPE_UNANNOUNCED,
        .trigger = (trigger_en == 1) ? TWT_TRIGGER_EN : TWT_TRIGGER_DISABLE,                
        .wake_invl_exp = wake_int_exponent,                     
        .wake_invl_mant = wake_int_mantissa,                    
        .minimum_wake_duration = 255,                           
    };

    esp_err_t err = esp_wifi_twt_setup(&twt_setup_param);
    if (err != ESP_OK) {
        mp_raise_ValueError("Modern TWT setup failed. Verify router AX support.");
    }

    mp_print_str(&mp_plat_print, "Modern TWT setup request accepted and sent to AP.\n");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(esp32_twt_setup_obj, 4, 4, esp32_twt_setup);


// Function to get TWT statistics
static mp_obj_t esp32_twt_status(void) {
    wifi_twt_stats_t twt_stats;
    esp_err_t err = esp_wifi_twt_get_stats(1, &twt_stats);
    
    if (err != ESP_OK) {
        mp_raise_ValueError("Failed to fetch TWT statistics. Is the Wi-Fi connection active?");
    }

    mp_obj_t dict = mp_obj_new_dict(0);
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_status_code), mp_obj_new_int(twt_stats.status));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_tx_frames), mp_obj_new_int(twt_stats.twt_tx_frames_num));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_rx_frames), mp_obj_new_int(twt_stats.twt_rx_frames_num));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_succ_tx_frames), mp_obj_new_int(twt_stats.twt_succ_tx_frames_num));
    mp_obj_dict_store(dict, MP_OBJ_NEW_QSTR(MP_QSTR_succ_rx_frames), mp_obj_new_int(twt_stats.twt_succ_rx_frames_num));

    return dict;
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp32_twt_status_obj, esp32_twt_status);


// Function to teardown active TWT session
static mp_obj_t esp32_twt_teardown(void) {
    esp_err_t err = esp_wifi_twt_teardown(1);
    
    if (err != ESP_OK) {
        mp_raise_ValueError("Failed to teardown TWT session. Ensure a session is active.");
    }

    #if CONFIG_PM_ENABLE
    esp_pm_config_t pm_config = {
        .max_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .min_freq_mhz = CONFIG_ESP_DEFAULT_CPU_FREQ_MHZ,
        .light_sleep_enable = false
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_config));
    #endif

    mp_print_str(&mp_plat_print, "TWT session closed successfully. Power save mode disabled.\n");
    return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(esp32_twt_teardown_obj, esp32_twt_teardown);


// Module globals mapping table using standard modern syntax
static const mp_rom_map_elem_t esp32_twt_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_esp32_twt) },
    { MP_ROM_QSTR(MP_QSTR_setup), MP_ROM_PTR(&esp32_twt_setup_obj) },
    { MP_ROM_QSTR(MP_QSTR_status), MP_ROM_PTR(&esp32_twt_status_obj) },
    { MP_ROM_QSTR(MP_QSTR_teardown), MP_ROM_PTR(&esp32_twt_teardown_obj) },
    { MP_ROM_QSTR(MP_QSTR_FLOW_ANNOUNCED), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_FLOW_UNANNOUNCED), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_TRIGGER_DISABLE), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_TRIGGER_ENABLE), MP_ROM_INT(1) },
};
static MP_DEFINE_CONST_DICT(esp32_twt_module_globals, esp32_twt_module_globals_table);

const mp_obj_module_t esp32_twt_user_module = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t*)&esp32_twt_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_esp32_twt, esp32_twt_user_module);
