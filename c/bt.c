/* ########################################################################

   USBIP hardware emulation 

   ########################################################################

   Copyright (c) : 2021 José Ignacio Tornos Martínez

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   For e-mail suggestions :  jtornosm@redhat.com
   ######################################################################## */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>

#include "usbip.h"

/* Device Descriptor */
const USB_DEVICE_DESCRIPTOR dev_dsc=
{
    0x12,                   // Size of this descriptor in bytes
    0x01,                   // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0xe0,                   // Class Code
    0x01,                   // Subclass code
    0x01,                   // Protocol code
    0x40,                   // Max packet size for EP0, see usb_config.h
    0x0a12,                 // Vendor ID
    0x0001,                 // Product ID
    0x9188,                 // Device release number in BCD format
    0x00,                   // Manufacturer string index
    0x02,                   // Product string index
    0x00,                   // Device serial number string index
    0x01                    // Number of possible configurations
};

/* Configuration */
typedef struct __attribute__ ((__packed__)) _CONFIG_BT
{
 USB_CONFIGURATION_DESCRIPTOR dev_conf0;
 USB_INTERFACE_DESCRIPTOR dev_int0;
 USB_ENDPOINT_DESCRIPTOR dev_ep00;
 USB_ENDPOINT_DESCRIPTOR dev_ep01;
 USB_ENDPOINT_DESCRIPTOR dev_ep02;
 USB_INTERFACE_DESCRIPTOR dev_int1;
 USB_ENDPOINT_DESCRIPTOR dev_ep10;
 USB_ENDPOINT_DESCRIPTOR dev_ep11;
 USB_INTERFACE_DESCRIPTOR dev_int2;
 USB_ENDPOINT_DESCRIPTOR dev_ep20;
 USB_ENDPOINT_DESCRIPTOR dev_ep21;
 USB_INTERFACE_DESCRIPTOR dev_int3;
 USB_ENDPOINT_DESCRIPTOR dev_ep30;
 USB_ENDPOINT_DESCRIPTOR dev_ep31;
 USB_INTERFACE_DESCRIPTOR dev_int4;
 USB_ENDPOINT_DESCRIPTOR dev_ep40;
 USB_ENDPOINT_DESCRIPTOR dev_ep41;
 USB_INTERFACE_DESCRIPTOR dev_int5;
 USB_ENDPOINT_DESCRIPTOR dev_ep50;
 USB_ENDPOINT_DESCRIPTOR dev_ep51;
 USB_INTERFACE_DESCRIPTOR dev_int6;
 USB_ENDPOINT_DESCRIPTOR dev_ep60;
 USB_ENDPOINT_DESCRIPTOR dev_ep61;
} CONFIG_BT;

