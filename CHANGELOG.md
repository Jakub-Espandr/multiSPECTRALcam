# Changelog

## [1.0.1] - 2025-07-02
### main.ino
- Improved camera trigger logic: D2 is now LOW by default and only set HIGH for a short pulse when a specified number of consecutive zeros is detected on A3 (robust filtering against noise).
- Incremented lightCount only once per valid trigger event.
- Ensured D2 is held LOW during all initialization and animation screens.
- All relevant variables (counters, flags) are now global and declared at the top of the file.

### cam.py
- Changed trigger callback to respond to RISING_EDGE (transition from 0 to 1) on TRIGGER_PIN, instead of FALLING_EDGE. This means the camera now takes a photo when the pin goes from LOW to HIGH. 

---

## [1.0.0] - 2024-10-28
### main.ino
- Basic camera trigger logic: monitored A3 (light sensor) and controlled D2 to signal the camera based on light changes.
- Initial implementation of light counting and display updates.
- Included OLED display initialization and status screens.

### cam.py
- Listened for trigger signal on TRIGGER_PIN to capture a photo using libcamera-still.
- Saved photos to USB drive.
- Included shutdown functionality via SHUTDOWN_PIN.
- Used built-in LED for status indication.