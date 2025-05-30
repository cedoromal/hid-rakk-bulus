// SPDX-License-Identifier: GPL-2.0
/*
 * HID driver for Rakk Bulus
 *
 * Copyright (c) 2025 Carl Eric Doromal
 */

#include <linux/hid.h>

#define USB_VENDOR_ID_RAKK 0x248A
#define USB_DEVICE_ID_RAKK_BULUS 0x5B49
#define USB_DEVICE_ID_RAKK_BULUS_DONGLE 0x5B4A
#define USB_DEVICE_ID_RAKK_BULUS_BLUETOOTH 0x8266
#define RAKK_BULUS_ORIG_RDESC_LENGTH 150
#define RAKK_BULUS_DONGLE_ORIG_RDESC_LENGTH 172
#define RAKK_BULUS_BLUETOOTH_ORIG_RDESC_LENGTH 89
#define RAKK_BULUS_FAULT_OFFSET 17

// For Wired and Dongle
static const __u8 rakk_bulus_rdesc_fixed[] = {
    0x05, 0x01,                   // USAGE_PAGE         0x0001 Desktop
    0x09, 0x02,                   // USAGE              0x00010002 Mouse
    0xA1, 0x01,                   // COLLECTION         0x01 Application
    0x85, 0x01,                   //   REPORT_ID          0x01 (1)
    0x09, 0x01,                   //   USAGE              0x00010001 Pointer
    0xA1, 0x00,                   //   COLLECTION         0x00 Physical
    0x05, 0x09,                   //     USAGE_PAGE         0x0009 Button
    0x19, 0x01,                   //     USAGE_MINIMUM      0x00090001 (1)
    0x29, 0x05,                   //     USAGE_MAXIMUM      0x00090005 (5) /* Originally set to 0x00090003 (3) */
    0x15, 0x00,                   //     LOGICAL_MINIMUM    0x00 (0)
    0x25, 0x01,                   //     LOGICAL_MAXIMUM    0x01 (1)
    0x75, 0x01,                   //     REPORT_SIZE        0x01 (1) Number of bits per field
    0x95, 0x05,                   //     REPORT_COUNT       0x05 (5) Number of fields
    0x81, 0x02,                   //     INPUT              0x00000002 (5 fields x 1 bit) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
    0x75, 0x03,                   //     REPORT_SIZE        0x03 (3) Number of bits per field
    0x95, 0x01,                   //     REPORT_COUNT       0x01 (1) Number of fields
    0x81, 0x01,                   //     INPUT              0x00000001 (1 field x 3 bits) 1=Constant 0=Array 0=Absolute
    0x05, 0x01,                   //     USAGE_PAGE         0x0001 Desktop
    0x09, 0x30,                   //     USAGE              0x00010030 X
    0x09, 0x31,                   //     USAGE              0x00010031 Y
    0x16, 0x01, 0x80,             //     LOGICAL_MINIMUM    0x8001 (-32767)
    0x26, 0xFF, 0x7F,             //     LOGICAL_MAXIMUM    0x7FFF (32767)
    0x75, 0x10,                   //     REPORT_SIZE        0x10 (16) Number of bits per field
    0x95, 0x02,                   //     REPORT_COUNT       0x02 (2) Number of fields
    0x81, 0x06,                   //     INPUT              0x00000006 (2 fields x 16 bits) 0=Data 1=Variable 1=Relative 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
    0x09, 0x38,                   //     USAGE              0x00010038 Wheel
    0x15, 0x81,                   //     LOGICAL_MINIMUM    0x81 (-127)
    0x25, 0x7F,                   //     LOGICAL_MAXIMUM    0x7F (127)
    0x75, 0x08,                   //     REPORT_SIZE        0x08 (8) Number of bits per field
    0x95, 0x01,                   //     REPORT_COUNT       0x01 (1) Number of fields
    0x81, 0x06,                   //     INPUT              0x00000006 (1 field x 8 bits) 0=Data 1=Variable 1=Relative 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
    0xC0,                         //   END_COLLECTION     Physical
    0xC0,                         // END_COLLECTION     Application
};