const CONFIG_BT  configuration_bt={{
    /* Configuration Descriptor */
    0x09,//sizeof(USB_CFG_DSC),    // Size of this descriptor in bytes
    USB_DESCRIPTOR_CONFIGURATION,                // CONFIGURATION descriptor type
    sizeof(CONFIG_BT),                 // Total length of data for this cfg
    2,                      // Number of interfaces in this cfg
    1,                      // Index value of this configuration
    0,                      // Configuration string index
    0xC0,      
    50,                     // Max power consumption (2X mA)
    },{ 
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    0,                      // Interface Number
    0,                      // Alternate Setting Number
    3,                      // Number of endpoints in this intf
    0xe0,                   // Class code
    0x01,                   // Subclass code
    0x01,                   // Protocol code
    0                       // Interface string index
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x81,                   // Address (USB_ENDPOINT_DIR_MASK) EP 1 IN
    0x03,                   // Attributes
    0x0010,                 // Size
    0x01                    // Interval
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x02,                   // Address (USB_ENDPOINT_DIR_MASK) EP 2 OUT
    0x02,                   // Attributes
    0x0040,                 // Size
    0x01                    // Interval
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x82,                   // Address (USB_ENDPOINT_DIR_MASK) EP 2 IN
    0x02,                   // Attributes
    0x0040,                 // Size
    0x01                    // Interval
    },{
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    1,                      // Interface Number
    0,                      // Alternate Setting Number
    2,                      // Number of endpoints in this intf
    0xe0,                   // Class code
    0x01,                   // Subclass code
    0x01,                   // Protocol code
    0                       // Interface string index
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x03,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 OUT
    0x01,                   // Attributes
    0x0000,                 // Size
    0x01                    // Interval
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x83,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 IN
    0x01,                   // Attributes
    0x0000,                 // Size
    0x01                    // Interval
    },{
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    1,                      // Interface Number
    1,                      // Alternate Setting Number
    2,                      // Number of endpoints in this intf
    0xe0,                   // Class code
    0x01,                   // Subclass code
    0x01,                   // Protocol code
    0                       // Interface string index
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x03,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 OUT
    0x01,                   // Attributes
    0x0009,                 // Size
    0x01                    // Interval
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x83,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 IN
    0x01,                   // Attributes
    0x0009,                 // Size
    0x01                    // Interval
    },{
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    1,                      // Interface Number
    2,                      // Alternate Setting Number
    2,                      // Number of endpoints in this intf
    0xe0,                   // Class code
    0x01,                   // Subclass code
    0x01,                   // Protocol code
    0                       // Interface string index
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x03,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 OUT
    0x01,                   // Attributes
    0x0011,                 // Size
    0x01                    // Interval
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x83,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 IN
    0x01,                   // Attributes
    0x0011,                 // Size
    0x01                    // Interval
    },{
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    1,                      // Interface Number
    3,                      // Alternate Setting Number
    2,                      // Number of endpoints in this intf
    0xe0,                   // Class code
    0x01,                   // Subclass code
    0x01,                   // Protocol code
    0                       // Interface string index
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x03,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 OUT
    0x01,                   // Attributes
    0x0019,                 // Size
    0x01                    // Interval
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x83,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 IN
    0x01,                   // Attributes
    0x0019,                 // Size
    0x01                    // Interval
    },{
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    1,                      // Interface Number
    4,                      // Alternate Setting Number
    2,                      // Number of endpoints in this intf
    0xe0,                   // Class code
    0x01,                   // Subclass code
    0x01,                   // Protocol code
    0                       // Interface string index
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x03,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 OUT
    0x01,                   // Attributes
    0x0021,                 // Size
    0x01                    // Interval
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x83,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 IN
    0x01,                   // Attributes
    0x0021,                 // Size
    0x01                    // Interval
    },{
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    1,                      // Interface Number
    5,                      // Alternate Setting Number
    2,                      // Number of endpoints in this intf
    0xe0,                   // Class code
    0x01,                   // Subclass code
    0x01,                   // Protocol code
    0                       // Interface string index
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x03,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 OUT
    0x01,                   // Attributes
    0x0031,                 // Size
    0x01                    // Interval
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x83,                   // Address (USB_ENDPOINT_DIR_MASK) EP 3 IN
    0x01,                   // Attributes
    0x0031,                 // Size
    0x01                    // Interval
}};

const char *configuration[] = {(const char *)&configuration_bt};

const unsigned char string_0[] = { // available languages  descriptor
		0x04,                  
                USB_DESCRIPTOR_STRING, 
		0x09, 
                0x04 
		};

const unsigned char string_1[] = { //
		0x0A, 
                USB_DESCRIPTOR_STRING, // bLength, bDscType
		'T', 0x00, //
		'e', 0x00, //
		's', 0x00, //
		't', 0x00, //
		};

const unsigned char string_2[] = { //
		0x18, 
                USB_DESCRIPTOR_STRING, //
		'B', 0x00, //
		'T', 0x00, //
		' ', 0x00, //
		'U', 0x00, //
		'S', 0x00, //
		'B', 0x00, //
		' ', 0x00, //
		'T', 0x00, //
		'E', 0x00, //
		'S', 0x00, //
		'T', 0x00, //
		};

