import time
import network
import esp32_twt  # Custom C module compiled into firmware

# 1. Establish connection to a Wi-Fi 6 network (2.4GHz 802.11ax compatible router)
wlan = network.WLAN(network.STA_IF)
wlan.active(True)

print("Connecting to Wi-Fi network...")
wlan.connect("YOUR_WIFI6_SSID", "YOUR_WIFI6_PASSWORD")

while not wlan.isconnected():
    time.sleep(0.5)

print("Connected successfully! Station IP:", wlan.ifconfig())
time.sleep(2)

# 2. Activate TWT via the custom C module
# Interval calculation: (2^exponent) * mantissa microseconds
# Using exponent = 12 and mantissa = 20000:
# (2^12) * 20000 us = 4096 * 20000 us = 81,920,000 us = ~82 seconds sleep interval

print("Applying advanced TWT configuration parameters - microcontroller entering power-save mode...")
esp32_twt.setup(
    12, 
    20000, 
    esp32_twt.FLOW_ANNOUNCED, 
    esp32_twt.TRIGGER_ENABLE
)

# 3. Main testing loop for current consumption analysis
# The device maintains a logical Wi-Fi connection, but the radio and CPU 
# automatically fall into a low-power state, waking up briefly every ~82 seconds.
print("Test loop active. Begin your current consumption measurements now!")
while True:
    print("Device connection health status:", wlan.isconnected())
    # During this sleep interval, the chip defaults to Light Sleep to minimize current draw
    time.sleep(10)
