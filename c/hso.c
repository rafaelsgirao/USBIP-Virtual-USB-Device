/* ########################################################################

   USBIP hardware emulation 

   ########################################################################

   Copyright (c) : 2016  Luis Claudio Gambôa Lopes

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

   For e-mail suggestions :  lcgamboa@yahoo.com
   ######################################################################## */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "usbip.h"

/* Device Descriptor */
const USB_DEVICE_DESCRIPTOR dev_dsc=
{
    0x12,                   // Size of this descriptor in bytes
    0x01,                   // DEVICE descriptor type
    0x0200,                 // USB Spec Release Number in BCD format
    0xe0,                   // Class Code
    0x01,                   // Subclass code
    0x02,                   // Protocol code
    0x10,                   // Max packet size for EP0, see usb_config.h
    0x0af0,                 // Vendor ID
    0x6711,                 // Product ID
    0x0100,                 // Device release number in BCD format
    0x00,                   // Manufacturer string index
    0x00,                   // Product string index
    0x00,                   // Device serial number string index
    0x01                    // Number of possible configurations
};

const USB_DEVICE_QUALIFIER_DESCRIPTOR dev_qua = { 
    0x0A,       // bLength
    0x06,       // bDescriptorType
    0x0200,     // bcdUSB 
    0xe0,       // bDeviceClass
    0x00,       // bDeviceSubClass
    0x00,       // bDeviceProtocol
    0x10,       // bMaxPacketSize
    0x01,       // iSerialNumber
    0x00        //bNumConfigurations*/
};

#define USB_DESCRIPTOR_INTERFACE_EXTRA 0x10

//Configuration
typedef struct __attribute__ ((__packed__)) _USB_INTERFACE_DESCRIPTOR_EXTRA
{
    byte bLength;               
    byte bDescriptorType;       
    byte bCappabilityType;      
} USB_INTERFACE_DESCRIPTOR_EXTRA;

//Configuration
typedef struct __attribute__ ((__packed__)) _CONFIG_HSO
{
 USB_CONFIGURATION_DESCRIPTOR dev_conf0;
 USB_INTERFACE_DESCRIPTOR dev_int0;
 USB_INTERFACE_DESCRIPTOR_EXTRA dev_int0_extra;
 USB_ENDPOINT_DESCRIPTOR dev_ep0;
 USB_ENDPOINT_DESCRIPTOR dev_ep1;
 USB_ENDPOINT_DESCRIPTOR dev_ep2;
} CONFIG_HSO;

const CONFIG_HSO  configuration_hso={{
    /* Configuration Descriptor */
    0x09,//sizeof(USB_CFG_DSC),    // Size of this descriptor in bytes
    USB_DESCRIPTOR_CONFIGURATION,                // CONFIGURATION descriptor type
    sizeof(CONFIG_HSO),                 // Total length of data for this cfg
    1,                      // Number of interfaces in this cfg
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
    0xff,                   // Class code
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    0                       // Interface string index
    },{
    /* Interface Descriptor Extra */
    0x03,
    USB_DESCRIPTOR_INTERFACE_EXTRA,
    0
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x81,                   // Address (USB_ENDPOINT_DIR_MASK)
    0x02,                   // Attributes -> Change to 0x03 to generate bug in hso_create_net_device
    0x0040,                 // Size
    0x0A                    // Interval
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x02,                   // Address (USB_ENDPOINT_DIR_MASK)
    0x02,                   // Attributes
    0x0040,                 // Size
    0x0A                    // Interval
    },{
    /* Endpoint Descriptor */
    0x07,//USB_DT_ENDPOINT_SIZE,   // Size of this endpoint descriptor in bytes
    USB_DESCRIPTOR_ENDPOINT,       // Endpoint Type (USB_ENDPOINT_XFER_BULK)
    0x82,                   // Address (USB_ENDPOINT_DIR_MASK)
    0x03,                   // Attributes
    0x0040,                 // Size
    0x0A                    // Interval
}};

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
		0x10, 
                USB_DESCRIPTOR_STRING, //
		'U', 0x00, //
		'S', 0x00, //
		'B', 0x00, //
		' ', 0x00, //
		'H', 0x00, //
		'S', 0x00, //
		'O', 0x00, //
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


const char *configuration = (const char *)&configuration_hso; 

const USB_INTERFACE_DESCRIPTOR *interfaces[]={ &configuration_hso.dev_int0};

const unsigned char *strings[]={string_0, string_1, string_2, string_3};


#define BSIZE 64 
char buffer[BSIZE+1];
int  bsize=0;


void handle_data(int sockfd, USBIP_RET_SUBMIT *usb_req, int bl)
{ 
  /* TODO Same data handling than cdc-acm, not used but ... */	
  printf("HSO DATA TEST\n");

  if(usb_req->ep == 0x01)
  {  
    printf("EP1 received \n"); 
    
    if(usb_req->direction == 0) //input
    { 
      printf("direction=input\n");  
      bsize=recv (sockfd, (char *) buffer , bl, 0);
      send_usb_req(sockfd, usb_req,"", 0, 0);
      buffer[bsize+1]=0; //string terminator
      printf("received (%s)\n",buffer);
    }
    else
    {    
        printf("direction=output\n");  
    }
    //not supported
    send_usb_req(sockfd, usb_req, "", 0, 0);
    usleep(500);
  }
  
  if((usb_req->ep == 0x02))
  {
    printf("EP2 received \n"); 
    if(usb_req->direction == 0) //input
    { 
      int i;
      printf("direction=input\n");  
      bsize=recv (sockfd, (char *) buffer , bl, 0);
      send_usb_req(sockfd, usb_req,"", 0, 0);
      buffer[bsize+1]=0; //string terminator
      printf("received (%s)\n",buffer);
      for(i=0;i<bsize;i++)
          printf("%02X",(unsigned char)buffer[i]);
      printf("\n");       
    }
    else //output
    {
      printf("direction=output\n");  
      if(bsize != 0)
      {   
        int i;
        for(i=0;i<bsize;i++)//increment received char
           buffer[i]+=1;

        send_usb_req(sockfd, usb_req, buffer, bsize, 0);    
        printf("sending (%s)\n",buffer);
        bsize=0;
      }
      else
      {
        send_usb_req(sockfd, usb_req,"", 0, 0);    
        usleep(500);
        printf("no data disponible\n");
      }
    }
  }
  
};


typedef struct _LINE_CODING
{
    word dwDTERate;  //in bits per second
    byte bCharFormat;//0-1 stop; 1-1.5 stop; 2-2 stop bits
    byte ParityType; //0 none; 1- odd; 2 -even; 3-mark; 4 -space
    byte bDataBits;  //5,6,7,8 or 16
}LINE_CODING;

LINE_CODING linec;

unsigned short linecs=0;

void handle_unknown_control(int sockfd, StandardDeviceRequest * control_req, USBIP_RET_SUBMIT *usb_req)
{
    printf("HSO CONTROL RequestType 0x%x Request 0x%x\n", control_req->bmRequestType, control_req->bRequest);

    if(control_req->bmRequestType == 0x40)
    {
        if(control_req->bRequest == 0x82)
        {
            if ((recv (sockfd, (char *) &linec , control_req->wLength, 0)) != control_req->wLength)
            {
                printf ("receive error : %s \n", strerror (errno));
                exit(-1);
            };
            send_usb_req(sockfd,usb_req,"",0,0);
        }
    }
}

int main()
{
   printf("hso started....\n");
   usbip_run(&dev_dsc);
}