const unsigned char string_3[] = { //
		0x18, 
                USB_DESCRIPTOR_STRING, //
		'V', 0x00, //
		'i', 0x00, //
		'r', 0x00, //
		't', 0x00, //
		'u', 0x00, //
		'a', 0x00, //
		'l', 0x00, //
		' ', 0x00, //
                'U', 0x00, //
                'S', 0x00, //
                'B', 0x00, //
		};

const unsigned char *strings[]={string_0, string_1, string_2, string_3};

#define BD_ADDRESS_SIZE 6
static unsigned char bd_addr[BD_ADDRESS_SIZE] = {0};

char verbose_level = 0;
unsigned short server_usbip_tcp_port = 3240;
char usb_bus_port[MAX_USB_BUS_PORT_SIZE] = "1-1";
static char usb_bd_address[] = "00:1A:7D:DA:71:13";
#define MAX_MANUFACTURER_SIZE 32
static char manufacturer[MAX_MANUFACTURER_SIZE] = "Trust";

#define MAX_CONTROL_SIZE 16
#define MAX_CONTROL_TAIL_SIZE 256
#define MAX_COMMENT_SIZE 256
#define DEFAULT_BT_CONTROL_INDEX 0
#define BD_ADDRESS_BT_CONTROL_INDEX 3

struct bt_control_st
{
    unsigned char control_size;
    unsigned char control[MAX_CONTROL_TAIL_SIZE];
    unsigned char control_flag;
    unsigned char tail_size;
    unsigned char tail[MAX_CONTROL_TAIL_SIZE];
    char comment[MAX_COMMENT_SIZE];
};

/*
 * bt control flag values:
 * - '0' control answer includes control + tail
 * - '1' control request is longer, control answer includes short control + tail
 * - '2' control answer only includes tail
 * - '3' control answer only includes tail, start scanning
 */

