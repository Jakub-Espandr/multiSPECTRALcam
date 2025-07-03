<p align="center">
  <a href="https://ibb.co/ZzBgp1Kv">
    <img src="https://i.ibb.co/fdkNzGtZ/FCCmulti-SPECTRAL.jpg" alt="multiSPECTRAL" width="500"/>
  </a>
</p>

<h1 align="center">FlyCamCzech multiSPECTRALcamera</h1>
<p align="center"><em>(Born4Flight | FlyCamCzech | Jakub Ešpandr)</em></p>

## 🎯 Overview

This project implements a modified camera system capable of capturing Near-Infrared (NIR) images using a Raspberry Pi HQ camera with custom sensor modifications. The system is specifically designed for drone integration with DJI Phantom 4 for agricultural and environmental monitoring applications.

## 🛠️ Hardware Components

### Core Components
- **Raspberry Pi Zero 2W**
- **Raspberry Pi HQ Camera** with modified sensor
- **Custom 3D printed frame** for mounting and protection
- **DJI Phantom 4 mounting solution**

### Sensor Modification
The standard Raspberry Pi HQ camera sensor has been modified to capture Near-Infrared (NIR) spectrum images by:
- Removing the IR-cut filter
- Installing custom filter for NIR wavelength isolation
- Optimizing sensor sensitivity for agricultural applications

## 🚁 DJI Phantom 4 Integration

### Mounting System
- Custom 3D printed gimbal adapter
- Secure attachment points designed for flight stability
- Cable management for power and data connections
- Vibration dampening for image quality

### Triggering Solutions
- **Drone status LED monitoring** using RGB sensor for automated capture
- **Optical triggering system** synchronized with drone operations

# multiSPECTRALcam - Raspberry Pi Camera Setup

## Table of Contents
1. [Project Overview](#project-overview)
2. [Prerequisites](#prerequisites)
3. [System Preparation](#system-preparation)
4. [Camera & GPIO Setup](#camera--gpio-setup)
5. [Deploying cam.py](#deploying-campy)
6. [Service Setup (Autostart)](#service-setup-autostart)
7. [Troubleshooting & Useful Commands](#troubleshooting--useful-commands)

---

## Project Overview
multiSPECTRALcam is a Raspberry Pi-based camera system using the IMX477 module and custom Python automation (`cam.py`). It is designed for robust, automated photo capture triggered by external hardware (e.g., Arduino/Nano). This guide walks you through the full installation and setup process.

---

## Prerequisites
- Raspberry Pi with **Raspberry Pi OS Lite (32 bit) - Debian Bookworm** (required)
- IMX477 camera module (or compatible)
- Sudo/root access
- Internet connection for package installation
- (Optional) USB drive for photo storage

---

## System Preparation

### 1. Update System Packages
```sh
sudo apt-get update
sudo apt-get upgrade -y
```

### 2. Install Python 3 and pip
```sh
sudo apt-get install python3 python3-pip -y
```

### 3. Install pigpio and Enable Daemon
`pigpio` is required for GPIO control from Python:
```sh
sudo apt-get install pigpio python3-pigpio -y
sudo systemctl enable pigpiod
sudo systemctl start pigpiod
```

### 4. Install libcamera
```sh
sudo apt-get install libcamera-apps -y
```

### 5. (Optional) Install xdotool
If you need to simulate keyboard/mouse input:
```sh
sudo apt-get install xdotool -y
```

### 6. (Optional) I2C Tools and Full Upgrade
For I2C device detection and system upgrade:
```sh
sudo apt-get install i2c-tools
sudo i2cdetect -y 1
sudo apt-get update
sudo apt-get full-upgrade
```

---

## Camera & GPIO Setup

### 1. Configure Camera Overlay
Edit the firmware config to disable auto-detect and enable the IMX477 camera overlay:
```sh
sudo nano /boot/firmware/config.txt
```
Add or ensure these lines are present:
```
camera_auto_detect=0
dtoverlay=imx477
```

### 2. (Optional) Edit Boot Command Line for Fast Boot
```sh
sudo nano /boot/firmware/cmdline.txt
```
Add or ensure:
```
quiet fastboot nosplash
```

### 3. Disable Unnecessary Services (Optional, for faster boot)
```sh
sudo systemctl disable ModemManager
sudo systemctl disable polkit
sudo systemctl disable avahi-daemon
sudo systemctl disable bluetooth
```

---

## Deploying cam.py

### 1. Copy cam.py to Raspberry Pi
The `cam.py` file is located in the `multiSPECTRALcam` directory of this repository.

Replace `raspiX` and `IP` with your actual username and IP address:
```sh
scp /path/to/multiSPECTRALcam/cam.py raspiX@IP:/home/raspiX/
# Example:
scp /path/to/multiSPECTRALcam/cam.py raspi2@10.15.20.102:/home/raspi2/
scp /path/to/multiSPECTRALcam/cam.py raspi1@10.15.20.138:/home/raspi1/
```

---

## Service Setup (Autostart)

### 1. Create a systemd Service for cam.py
Create and edit the service file:
```sh
sudo nano /etc/systemd/system/cam.service
```
Paste the following (replace `raspiX` with your username):
```
[Unit]
Description=Autostart cam.py
After=network.target

[Service]
ExecStart=/usr/bin/python3 /home/raspiX/cam.py
WorkingDirectory=/home/raspiX
StandardOutput=inherit
StandardError=inherit
Restart=always
User=root

[Install]
WantedBy=multi-user.target
```

### 2. Enable and Start the Service
```sh
sudo systemctl daemon-reexec
sudo systemctl daemon-reload
sudo systemctl enable cam.service
sudo systemctl start cam.service
```

---

## Troubleshooting & Useful Commands

- **Check Service Status:**
  ```sh
  sudo systemctl status cam.service
  ```
- **View Service Logs:**
  ```sh
  journalctl -u cam.service -e
  ```
- **Allow Passwordless Shutdown (Optional, for shutdown from script):**
  ```sh
  sudo visudo
  # Add this line (replace raspiX with your username):
  raspiX ALL=(ALL) NOPASSWD: /sbin/shutdown
  ```

---

## 🔐 License

This project is licensed under the **Non-Commercial Public License (NCPL v1.0)**  
© 2025 Jakub Ešpandr - Born4FLight, FlyCamCzech

See the [LICENSE](https://github.com/Jakub-Espandr/multiSPECTRALcam/raw/main/LICENSE) file for full terms.
