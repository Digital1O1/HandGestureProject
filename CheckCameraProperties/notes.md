# Camera Control Notes

## Note
- You can apply v4l2 commands in real time when using terminal

## Using v4l2-ctl --list-ctrls
- Lists out all available controls 
- Some features can't be turned off with OpenCV
- Best to use v4l2-ctl to control web camera parameters

## List all supported camera settings (Controls)
- Informed you of 
  - What can be changed
  - Their min/max values
  - Their current values
```bash
v4l2-ctl --device=/dev/video2 --list-ctrls
```

## List devices

```bash
v4l2-ctl --list-devices
```

## To check current camera settings
```bash

# This will list out enum for different modes like 
#Camera Controls

#                  auto_exposure 0x009a0901 (menu)   : min=0 max=3 default=3 value=1 (Manual Mode)
#				1: Manual Mode
#				3: Aperture Priority Mode

v4l2-ctl --device=/dev/video2 --all

# Or a specific setting
v4l2-ctl --device=/dev/video2 --get-ctrl=white_balance_automatic

```

```bash

User Controls

                     brightness 0x00980900 (int)    : min=0 max=255 step=1 default=128 value=96
                       contrast 0x00980901 (int)    : min=0 max=255 step=1 default=128 value=96
                     saturation 0x00980902 (int)    : min=0 max=255 step=1 default=128 value=96
        white_balance_automatic 0x0098090c (bool)   : default=1 value=1
                           gain 0x00980913 (int)    : min=0 max=255 step=1 default=0 value=255
           power_line_frequency 0x00980918 (menu)   : min=0 max=2 default=2 value=2 (60 Hz)
      white_balance_temperature 0x0098091a (int)    : min=2000 max=6500 step=1 default=4000 value=3588 flags=inactive
                      sharpness 0x0098091b (int)    : min=0 max=255 step=1 default=128 value=128
         backlight_compensation 0x0098091c (int)    : min=0 max=1 step=1 default=0 value=0

Camera Controls

                  auto_exposure 0x009a0901 (menu)   : min=0 max=3 default=3 value=3 (Aperture Priority Mode)
         exposure_time_absolute 0x009a0902 (int)    : min=3 max=2047 step=1 default=250 value=646 flags=inactive
     exposure_dynamic_framerate 0x009a0903 (bool)   : default=0 value=1
                   pan_absolute 0x009a0908 (int)    : min=-36000 max=36000 step=3600 default=0 value=0
                  tilt_absolute 0x009a0909 (int)    : min=-36000 max=36000 step=3600 default=0 value=0
                 focus_absolute 0x009a090a (int)    : min=0 max=250 step=5 default=0 value=35 flags=inactive
     focus_automatic_continuous 0x009a090c (bool)   : default=1 value=1
                  zoom_absolute 0x009a090d (int)    : min=100 max=500 step=1 default=100 value=100
```

## Examples to disable features 
### Disabling exposure_dynamic_framerate
```
v4l2-ctl --device=/dev/video2 --set-ctrl=exposure_dynamic_framerate=0
```
### Disabling Auto Exposure
```
v4l2-ctl --device=/dev/video2 --set-ctrl=auto_exposure=1
```
### Disabling Auto White Balance
```
v4l2-ctl --device=/dev/video2 --set-ctrl=white_balance_automatic=0
```

### To quickly verify if the changes were made
```
ffplay -f v4l2 -i /dev/video2
```

---

# Note : Don't use system() in real world applications
- It's slow and blocks the entire program
- Limited error handling
- Output is 'fragle'
  - Meaning that if you need to extract values you have to parse the raw text
  - Which can break if hte tool's output format changes 

# What to use instead
- V4L2 C API directly