static struct bt_control_st bt_control_array[] =
{
    {.control_size = 3, .control = {0x01, 0x10, 0x00}, .control_flag = 0, .tail_size = 8, .tail = {0x06, 0xbb, 0x22, 0x05, 0x0a, 0x00, 0xbb, 0x22}, .comment="control configuration"},
    {.control_size = 3, .control = {0x03, 0x0c, 0x00}, .control_flag = 0, .tail_size = 0, .tail = {0}, .comment = ""},
    {.control_size = 3, .control = {0x03, 0x10, 0x00}, .control_flag = 0, .tail_size = 8, .tail = {0xff, 0xff, 0x8f, 0xfe, 0xdb, 0xff, 0x5b, 0x87}, .comment = ""},
    {.control_size = 3, .control = {0x09, 0x10, 0x00}, .control_flag = 0, .tail_size = 6, .tail = {0x13, 0x71, 0xda, 0x7d, 0x1a, 0x00}, .comment = "bd address"},
    {.control_size = 3, .control = {0x05, 0x10, 0x00}, .control_flag = 0, .tail_size = 7, .tail = {0x36, 0x01, 0x40, 0x0a, 0x00, 0x08, 0x00}, .comment = ""},
    {.control_size = 3, .control = {0x23, 0x0c, 0x00}, .control_flag = 0, .tail_size = 3, .tail = {0x00, 0x00, 0x00}, .comment = ""},
    {.control_size = 3, .control = {0x14, 0x0c, 0x00}, .control_flag = 0, .tail_size = 248, .tail = {'B' , 'T' , ' ' , 'U' , 'S' , 'B' , ' ' , 'T' ,
        'E' , 'S' , 'T' , ' ' , '-' , ' ' , 'C' , 'S' , 'R' , '8' , '5' , '1' , '0' , ' ' , 'A' , '1' , '0' , 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, .comment = ""},
    {.control_size = 3, .control = {0x25, 0x0c, 0x00}, .control_flag = 0, .tail_size = 3, .tail = {0x00, 0x60, 0x00}, .comment = ""},
    {.control_size = 3, .control = {0x38, 0x0c, 0x00}, .control_flag = 0, .tail_size = 2, .tail = {0x00, 0x62}, .comment = ""},
    {.control_size = 3, .control = {0x39, 0x0c, 0x00}, .control_flag = 0, .tail_size = 4, .tail = {0x01, 0x33, 0x8b, 0x9e}, .comment = ""},
    {.control_size = 2, .control = {0x05, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x16, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 3, .control = {0x02, 0x20, 0x00}, .control_flag = 0, .tail_size = 3, .tail = {0x00, 0x00, 0x00}, .comment = ""},
    {.control_size = 3, .control = {0x03, 0x20, 0x00}, .control_flag = 0, .tail_size = 8, .tail = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, .comment = ""},
    {.control_size = 3, .control = {0x1c, 0x20, 0x00}, .control_flag = 0, .tail_size = 8, .tail = {0xff, 0xff, 0xff, 0x1f, 0x00, 0x00, 0x00, 0x00}, .comment = ""},
    {.control_size = 3, .control = {0x02, 0x10, 0x00}, .control_flag = 0, .tail_size = 64, .tail = {0xff, 0xff, 0xff, 0x03, 0xfe, 0xff, 0xff, 0xff, 
        0xff, 0xff, 0xff, 0xff, 0xf3, 0x0f, 0xe8, 0xfe, 0x3f, 0xf7, 0x83, 0xff, 0x1c, 0x00, 0x00, 0x00, 0x61, 0xf7, 0xff, 0xff, 0x7f, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, .comment = ""},
    {.control_size = 2, .control = {0x52, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x45, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 3, .control = {0x58, 0x0c, 0x00}, .control_flag = 0, .tail_size = 1, .tail = {0x04}, .comment = ""},
    {.control_size = 4, .control = {0x04, 0x10, 0x01, 0x01}, .control_flag = 2, .tail_size = 13, .tail = {0x04, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 
        0x00, 0x00, 0x00, 0x00, 0x00}, .comment = ""},
    {.control_size = 2, .control = {0x01, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 10, .control = {0x0d, 0x0c, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}, .control_flag = 2, .tail_size = 7, .tail = {0x0d, 0x0c, 0x00, 0x10, 0x00, 0x00, 0x00}, .comment = ""},
    {.control_size = 2, .control = {0x0f, 0x08}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 3, .control = {0x1b, 0x0c, 0x00}, .control_flag = 0, .tail_size = 4, .tail = {0x00, 0x08, 0x12, 0x00}, .comment = ""},
    {.control_size = 3, .control = {0x46, 0x0c, 0x00}, .control_flag = 0, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x01, 0x20}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 3, .control = {0x07, 0x20, 0x00}, .control_flag = 0, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 3, .control = {0x0f, 0x20, 0x00}, .control_flag = 0, .tail_size = 1, .tail = {0x19}, .comment = ""},
    {.control_size = 3, .control = {0x10, 0x20, 0x00}, .control_flag = 0, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 10, .control = {0x12, 0x0c,  0x07, 0x00, 0x00, 0x00, 0x00, 0x00,  0x00, 0x01}, .control_flag = 2, .tail_size = 5, .tail = {0x12, 0x0c, 0x00, 0x00, 0x00}, .comment = ""},
    {.control_size = 2, .control = {0x56, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x6d, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x08, 0x20}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x09, 0x20}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x13, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x24, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x1a, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x3a, 0x0c}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x05, 0x20}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x0b, 0x20}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x0c, 0x20}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 8, .control = {0x01, 0x04, 0x05, 0x33, 0x8b, 0x9e, 0x08, 0x00}, .control_flag = 3, .tail_size = 6, .tail = {0x0f, 0x04, 0x00, 0x01, 0x01, 0x04}, .comment ="scan request"},
    {.control_size = 3, .control = {0x02, 0x04, 0x00}, .control_flag = 0, .tail_size = 0, .tail = {0}, .comment = ""},
    {.control_size = 2, .control = {0x11, 0x20}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 2, .control = {0x29, 0x04}, .control_flag = 1, .tail_size = 1, .tail = {0x00}, .comment = ""},
    {.control_size = 0, .control = {0}, .tail_size = 0, .tail = {0}, .comment = ""}
};

