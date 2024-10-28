# multiSPECTRAL Camera

A custom multispectral imaging solution built on Raspberry Pi platform for aerial photography and agricultural monitoring.

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

---

## 🔐 License

This project is licensed under the **Non-Commercial Public License (NCPL v1.0)**  
© 2025 Jakub Ešpandr - Born4FLight, FlyCamCzech

See the [LICENSE](https://github.com/Jakub-Espandr/multiSPECTRALcam/raw/main/LICENSE) file for full terms.