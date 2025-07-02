import os
import time
import pigpio
import subprocess
from datetime import datetime
import shutil
import sys

# Konstanty
TRIGGER_PIN = 27
SHUTDOWN_PIN = 23
OUTPUT_PIN = 22
LED_PATH = "/sys/class/leds/ACT/brightness"
USB_MOUNT_PATH = "/media/usb"
RAM_TEMP_PATH = "/dev/shm/photo.jpg"

# Inicializace GPIO
pi = pigpio.pi()
pi.set_mode(OUTPUT_PIN, pigpio.OUTPUT)
pi.write(OUTPUT_PIN, 0)  # výchozí stav – LOW

pi.set_mode(TRIGGER_PIN, pigpio.INPUT)
pi.set_pull_up_down(TRIGGER_PIN, pigpio.PUD_UP)

pi.set_mode(SHUTDOWN_PIN, pigpio.INPUT)
pi.set_pull_up_down(SHUTDOWN_PIN, pigpio.PUD_UP)

last_press_time = 0

# LED ovládání
def led_on():
    with open(LED_PATH, 'w') as f:
        f.write('1')

def led_off():
    with open(LED_PATH, 'w') as f:
        f.write('0')

led_on()

# Najdi připojený USB oddíl
def find_usb_partition():
    try:
        result = subprocess.run(['lsblk', '-rno', 'NAME,TYPE'], capture_output=True, text=True)
        for line in result.stdout.strip().split('\n'):
            name, type_ = line.strip().split()
            if type_ == 'part' and name.startswith('sd'):
                return f"/dev/{name}"
    except Exception as e:
        print("Chyba při detekci USB oddílu:", e)
    return None

# Připojení USB disku
def mount_usb():
    if not os.path.exists(USB_MOUNT_PATH):
        os.makedirs(USB_MOUNT_PATH)

    usb_partition = find_usb_partition()
    if usb_partition:
        if not os.path.ismount(USB_MOUNT_PATH):
            result = subprocess.run(['sudo', 'mount', usb_partition, USB_MOUNT_PATH],
                                    stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            if result.returncode == 0:
                print("✅ USB disk připojen.")
                pi.write(OUTPUT_PIN, 1)  # zapnout výstupní pin jen při úspěchu
                return True
            else:
                print("❌ Připojení USB selhalo:", result.stderr.decode())
    else:
        print("⚠️ USB oddíl nebyl nalezen.")
    return False

# Pokus o připojení
usb_ready = mount_usb()

# Pořízení snímku
def capture_image(filename):
    if not usb_ready:
        print("⛔️ USB není připojeno. Snímek nebude uložen.")
        return

    temp_image_path = f"/dev/shm/{filename}.jpg"
    usb_image_path = os.path.join(USB_MOUNT_PATH, os.path.basename(temp_image_path))

    subprocess.run([
        "libcamera-still",
        "-o", temp_image_path,
        "--width", "3264",
        "--height", "2448",
        "--awb", "daylight",
        "--quality", "90",
        "-t", "1",
        "-n"
    ])

    try:
        shutil.move(temp_image_path, usb_image_path)
        print(f"📸 Snímek uložen na USB jako {usb_image_path}")
    except Exception as e:
        print(f"❌ Chyba při ukládání snímku: {e}")

# Reakce na stisk spouště
def capture_on_trigger(gpio, level, tick):
    led_off()
    timestamp = datetime.now().strftime("IMG_%Y%m%d_%H%M%S")
    capture_image(timestamp)
    led_on()

# Vypínací tlačítko
def shutdown_handler(gpio, level, tick):
    global last_press_time
    current_time = time.time()
    if current_time - last_press_time > 0.2:
        last_press_time = current_time
        print("📴 Signál k vypnutí přijat.")
        pi.write(OUTPUT_PIN, 0)
        time.sleep(1)
        #subprocess.run(['sudo', 'shutdown', '-h', 'now'])
        subprocess.run(['sudo', 'systemctl', 'poweroff'])

# Callbacky
pi.callback(TRIGGER_PIN, pigpio.RISING_EDGE, capture_on_trigger)
pi.callback(SHUTDOWN_PIN, pigpio.FALLING_EDGE, shutdown_handler)

# Hlavní smyčka
try:
    print("📷 Kamera připravena. Čekám na stisk tlačítka...")
    while True:
        time.sleep(0.1)
except KeyboardInterrupt:
    print("🛑 Program ukončen uživatelem.")
finally:
    pi.stop()