#define MAX_DEVICE_SIZE 256

struct bt_device_st
{
    unsigned char device_size;
    unsigned char device[MAX_DEVICE_SIZE];
};

static struct bt_device_st bt_device_array[] =
{
    {.device_size = 32, .device = {0x3e, 0x1e, 0x02, 0x01, 0x00, 0x01, 0xe6, 0xcc, 0x48, 0x6f, 0x69, 0x66, 0x12, 0x02, 0x01, 0x1a,
                                   0x02, 0x0a, 0x0c, 0x0b, 0xff, 0x4c, 0x00, 0x10, 0x06, 0x01, 0x1e, 0xf3, 0xa8, 0x41, 0x41, 0xa9}},
    {.device_size = 14, .device = {0x3e, 0x0c, 0x02, 0x01, 0x04, 0x01, 0xe6, 0xcc, 0x48, 0x6f, 0x69, 0x66, 0x00, 0xa9}},
    {.device_size = 45, .device = {0x3e, 0x2b, 0x02, 0x01, 0x03, 0x01, 0xc3, 0x18, 0x59, 0xd5, 0xde, 0x4c, 0x1f, 0x1e, 0xff, 0x06,
                                   0x00, 0x01, 0x09, 0x20, 0x02, 0x42, 0x62, 0x2c, 0xee, 0xcd, 0xda, 0x30, 0xb7, 0x4c, 0x79, 0xd6,
                                   0xaf, 0x4c, 0x72, 0x54, 0x6a, 0x79, 0xa2, 0x53, 0x16, 0x71, 0xad, 0xf8, 0xb4}},
    {.device_size = 43, .device = {0x3e, 0x29, 0x02, 0x01, 0x00, 0x01, 0x2c, 0xf7, 0x1e, 0x56, 0x32, 0xe1, 0x1f, 0x02, 0x01, 0x06,
                                   0x03, 0x03, 0x00, 0xa5, 0x09, 0x16, 0x02, 0x38, 0xe1, 0x32, 0x56, 0x1e, 0xf7, 0x2c, 0x0d, 0x09,
                                    'T',  'E',  'S',  'T',  ' ',  'W',  'A',  'T',  'C',  'H', 0x00}},
    {.device_size = 24, .device = {0x3e, 0x16, 0x02, 0x01, 0x04, 0x01, 0x2c, 0xf7, 0x1e, 0x56, 0x32, 0xe1, 0x0a, 0x09, 0xff, 0x60,
                                   0x00, 0xe1, 0x32, 0x56, 0x1e, 0xf7, 0x2c, 0xcc}},
    {.device_size = 39, .device = {0x3e, 0x25, 0x02, 0x01, 0x00, 0x00, 0x7a, 0xfb, 0x88, 0xe5, 0xb7, 0x54, 0x1a, 0x02, 0x01, 0x06,
                                   0x03, 0x03, 0x8f, 0xfe, 0x03, 0xff, 0x12, 0x03, 0x0e, 0x09,  'T',  'E',  'S',  'T',  ' ',  'S',
                                    'P',  'E',  'A',  'K',  'E',  'R', 0x00}},
    {.device_size = 0, .device = {0}}
};

static unsigned char scanning = 0;
static unsigned char buffer[1024] = {0};

