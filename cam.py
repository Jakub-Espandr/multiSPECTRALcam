import os
import time
import pigpio
import subprocess
from datetime import datetime
import shutil
import sys

# Define constants
TRIGGER_PIN = 27        # GPIO pin for trigger button
SHUTDOWN_PIN = 23       # GPIO pin for shutdown button
OUTPUT_PIN = 22         # GPIO pin to set HIGH at start
LED_PATH = "/sys/class/leds/ACT/brightness"  # Path to control the built-in LED
USB_MOUNT_PATH = "/media/usb"
RAM_TEMP_PATH = "/dev/shm/photo.jpg"  # Temporary storage in RAM

# Initialize pigpio library and set OUTPUT_PIN to HIGH immediately
pi = pigpio.pi()
pi.set_mode(OUTPUT_PIN, pigpio.OUTPUT)
pi.write(OUTPUT_PIN, 1)  # Set OUTPUT_PIN to HIGH right at the start

pi.set_mode(TRIGGER_PIN, pigpio.INPUT)
pi.set_pull_up_down(TRIGGER_PIN, pigpio.PUD_UP)

pi.set_mode(SHUTDOWN_PIN, pigpio.INPUT)
pi.set_pull_up_down(SHUTDOWN_PIN, pigpio.PUD_UP)

# Initialize last press time for debounce
last_press_time = 0

# Function to turn LED on or off
def led_on():
    with open(LED_PATH, 'w') as f:
        f.write('1')

def led_off():
    with open(LED_PATH, 'w') as f:
        f.write('0')

# Turn on LED initially
led_on()

# Function to mount the USB drive with write permissions
def mount_usb():
    if not os.path.exists(USB_MOUNT_PATH):
        os.makedirs(USB_MOUNT_PATH)
    if not os.path.ismount(USB_MOUNT_PATH):
        subprocess.run(['sudo', 'umount', '-l', USB_MOUNT_PATH], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        subprocess.run(['sudo', 'mount', '-o', 'uid=raspi,gid=raspi,umask=000', '/dev/sda1', USB_MOUNT_PATH],
                       stdout=subprocess.PIPE, stderr=subprocess.PIPE)

# Mount USB at script start
mount_usb()

# Function to capture an image
def capture_image(filename):
    temp_image_path = f"/dev/shm/{filename}.jpg"
    usb_image_path = os.path.join(USB_MOUNT_PATH, os.path.basename(temp_image_path))

    # Capture image in RAM
    subprocess.run([
        "libcamera-still",
        "-o", temp_image_path,
        "--width", "3264",   # Set resolution as desired
        "--height", "2448",
        "--awb", "daylight",
        "-t", "1",           # Immediate capture
        "-n"
    ])

    # Move image from RAM to USB
    try:
        shutil.move(temp_image_path, usb_image_path)
        print(f"Photo saved to USB as {usb_image_path}")
    except Exception as e:
        print(f"Error saving photo to USB: {e}")

# Function to handle button press for capturing regular photos
def capture_on_trigger(gpio, level, tick):
    # Turn off LED when trigger is detected
    led_off()

    # Capture the photo
    timestamp = datetime.now().strftime("IMG_%Y%m%d_%H%M%S")
    capture_image(timestamp)

    # Turn LED back on after saving the photo
    led_on()

# Function to handle shutdown with debounce
def shutdown_handler(gpio, level, tick):
    global last_press_time
    current_time = time.time()
    if current_time - last_press_time > 0.2:  # 200 ms debounce
        last_press_time = current_time
        print("Shutdown signal received. Completing tasks...")
        pi.write(OUTPUT_PIN, 0)  # Set OUTPUT_PIN to LOW
        time.sleep(1)  # Ensure any pending tasks complete
        print("Forcing Raspberry Pi shutdown.")
        subprocess.run(['sudo', 'shutdown', '-h', 'now'])  # Force immediate shutdown

# Set up callbacks for button presses
pi.callback(TRIGGER_PIN, pigpio.FALLING_EDGE, capture_on_trigger)
pi.callback(SHUTDOWN_PIN, pigpio.FALLING_EDGE, shutdown_handler)

try:
    print("Camera ready. Waiting for trigger...")
    while True:
        time.sleep(0.1)  # Reduce CPU load
except KeyboardInterrupt:
    print("Program terminated by user.")
finally:
    pi.stop()
