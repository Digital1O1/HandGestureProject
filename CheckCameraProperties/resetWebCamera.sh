#!/bin/bash

# Vendor and Product ID for Logitech C920 webcam
VENDOR_ID="046d"
PRODUCT_ID="08e5"

# Get the serial number of the device by matching Vendor and Product ID
DEVICE_SERIAL=$(lsusb -v | grep -B 3 "$VENDOR_ID:$PRODUCT_ID" | grep "iSerial" | awk '{print $3}')

if [ -z "$DEVICE_SERIAL" ]; then
    echo "Device not found or no serial number available."
    exit 1
fi

echo "Device Serial Number: $DEVICE_SERIAL"

# Find the device path by matching the serial number in /sys/bus/usb/devices/
DEVICE_SYS_PATH=$(find /sys/bus/usb/devices/ -name "*$DEVICE_SERIAL*" -type d)

if [ -z "$DEVICE_SYS_PATH" ]; then
    echo "Device path not found."
    exit 1
fi

echo "Found device at: $DEVICE_SYS_PATH"

# If needed, you can now unbind and rebind the device
echo "Unbinding device..."
echo "$DEVICE_SYS_PATH" | sudo tee /sys/bus/usb/drivers/usb/unbind

# Wait for a moment to ensure the device is unbound
sleep 2

echo "Binding device..."
echo "$DEVICE_SYS_PATH" | sudo tee /sys/bus/usb/drivers/usb/bind