static struct bt_control_st *get_bt_control(unsigned char *bt_control, int bt_control_size)
{
    int i = 0;
    int j = 0;
    struct bt_control_st *control_st = NULL;

    while(1)
    {
        control_st = &bt_control_array[i++];
        if (control_st->control_size == 0)
        {
            control_st = NULL;
            break;
        }
        if ((control_st->control_size != bt_control_size) &&
           (!((control_st->control_flag == 1) && (control_st->control_size < bt_control_size))))
        {
            continue;
        }
        for(j = 0; j < control_st->control_size; j++)
        {
            if (control_st->control[j] != bt_control[j])
            {
                control_st = NULL;
                break;
            }
        }
        if (control_st)
        {
            break;
        }
    }
    
    return control_st;
}

static void process_bt_control(int sockfd, USBIP_RET_SUBMIT *usb_req, int bt_control_size, unsigned char *bt_control, int bt_control_seq)
{
    static int last_bt_control_size = -1;
    static unsigned char last_bt_control[MAX_CONTROL_TAIL_SIZE] = {0};
    static int last_bt_control_seq = -1;
    static unsigned char first_bt_control = 1;
    static unsigned char frag_message_length = 0;
    static int frag_message_pos = 0;
    static int scanning_device_pos = 0;
    static int scanning_frag_device_pos = 0;
    struct bt_control_st *control_st = NULL;
    int message_length = 0;
    int i = 0;

    if ((last_bt_control_size == -1) && (bt_control_size >= 0) && (last_bt_control_seq >= 0) && (bt_control_seq == -1))
    {
        control_st = get_bt_control(bt_control, bt_control_size);
        if (!control_st)
        {
            printf("bt control is not found\n");
            return;
        }
        usb_req->seqnum = last_bt_control_seq;
        last_bt_control_seq = -1;
    }
    else if ((last_bt_control_size >= 0) && (bt_control_size == -1) && (last_bt_control_seq == -1) && (bt_control_seq >= 0))
    {
        control_st = get_bt_control(last_bt_control, last_bt_control_size);
        if (!control_st)
        {
            printf("bt control is not found\n");
            return;
        }
        usb_req->seqnum = bt_control_seq;
        last_bt_control_size = -1;
    }
    else
    {
        if ((last_bt_control_size == -1) && (bt_control_size >= 0))
        {
            last_bt_control_size = bt_control_size;
            memcpy(last_bt_control, bt_control, bt_control_size);
            return;
        }
        else if ((last_bt_control_seq == -1) && (bt_control_seq >= 0))
        {
            if (first_bt_control)
            {
                control_st = &bt_control_array[DEFAULT_BT_CONTROL_INDEX];
                first_bt_control = 0;
            }
            else if (frag_message_length)
            {
                message_length = frag_message_length - frag_message_pos;
                if (message_length > MAX_CONTROL_SIZE)
                {
                    message_length = MAX_CONTROL_SIZE;
                }
                send_usb_req(sockfd, usb_req, (char *)buffer + frag_message_pos, message_length, 0);
                frag_message_pos += message_length;
                if (frag_message_pos == frag_message_length)
                {
                    frag_message_pos = frag_message_length = 0;
                }
                return;
            }
            else if (scanning)
            {
                if (bt_device_array[scanning_device_pos].device_size == 0)
                {
                    scanning = 0;
                    last_bt_control_seq = bt_control_seq;
                    printf("scanning end\n");
                }
                else if (bt_device_array[scanning_device_pos].device_size > MAX_CONTROL_SIZE)
                {
                    message_length = bt_device_array[scanning_device_pos].device_size - scanning_frag_device_pos;
                    if (message_length > MAX_CONTROL_SIZE)
                    {
                        message_length = MAX_CONTROL_SIZE;
                    }
                    send_usb_req(sockfd, usb_req, (char *)bt_device_array[scanning_device_pos].device + scanning_frag_device_pos, message_length, 0);
                    scanning_frag_device_pos += message_length;
                    if (scanning_frag_device_pos == bt_device_array[scanning_device_pos].device_size)
                    {
                        scanning_frag_device_pos = 0;
                        scanning_device_pos ++;
                    }
                }
                else
                {
                    send_usb_req(sockfd, usb_req, (char *)bt_device_array[scanning_device_pos].device, bt_device_array[scanning_device_pos].device_size, 0);
                    scanning_device_pos ++;
                }
                return;
            }
            else
            { 
                last_bt_control_seq = bt_control_seq;
                return;
            }
        }
        else if ((last_bt_control_seq >=0) && (bt_control_seq >= 0))
        { 
            last_bt_control_seq = bt_control_seq;
            return;
        }
        else
        {
            printf("unknown bt control status\n");
            return;
        }
    }

    if (strlen(control_st->comment))
    {
        printf("%s\n", control_st->comment);
    }
    if (control_st->control_flag == 3)
    {
        send_usb_req(sockfd, usb_req, (char *)control_st->tail, control_st->tail_size, 0);
        scanning = 1;
        scanning_device_pos = 0;
        printf("scanning ...\n");
    }
    else
    {
        buffer[message_length ++] = 0x0e;
        buffer[message_length ++] = ((control_st->control_flag == 2)? 0: control_st->control_size) + control_st->tail_size + 1;
        buffer[message_length ++] = 0x01;
        if (control_st->control_flag != 2) 
        {
            for (i = 0; i < control_st->control_size; i ++)
                buffer[message_length ++] = control_st->control[i];
        }
        for (i = 0; i < control_st->tail_size; i ++)
            buffer[message_length ++] = control_st->tail[i];
        if (message_length > MAX_CONTROL_SIZE)
        {
            frag_message_length = message_length;
            frag_message_pos = MAX_CONTROL_SIZE;
            message_length = MAX_CONTROL_SIZE;
        }
        send_usb_req(sockfd, usb_req, (char *)buffer, message_length, 0);
    }
}

