# Logicam Camera Setting Cheatsheet (video2)

## Command to list out controls 
```bash
v4l2-ctl --device=/dev/video2 --all
```

**Driver Info**
===============

* Driver name      : uvcvideo
* Card type        : HD Pro Webcam C920
* Bus info         : usb-0000:00:14.0-6
* Driver version   : 6.1.128
* Capabilities     : 0x84a00001
    * Video Capture
    * Metadata Capture
    * Streaming
    * Extended Pix Format
    * Device Capabilities

**Device Caps**
==============

* Device Caps      : 0x04200001
    * Video Capture
    * Streaming
    * Extended Pix Format

**Media Driver Info**
=====================

* Driver name      : uvcvideo
* Model            : HD Pro Webcam C920
* Serial           : 416E3D9F
* Bus info         : usb-0000:00:14.0-6
* Media version    : 6.1.128
* Hardware revision: 0x00000021 (33)
* Driver version   : 6.1.128

**Interface Info**
================

* ID               : 0x03000002
* Type             : V4L Video

**Entity Info**
==============

* ID               : 0x00000001 (1)
* Name             : HD Pro Webcam C920
* Function         : V4L2 I/O
* Flags            : default
* Pad 0x01000007   : 0: Sink
    * Link 0x0200001f: from remote pad 0x100000a of entity 'Processing 3' (Video Pixel Formatter): Data, Enabled, Immutable

**Priority**
==========

* Priority: 2

**Video Input**
==============

* Video input : 0 (Input 1: ok)

**Format Video Capture**
=====================

* Width/Height      : 640/480
* Pixel Format      : 'YUYV' (YUYV 4:2:2)
* Field             : None
* Bytes per Line    : 1280
* Size Image        : 614400
* Colorspace        : sRGB
* Transfer Function : Rec. 709
* YCbCr/HSV Encoding: ITU-R 601
* Quantization      : Default (maps to Limited Range)
* Flags             :

**Crop Capability Video Capture**
=============================

* Bounds      : Left 0, Top 0, Width 640, Height 480
* Default     : Left 0, Top 0, Width 640, Height 480
* Pixel Aspect: 1/1

**Selection Video Capture**
=========================

* crop_default, Left 0, Top 0, Width 640, Height 480, Flags:
* crop_bounds, Left 0, Top 0, Width 640, Height 480, Flags:

**Streaming Parameters Video Capture**
=====================================

* Capabilities     : timeperframe
* Frames per second: 30.000 (30/1)
* Read buffers     : 0

**User Controls**
================
| Control | Address | Type | Min/Max | Default | Value |
| --- | --- | --- | --- | --- | --- |
| Brightness | 0x00980900 | int | 0/255 | 128 | 128 |
| Contrast | 0x00980901 | int | 0/255 | 128 | 128 |
| Saturation | 0x00980902 | int | 0/255 | 128 | 128 |
| White Balance Automatic | 0x0098090c | bool | - | 1 | 1 |
| Gain | 0x00980913 | int | 100/500 | 100 | 100 |
| Led1 Mode | 0x0a046d05 | menu | 0/3 | 0 | Auto (3) |
| Led1 Frequency | 0x0a046d06 | int | 0/255 | 0 | 0 |

**Camera Controls**
================
| Control | Address | Type | Min/Max | Default | Value |
| --- | --- | --- | --- | --- | --- |
| Auto Exposure | 0x009a0901 | menu | 0/3 | 3 | Manual Mode (1) |
| Exposure Time Absolute | 0x009a0902 | int | 3/2047 | 250 | 666 |
| Exposure Dynamic Framerate | 0x009a0903 | bool | - | 0 | 1 |
| Pan Absolute | 0x009a0908 | int | -36000/36000 | 0 | 0 |
| Tilt Absolute | 0x009a0909 | int | -36000/36000 | 0 | 0 |
| Focus Absolute | 0x009a090a | int | 0/250 | 0 | 30 (inactive) |
| Focus Automatic Continuous | 0x009a090c | bool | - | 1 | 1 |
| Zoom Absolute | 0x009a090d | int | 100/500 | 100 | 100 |
| Led1 Mode | 0x0a046d05 | menu | 0/3 | 0 | Off (0) |
| Led1 Frequency | 0x0a046d06 | int | 0/255 | 0 | 0 |

## Example command 
```bash 
v4l2-ctl --device=/dev/video2 --set-ctrl=focus_automatic_continuous=0
```