// For Bluetooth
static const __u8 rakk_bulus_bluetooth_rdesc_fixed[] = {
    0x05, 0x01,                   // USAGE_PAGE         0x0001 Desktop
    0x09, 0x02,                   // USAGE              0x00010002 Mouse
    0xA1, 0x01,                   // COLLECTION         0x01 Application
    0x85, 0x03,                   //   REPORT_ID          0x03 (3)
    0x09, 0x01,                   //   USAGE              0x00010001 Pointer
    0xA1, 0x00,                   //   COLLECTION         0x00 Physical
    0x05, 0x09,                   //     USAGE_PAGE         0x0009 Button
    0x19, 0x01,                   //     USAGE_MINIMUM      0x00090001 (1)
    0x29, 0x05,                   //     USAGE_MAXIMUM      0x00090005 (5) /* Originally set to 0x00090003 (3) */
    0x15, 0x00,                   //     LOGICAL_MINIMUM    0x00 (0)
    0x25, 0x01,                   //     LOGICAL_MAXIMUM    0x01 (1)
    0x75, 0x01,                   //     REPORT_SIZE        0x01 (1) Number of bits per field
    0x95, 0x05,                   //     REPORT_COUNT       0x05 (5) Number of fields
    0x81, 0x02,                   //     INPUT              0x00000002 (5 fields x 1 bit) 0=Data 1=Variable 0=Absolute 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
    0x75, 0x03,                   //     REPORT_SIZE        0x03 (3) Number of bits per field
    0x95, 0x01,                   //     REPORT_COUNT       0x01 (1) Number of fields
    0x81, 0x01,                   //     INPUT              0x00000001 (1 field x 3 bits) 1=Constant 0=Array 0=Absolute
    0x05, 0x01,                   //     USAGE_PAGE         0x0001 Desktop
    0x09, 0x30,                   //     USAGE              0x00010030 X
    0x09, 0x31,                   //     USAGE              0x00010031 Y
    0x16, 0x01, 0x80,             //     LOGICAL_MINIMUM    0x8001 (-32767)
    0x26, 0xFF, 0x7F,             //     LOGICAL_MAXIMUM    0x7FFF (32767)
    0x75, 0x10,                   //     REPORT_SIZE        0x10 (16) Number of bits per field
    0x95, 0x02,                   //     REPORT_COUNT       0x02 (2) Number of fields
    0x81, 0x06,                   //     INPUT              0x00000006 (2 fields x 16 bits) 0=Data 1=Variable 1=Relative 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
    0x09, 0x38,                   //     USAGE              0x00010038 Wheel
    0x15, 0x81,                   //     LOGICAL_MINIMUM    0x81 (-127)
    0x25, 0x7F,                   //     LOGICAL_MAXIMUM    0x7F (127)
    0x75, 0x08,                   //     REPORT_SIZE        0x08 (8) Number of bits per field
    0x95, 0x01,                   //     REPORT_COUNT       0x01 (1) Number of fields
    0x81, 0x06,                   //     INPUT              0x00000006 (1 field x 8 bits) 0=Data 1=Variable 1=Relative 0=NoWrap 0=Linear 0=PrefState 0=NoNull 0=NonVolatile 0=Bitmap
    0xC0,                         //   END_COLLECTION     Physical
    0xC0,                         // END_COLLECTION     Application
};

static __u8 *rakk_bulus_report_fixup(struct hid_device *hdev, __u8 *rdesc, unsigned int *rsize) {
    switch (hdev->product) {
        case USB_DEVICE_ID_RAKK_BULUS_BLUETOOTH:
            if (*rsize == RAKK_BULUS_BLUETOOTH_ORIG_RDESC_LENGTH && memcmp(rdesc, rakk_bulus_bluetooth_rdesc_fixed, RAKK_BULUS_FAULT_OFFSET) == 0) {
                hid_info(hdev, "Fixing up Rakk Bulus (Bluetooth) report descriptor\n");
                *rsize = sizeof(rakk_bulus_bluetooth_rdesc_fixed);
                return rakk_bulus_bluetooth_rdesc_fixed;
            }
            break;

        case USB_DEVICE_ID_RAKK_BULUS_DONGLE:
        case USB_DEVICE_ID_RAKK_BULUS:
            if ((*rsize == RAKK_BULUS_ORIG_RDESC_LENGTH || *rsize == RAKK_BULUS_DONGLE_ORIG_RDESC_LENGTH) 
            && memcmp(rdesc, rakk_bulus_rdesc_fixed, RAKK_BULUS_FAULT_OFFSET) == 0) {
                hid_info(hdev, "Fixing up Rakk Bulus (Wired/Dongle) report descriptor\n");
                *rsize = sizeof(rakk_bulus_rdesc_fixed);
                return rakk_bulus_rdesc_fixed;
            }
            break;
	}
    return rdesc;
}

static const struct hid_device_id rakk_bulus_devices[] = {
	{ HID_USB_DEVICE(USB_VENDOR_ID_RAKK, USB_DEVICE_ID_RAKK_BULUS) },
    { HID_USB_DEVICE(USB_VENDOR_ID_RAKK, USB_DEVICE_ID_RAKK_BULUS_DONGLE) },
    { HID_BLUETOOTH_DEVICE(USB_VENDOR_ID_RAKK, USB_DEVICE_ID_RAKK_BULUS_BLUETOOTH) },
	{ }
};
MODULE_DEVICE_TABLE(hid, rakk_bulus_devices);

static struct hid_driver rakk_bulus_driver = {
	.name = "rakk-bulus",
	.id_table = rakk_bulus_devices,
	.report_fixup = rakk_bulus_report_fixup,
};
module_hid_driver(rakk_bulus_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carl Eric Doromal <contact@cedoromal.com>");
MODULE_DESCRIPTION("HID driver for Rakk Bulus");