void handle_data(int sockfd, USBIP_RET_SUBMIT *usb_req, int bl)
{
    if (verbose_level > 2)
    {
        printf("ep%d direction %d seq %d size %d\n", usb_req->ep, usb_req->direction, usb_req->seqnum, bl);
    }

    if (usb_req->ep == 0x01)
    {
        if (usb_req->direction == 1) //output
        {
            process_bt_control(sockfd, usb_req, -1, NULL, usb_req->seqnum);
        }
    }

    if((usb_req->ep == 0x02))
    {
        if (usb_req->direction == 1) //output
        {
            /* TODO */
        }
    }
};

void handle_unknown_control(int sockfd, StandardDeviceRequest * control_req, USBIP_RET_SUBMIT *usb_req)
{
    int bt_control_size = 0;
    unsigned char bt_control[MAX_CONTROL_TAIL_SIZE] = {0};
    int i;

    if (verbose_level > 2)
    {
        printf("BT CONTROL RequestType 0x%x Request 0x%x lenght %d\n", control_req->bmRequestType, control_req->bRequest, control_req->wLength);
    }

    if (control_req->bmRequestType == 0x20)
    {
        if ((control_req->bRequest == 0x0) && (control_req->wLength))
        {
            bt_control_size = recv (sockfd, (char *) bt_control, control_req->wLength, 0);
            if (bt_control_size != control_req->wLength)
            {
                printf("unexpected contrl seq size %d/%d\n", bt_control_size, control_req->wLength);
            }
            if (verbose_level > 2)
            {
                printf("BT CONTROL:");
                for (i = 0; i < bt_control_size; i++)
                    printf(" 0x%02x", bt_control[i]);
                printf("\n");
            }
            send_usb_req(sockfd,usb_req, NULL, bt_control_size, 0);
            process_bt_control(sockfd, usb_req, bt_control_size, bt_control, -1);
        }
    }
}

void handle_unlink(int sockfd, USBIP_RET_SUBMIT *usb_req, int bl)
{
    USBIP_RET_UNLINK ret = {0};
    
    /* rfkill block operation */
    printf("rfkill block\n");
    ret.command=htonl(0x04);
    ret.seqnum=htonl(usb_req->seqnum);
    ret.status=htonl(0xffffff98);

    if (verbose_level > 1)
        print_msg((char *)&ret, sizeof(USBIP_RET_UNLINK),"USBIP_RET_UNLINK", 0);
    if (send (sockfd, (char *)&ret, sizeof(USBIP_RET_UNLINK), 0) != sizeof(USBIP_RET_UNLINK))
    {
        printf ("send error : %s \n", strerror (errno));
        exit(-1);
    }
    scanning = 0;
}

static void configure_usb_bd_address(void)
{
    int i;
    /* Get interface MAC address to filter */
    sscanf(usb_bd_address, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
        &bd_addr[0], &bd_addr[1], &bd_addr[2], &bd_addr[3], &bd_addr[4], &bd_addr[5]);

    for(i = 0; i < BD_ADDRESS_SIZE; i++)
    {
        bt_control_array[BD_ADDRESS_BT_CONTROL_INDEX].tail[i] = bd_addr[BD_ADDRESS_SIZE - 1 -i];
    }
}

static void int_handler(int signo)
{
    if (signo == SIGTERM)
    {
        printf("TERM\n");
    }
    fflush(stdout);
    exit(0);
}

static const char* const pcShortOptions = "hvp:b:a:";
static const struct option stLongOptions[] =
{
    {"help"                        , 0, 0, 'h'},
    {"verbose"                     , 0, 0, 'v'},
    {"port"                        , 1, 0, 'p'},
    {"bus"                         , 1, 0, 'b'},
    {"address"                     , 1, 0, 'a'},
    {0, 0, 0, 0}
};

static void help()
{
    printf("help:\n");
    printf("[-h] [-p <tcp port>] [-b <bus-port:...>] [-e <ethernet address>] [-i <network_interface>]\n\n");
    printf("--help/-v                                  Show this help\n");
    printf("--verbose/-h                               Set verbose mode\n");
    printf("--port/-p <tcp port>                       Tcp port for usbip server\n");
    printf("--bus/-b <bus-port:...>                    Usb bus and port for emulation\n");
    printf("--address/-a <xx:xx:xx:xx:xx:xx>           BD address for emulated device\n");
    printf("\n");
}

int main(int argc, char **argv)
{
    int32_t iOption = -1;

    signal(SIGINT, int_handler);
    signal(SIGTERM, int_handler);

    /* Get options */
    do
    {
        iOption = getopt_long(argc, argv, pcShortOptions, stLongOptions, 0);
        switch (iOption)
        {
            case 'h':
                help();
                return 0;
            case 'v':
                verbose_level ++;
                break;
            case 'p':
                server_usbip_tcp_port = atoi(optarg);
                break;
            case 'b':
                strncpy(usb_bus_port, optarg, MAX_USB_BUS_PORT_SIZE-1);
                break;
            case 'a':
                strncpy(usb_bd_address, optarg, BD_ADDRESS_SIZE * 3 - 1);
                break;
            case -1:
                break;
            default:
                printf("unknown command\n");
                exit(255);
        }
    }
    while (iOption != -1);
    printf("bt started....\n");
    printf("server usbip tcp port: %d\n", server_usbip_tcp_port);
    printf("Bus-Port: %s\n", usb_bus_port);
    configure_usb_bus_port();
    printf("BD address: %s\n", usb_bd_address);
    configure_usb_bd_address();
    printf("Manufacturer: %s\n", manufacturer);
    usbip_run(&dev_dsc);
}

