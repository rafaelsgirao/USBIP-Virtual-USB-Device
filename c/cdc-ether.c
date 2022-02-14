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

   For e-mail suggestions :  jtornosm@redhat.com
   ######################################################################## */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <getopt.h>

#include "usbip.h"

/* Device Descriptor */
const USB_DEVICE_DESCRIPTOR dev_dsc=
{
    0x12,                   // Size of this descriptor in bytes
    0x01,                   // DEVICE descriptor type
    0x0210,                 // USB Spec Release Number in BCD format
    0x00,                   // Class Code
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    0x40,                   // Max packet size for EP0, see usb_config.h
    0x0fe6,                 // Vendor ID (ICS Advent)
    0x9900,                 // Product ID
    0x2000,                 // Device release number in BCD format
    0x01,                   // Manufacturer string index
    0x02,                   // Product string index
    0x03,                   // Device serial number string index
    0x02                    // Number of possible configurations
};

const USB_DT_BOS_DEVICE_CAPABILITY dt_bos_device_cap={{
    0x05,                   // Size of this descriptor in bytes
    0x0f,                   // Device descriptor type
    0x000c,                 // Total length
    0x01                    // Num device caps
    },{
    0x07,                   // Size of this descriptor in bytes
    0x10,                   // Device descriptor type
    0x02,                   // Device capability type
    0x02,
    0x00,
    0x00,
    0x00
}};

//Configuration1
typedef struct __attribute__ ((__packed__)) _CONFIG1_CDC_ETHER
{
    USB_CONFIGURATION_DESCRIPTOR dev_conf0;
    USB_INTERFACE_DESCRIPTOR dev_int0;
    USB_ENDPOINT_DESCRIPTOR dev_ep0;
    USB_ENDPOINT_DESCRIPTOR dev_ep1;
    USB_ENDPOINT_DESCRIPTOR dev_ep2;
} CONFIG1_CDC_ETHER;

//Configuration2
typedef struct __attribute__ ((__packed__)) _CONFIG2_CDC_ETHER
{
    USB_CONFIGURATION_DESCRIPTOR dev_conf0;
    USB_INTERFACE_DESCRIPTOR dev_int0;
    USB_CDC_HEADER_FN_DSC cdc_header;
    USB_CDC_UNION_FN_DSC cdc_union;
    USB_CDC_ETHER_FN_DSC cdc_ether;
    USB_ENDPOINT_DESCRIPTOR dev_ep0;
    USB_INTERFACE_DESCRIPTOR dev_int1;
    USB_INTERFACE_DESCRIPTOR dev_int2;
    USB_ENDPOINT_DESCRIPTOR dev_ep1;
    USB_ENDPOINT_DESCRIPTOR dev_ep2;
} CONFIG2_CDC_ETHER;

/* Configuration 1 Descriptor */
const CONFIG1_CDC_ETHER  configuration1_cdc={{
    /* Configuration Descriptor */
    0x09,//sizeof(USB_CFG_DSC),    // Size of this descriptor in bytes
    USB_DESCRIPTOR_CONFIGURATION,                // CONFIGURATION descriptor type
    sizeof(CONFIG1_CDC_ETHER),                 // Total length of data for this cfg
    1,                      // Number of interfaces in this cfg
    1,                      // Index value of this configuration
    0,                      // Configuration string index
    0x80,
    0x32,                     // Max power consumption (2X mA)
    },{
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    0,                      // Interface Number
    0,                      // Alternate Setting Number
    3,                      // Number of endpoints in this intf
    0xff,                   // Class code Communications
    0xff,                   // Subclass code Ethernet Networking
    0x00,                   // Protocol code
    0x00                    // Interface string index CDC Communications Control
    },{
     /* Endpoint Descriptor */
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    0x81,                       //EndpointAddress 1 IN
    0x02,                       //Attributes
    0x0200,                     //size
    0x00                        //Interval
    },{
     /* Endpoint Descriptor */
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    0x02,                       //EndpointAddress 2 OUT
    0x02,                       //Attributes
    0x0200,                     //size
    0x00                        //Interval
    },{
     /* Endpoint Descriptor */
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    0x83,                       //EndpointAddress 3 IN
    0x03,                       //Attributes
    0x0002,                     //size
    0x08                        //Interval
}};

/* Configuration 2 Descriptor */
const CONFIG2_CDC_ETHER  configuration2_cdc={{
    /* Configuration Descriptor */
    0x09,//sizeof(USB_CFG_DSC),    // Size of this descriptor in bytes
    USB_DESCRIPTOR_CONFIGURATION,                // CONFIGURATION descriptor type
    sizeof(CONFIG2_CDC_ETHER),                 // Total length of data for this cfg
    2,                      // Number of interfaces in this cfg
    2,                      // Index value of this configuration
    0,                      // Configuration string index
    0xa0,
    0x32,                     // Max power consumption (2X mA)
    },{
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    0,                      // Interface Number
    0,                      // Alternate Setting Number
    1,                      // Number of endpoints in this intf
    0x02,                   // Class code Communications
    0x06,                   // Subclass code Ethernet Networking
    0x00,                   // Protocol code
    5                       // Interface string index CDC Communications Control
    },{
    /* CDC Class-Specific Descriptors */
    /* Header Functional Descriptor */
    0x05,                   // bFNLength
    0x24,                   // bDscType
    0x00,                   // bDscSubType
    0x0110                  // bcdCDC (1.10)
    },{
    /* Union Functional Descriptor */
    0x05,                   // bFNLength
    0x24,                   // bDscType
    0x06,                   // bDscSubType
    0x00,                   // bMasterIntf
    0x01                    // bSlaveIntf
    },{
    /* Ethernet Functional Descriptor */
    0x0d,                   // bFNLength
    0x24,                   // bDscType
    0x0f,                   // bDscSubType 
    0x03,                   // MACAddress
    0x00000000,             // bmEthernetStatistics
    1514,                   // wMaxSegmentSize
    0x0000,                 // wNumberMCFilters
    0x00                    // bNumberPowerFilters
    },{
    /* Endpoint Descriptor */
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    0x83,                       //EndpointAddress 3 IN
    0x03,                       //Attributes
    0x0010,                     //size
    0x08                        //Interval
    },{
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    1,                      // Interface Number
    0,                      // Alternate Setting Number
    0,                      // Number of endpoints in this intf
    0x0A,                   // Class code CDC Data
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    0x00,                      // Interface string index
    },{
    /* Interface Descriptor */
    0x09,//sizeof(USB_INTF_DSC),   // Size of this descriptor in bytes
    USB_DESCRIPTOR_INTERFACE,               // INTERFACE descriptor type
    1,                      // Interface Number
    1,                      // Alternate Setting Number
    2,                      // Number of endpoints in this intf
    0x0A,                   // Class code CDC Data
    0x00,                   // Subclass code
    0x00,                   // Protocol code
    0x04,                   // Interface string index
    },{
    /* Endpoint Descriptor */
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    0x81,                       //EndpointAddress 1 IN
    0x02,                       //Attributes
    0x0200,                     //size
    0x00                        //Interval
    },{
    /* Endpoint Descriptor */
    0x07,/*sizeof(USB_EP_DSC)*/
    USB_DESCRIPTOR_ENDPOINT,    //Endpoint Descriptor
    0x02,                       //EndpointAddress 2 OUT
    0x02,                       //Attributes
    0x0200,                     //size
    0x00                        //Interval
}};

const char *configuration[] = {(const char *)&configuration1_cdc, (const char *)&configuration2_cdc}; 

const unsigned char string_0[] = { // available languages  descriptor
		0x04,                  
                USB_DESCRIPTOR_STRING, 
		0x09, 
                0x04 
		};

const unsigned char string_1[] = { //Manufacturer
                0x1e, 
                USB_DESCRIPTOR_STRING, // bLength, bDscType
                'V', 0x00, //
                'i', 0x00, //
                'r', 0x00, //
                't', 0x00, //
                'u', 0x00, //
                'a', 0x00, //
                'l', 0x00, //
                ' ', 0x00, //
                'D', 0x00, //
                'e', 0x00, //
                'v', 0x00, //
                'i', 0x00, //
                'c', 0x00, //
                'e', 0x00, //
                };

const unsigned char string_2[] = { //Product
                0x10,
                USB_DESCRIPTOR_STRING, //
                'U', 0x00, //
                'S', 0x00, //
                'B', 0x00, //
                ' ', 0x00, //
                'L', 0x00, //
                'A', 0x00, //
                'N', 0x00, //
                };

unsigned char string_3[] = { //Serial Number
                0x1a, 
                USB_DESCRIPTOR_STRING, //
                '0', 0x00, //
                '0', 0x00, //
                '0', 0x00, //
                '0', 0x00, //
                '0', 0x00, //
                '0', 0x00, //
                '0', 0x00, //
                '0', 0x00, //
                '0', 0x00, //
                '0', 0x00, //
                '0', 0x00, //
                '0', 0x00, //
                };

const unsigned char string_4[] = {
                0x1c,
                USB_DESCRIPTOR_STRING, //
                'E', 0x00, //
                't', 0x00, //
                'h', 0x00, //
                'e', 0x00, //
                'r', 0x00, //
                'n', 0x00, //
                'e', 0x00, //
                't', 0x00, //
                ' ', 0x00, //
                'D', 0x00, //
                'a', 0x00, //
                't', 0x00, //
                'a', 0x00, //
                };

const unsigned char string_5[] = {
		0x36, 
                USB_DESCRIPTOR_STRING, //
		'C', 0x00, //
		'D', 0x00, //
		'C', 0x00, //
		' ', 0x00, //
		'C', 0x00, //
		'o', 0x00, //
		'm', 0x00, //
		'm', 0x00, //
                'u', 0x00, //
                'n', 0x00, //
                'i', 0x00, //
                'c', 0x00, //
                'a', 0x00, //
                't', 0x00, //
                'i', 0x00, //
                'o', 0x00, //
                'n', 0x00, //
                's', 0x00, //
                ' ', 0x00, //
                'C', 0x00, //
                'o', 0x00, //
                'n', 0x00, //
                't', 0x00, //
                'r', 0x00, //
                'o', 0x00, //
                'l', 0x00, //
		};

const unsigned char *strings[]={string_0, string_1, string_2, string_3, string_4, string_5};

#undef DEBUG_EP_ALL
#undef DEBUG_EP_DATA
#define MAX_DATA_BUFFER_SIZE 10000
static char tx_data_buffer[MAX_DATA_BUFFER_SIZE];
static int tx_data_sockfd = -1;
static struct sockaddr_ll tx_data_socket_address;
#undef DEBUG_TX_DATA
static int rx_data_sockfd = -1;
static char rx_data_buffer[MAX_DATA_BUFFER_SIZE];
static pthread_t rx_data_threadId;
static int rx_data_driver_sockfd = -1;
static unsigned char mac_addr[ETH_ALEN] = {0};
static unsigned char broadcast_mac_addr[ETH_ALEN] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
static int rx_data_current_send_seq_pos = 0;
static int rx_data_next_send_seq_pos = 0;
#define RX_DATA_SEND_SEQ_ARRAY_SIZE 100000
static int rx_data_send_seq_array[RX_DATA_SEND_SEQ_ARRAY_SIZE] = { [0 ... (RX_DATA_SEND_SEQ_ARRAY_SIZE - 1)] = -1};
static int rx_data_send_size_array[RX_DATA_SEND_SEQ_ARRAY_SIZE] = {0};
static pthread_mutex_t rx_data_send_mutex;
static int rx_data_ifidx = 0;
#undef DEBUG_RX_DATA

unsigned short server_usbip_tcp_port = 3240;
char usb_bus_port[MAX_USB_BUS_PORT_SIZE] = "1-1";
static char usb_ethernet_address[] = "88:00:66:99:5b:e9";
#define MAX_MANUFACTURER_SIZE 32
static char manufacturer[MAX_MANUFACTURER_SIZE] = "Temium";
static char if_name[IFNAMSIZ] = "enp1s0";

static uint32_t net_checksum_add(int len, uint8_t *buf)
{
    uint32_t sum = 0;
    int i;

    for (i = 0; i < len; i++) {
	if (i & 1)
	    sum += (uint32_t)buf[i];
	else
	    sum += (uint32_t)buf[i] << 8;
    }
    return sum;
}

static uint16_t net_checksum_finish(uint32_t sum)
{
    while (sum>>16)
	sum = (sum & 0xFFFF)+(sum >> 16);
    return ~sum;
}

static uint16_t net_checksum_tcpudp(uint16_t length, uint16_t proto,
                             uint8_t *addrs, uint8_t *buf)
{
    uint32_t sum = 0;

    sum += net_checksum_add(length, buf);         // payload
    sum += net_checksum_add(8, addrs);            // src + dst address
    sum += proto + length;                        // protocol & length
    return net_checksum_finish(sum);
}

static void send_usb_req_data(int sockfd, USBIP_RET_SUBMIT * usb_req, char * data, unsigned int size, unsigned int status)
{
    struct ether_header *eh = (struct ether_header *)data;
    struct iphdr *iph = (struct iphdr *)(data + sizeof(struct ether_header));
    struct udphdr *udph = (struct udphdr *)(data + sizeof(struct ether_header) + sizeof(struct iphdr));
    struct tcphdr *tcph = (struct tcphdr *)(data + sizeof(struct ether_header) + sizeof(struct iphdr));
    int iph_len = 0;

    /* Fix checksum */
    if (eh->ether_type == htons(ETH_P_IP))
    {
        if (iph->version != 4)
        {
            printf("packet is not ipv4\n");
	    return;
        }
        iph_len = iph->ihl * 4;
        if (iph->protocol == IPPROTO_UDP)
        {
            udph = (struct udphdr *)(data + sizeof(struct ether_header) + iph_len);
            udph->check = 0;
            udph->check = net_checksum_tcpudp(ntohs(iph->tot_len) - iph_len, iph->protocol, (uint8_t *)(&(iph->saddr)), (uint8_t *)udph);
#ifdef DEBUG_RX_DATA
            printf("udp checksum %x\n", udph->check);
#endif
            udph->check = htons(udph->check);
        }
        else if (iph->protocol == IPPROTO_TCP)
        {
            tcph = (struct tcphdr *)(data + sizeof(struct ether_header) + iph_len);
            tcph->check = 0;
            tcph->check = net_checksum_tcpudp(ntohs(iph->tot_len) - iph_len, ntohs(iph->protocol), (uint8_t *)(&(iph->saddr)), (uint8_t *)tcph);
#ifdef DEBUG_RX_DATA
            printf("tcp checksum %x\n", tcph->check); 
#endif
            tcph->check = htons(tcph->check);
        }
    }

    /* Look for available sequence number */
    while(1)
    {
        pthread_mutex_lock(&rx_data_send_mutex);
        if (rx_data_send_seq_array[rx_data_current_send_seq_pos] != -1)
        {
            if (rx_data_send_size_array[rx_data_current_send_seq_pos] < size)
            {
                rx_data_send_seq_array[rx_data_current_send_seq_pos] = -1;
                printf("not enough granted data (%d/%d)\n", size, rx_data_send_size_array[rx_data_current_send_seq_pos]);
            }
            else
            {
                usb_req->seqnum = rx_data_send_seq_array[rx_data_current_send_seq_pos];
                rx_data_send_seq_array[rx_data_current_send_seq_pos] = -1;
                rx_data_current_send_seq_pos = (rx_data_current_send_seq_pos + 1) % RX_DATA_SEND_SEQ_ARRAY_SIZE;
                pthread_mutex_unlock(&rx_data_send_mutex);
                break;
            }
        }
        pthread_mutex_unlock(&rx_data_send_mutex);
        usleep(0);
    }
#ifdef DEBUG_RX_DATA
    printf("Send data to usbip: seq %d size %d\n", usb_req->seqnum, size);
#endif
    /* Forward to usbip */
    send_usb_req(sockfd, usb_req, data, size, status);
}

static int configure_tx_data()
{
    struct ifreq if_idx;

    /* Open RAW socket to send on */
    if ((tx_data_sockfd  = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1)
    {
        perror("socket");
        exit(-1);
    }

    /* Get the index of the interface to send on */
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, if_name, IFNAMSIZ-1);
    if (ioctl(tx_data_sockfd, SIOCGIFINDEX, &if_idx) < 0)
    {
        perror("SIOCGIFINDEX");
        exit(-1);
    }
    memset(&tx_data_socket_address, 0, sizeof(struct sockaddr_ll));
    tx_data_socket_address.sll_family = htons(AF_PACKET);
    tx_data_socket_address.sll_protocol = htons(ETH_P_ALL);
    tx_data_socket_address.sll_ifindex = if_idx.ifr_ifindex;
    tx_data_socket_address.sll_halen = ETH_ALEN;

    return 0;
}

static int send_tx_data(char * data, unsigned int len)
{
    struct ether_header *eh = (struct ether_header *)data;
#ifdef DEBUG_TX_DATA
    unsigned short i;
    
    printf("tx data from %02x:%02x:%02x:%02x:%02x:%02x (%d bytes)\n", 
        (unsigned char)data[0], (unsigned char)data[1], (unsigned char)data[2], 
        (unsigned char)data[3], (unsigned char)data[4], (unsigned char)data[5], len);
#endif

#ifdef DEBUG_TX_DATA 
    for(i=0; i < len; i ++)
    {
        printf("%02x ", (unsigned char)data[i]);
    }
    printf("\n");
#endif

    if (memcmp(eh->ether_shost, mac_addr, ETH_ALEN) != 0)
    {
#ifdef DEBUG_TX_DATA
        printf("wrong destination, drop tx data\n");
#endif
        return 0;
    }

    /* Configure destination MAC */
    memcpy(tx_data_socket_address.sll_addr, data, ETH_ALEN);
    
    /* Send packet */
    if (sendto(tx_data_sockfd, data, len, 0, (struct sockaddr*)&tx_data_socket_address, sizeof(struct sockaddr_ll)) < 0)
    {
        printf("tx data sending failed\n");
        exit(-1);
    }

    return 0;
}

static int configure_rx_data()
{
    struct ifreq ifopts;
    int sockopt;
    struct ifreq if_idx;
    struct sockaddr_ll addr;

    if (pthread_mutex_init(&rx_data_send_mutex, NULL) != 0) {                                  
        perror("pthread_mutex_init() error");                                       
        exit(-12);                                                                    
    }      

    /* Open PF_PACKET socket, listening for EtherType */
    if ((rx_data_sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) == -1) {
        perror("listener: socket");
        exit(-2);
    }

    /* Set interface to promiscuous mode */
    strncpy(ifopts.ifr_name, if_name, IFNAMSIZ-1);
    ioctl(rx_data_sockfd, SIOCGIFFLAGS, &ifopts);
    ifopts.ifr_flags |= IFF_PROMISC;
    if (ioctl(rx_data_sockfd, SIOCSIFFLAGS, &ifopts) < 0)
    {
        perror("SIOCSIFFLAGS");
        exit(-1);
    }

    /* Allow the socket to be reused - incase connection is closed prematurely */
    if (setsockopt(rx_data_sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1)
    {
        perror("SO_REUSEADDR");
        close(rx_data_sockfd);
        exit(-2);
    }

    /* Bind to device */
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, if_name, IFNAMSIZ-1);
    if (ioctl(rx_data_sockfd, SIOCGIFINDEX, &if_idx) < 0)
    {
        perror("SIOCGIFINDEX");
        exit(-1);
    }
    rx_data_ifidx = if_idx.ifr_ifindex;
    memset(&addr,0,sizeof(addr));
    addr.sll_family = htons(AF_PACKET);
    addr.sll_ifindex = rx_data_ifidx;
    if (setsockopt(rx_data_sockfd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&addr, sizeof(addr)) == -1)
    {
        perror("SO_BINDTODEVICE");
        close(rx_data_sockfd);
        exit(-2);
    }

    return 0;
}

static int receive_rx_data(char * data, int size)
{
    int num_bytes = 0;
    struct ether_header *eh = (struct ether_header *)data;
#ifdef DEBUG_RX_DATA
    unsigned short i;
#endif
    struct sockaddr_ll saddr;
    int saddr_len = sizeof (saddr);

    if ((num_bytes = recvfrom(rx_data_sockfd, data, size, 0, (void*)&saddr, (socklen_t *)&saddr_len)) < 0)
    {
        printf("rx data receiving failed\n");
        exit(-1);
    }
    if ((num_bytes) &&
        (saddr.sll_ifindex == rx_data_ifidx) &&
        ((memcmp(eh->ether_dhost, mac_addr, ETH_ALEN) == 0) || (memcmp(eh->ether_dhost, broadcast_mac_addr, ETH_ALEN) == 0)))
    {
#ifdef DEBUG_RX_DATA
        printf("rx data from %02x:%02x:%02x:%02x:%02x:%02x (%d bytes)\n",
            eh->ether_dhost[0], eh->ether_dhost[1], eh->ether_dhost[2], 
            eh->ether_dhost[3], eh->ether_dhost[4], eh->ether_dhost[5], num_bytes);
        printf("  sll: family %x protocol %x index %x hatype %x type %x len %x addr %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x\n", 
            saddr.sll_family, saddr.sll_protocol, saddr.sll_ifindex, saddr.sll_hatype, saddr.sll_pkttype, saddr.sll_halen, 
            saddr.sll_addr[0], saddr.sll_addr[1], saddr.sll_addr[2], saddr.sll_addr[3],
            saddr.sll_addr[4], saddr.sll_addr[5], saddr.sll_addr[6], saddr.sll_addr[7]);
        for(i=0; i < num_bytes; i ++)
        {
            printf("%02x ", (unsigned char)rx_data_buffer[i]);
        }
        printf("\n");
#endif
        return num_bytes;
    }

    return 0;
}

static  void * rx_data_thread(void * psockfd)
{
    int num_bytes;
    USBIP_RET_SUBMIT usb_req;
    
    printf("Listenning to %02x:%02x:%02x:%02x:%02x:%02x ...\n", 
        mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

    while(1)
    {
        num_bytes = receive_rx_data(rx_data_buffer, MAX_DATA_BUFFER_SIZE);
        if (num_bytes)
        {
            send_usb_req_data(*((int *)psockfd), &usb_req, rx_data_buffer, num_bytes, 0);
        }
    }

    return NULL;
}

static int start_rx_data(int sockfd)
{ 
    int err;
    static unsigned char started = 0;

    if (started)
    {
        return 0;
    }

    rx_data_driver_sockfd = sockfd;
    if ((err = pthread_create(&rx_data_threadId, NULL, &rx_data_thread, &rx_data_driver_sockfd)))
    {
        printf("rx_data thread craation failed: %s\n", strerror(err));
        exit(-3);
    }

    started = 1;
    
    return 0;
}

void handle_data(int sockfd, USBIP_RET_SUBMIT *usb_req, int bl)
{
#ifndef DEBUG_EP_ALL
    if (usb_req->ep != 3)
#endif
#if defined(DEBUG_EP_ALL) || defined(DEBUG_EP_DATA)
    printf("ep%d direction %d seq %d size %d\n", usb_req->ep, usb_req->direction, usb_req->seqnum, bl);
#endif

    if((usb_req->ep == 0x03))
    {
        if (usb_req->direction == 1) //output
        {
            send_usb_req(sockfd, usb_req,"", 0, 0);
        }
    }
    if((usb_req->ep == 0x01))
    {
        if (usb_req->direction == 1) //output
        {
            if (bl)
            {
                pthread_mutex_lock(&rx_data_send_mutex);
                rx_data_send_seq_array[rx_data_next_send_seq_pos] = usb_req->seqnum;
                rx_data_send_size_array[rx_data_next_send_seq_pos] = bl;
                rx_data_next_send_seq_pos = (rx_data_next_send_seq_pos + 1) % RX_DATA_SEND_SEQ_ARRAY_SIZE;
                pthread_mutex_unlock(&rx_data_send_mutex);
#ifdef DEBUG_RX_DATA
                printf("granted data: seq %d size %d\n", usb_req->seqnum, bl);
#endif
            }
        }
    }
    if((usb_req->ep == 0x02))
    {
        if ((usb_req->direction == 0) && bl) //input
        {
            if ((recv (sockfd, tx_data_buffer, bl, 0)) != bl)
            {
                printf ("receive error : %s \n", strerror (errno));
                return;
            }
            send_usb_req(sockfd, usb_req, NULL, bl, 0);
            send_tx_data(tx_data_buffer, bl);
        }   
    }
}

void handle_unknown_control(int sockfd, StandardDeviceRequest * control_req, USBIP_RET_SUBMIT *usb_req)
{
    if(control_req->bmRequestType == 0x21)
    {
        if(control_req->bRequest == 0x43)
        {
            send_usb_req(sockfd,usb_req,(char *)"",0,0);
            start_rx_data(sockfd);
        }
    }
}

static void configure_usb_ethernet_address(void)
{
    /* Get interface MAC address to filter */
    sscanf(usb_ethernet_address, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", 
        &mac_addr[0], &mac_addr[1], &mac_addr[2], &mac_addr[3], &mac_addr[4], &mac_addr[5]);

    /* Configure string for usbip string */
    string_3[2] = usb_ethernet_address[0];
    string_3[4] = usb_ethernet_address[1];
    string_3[6] = usb_ethernet_address[3];
    string_3[8] = usb_ethernet_address[4];
    string_3[10] = usb_ethernet_address[6];
    string_3[12] = usb_ethernet_address[7];
    string_3[14] = usb_ethernet_address[9];
    string_3[16] = usb_ethernet_address[10];
    string_3[18] = usb_ethernet_address[12];
    string_3[20] = usb_ethernet_address[13];
    string_3[22] = usb_ethernet_address[15];
    string_3[24] = usb_ethernet_address[16];
}

static const char* const pcShortOptions = "hp:b:e:i:";
static const struct option stLongOptions[] =
{
    {"help"                        , 0, 0, 'h'},
    {"port"                        , 1, 0, 'p'},
    {"bus"                         , 1, 0, 'b'},
    {"ethernet_address"            , 1, 0, 'e'},
    {"interface"                   , 1, 0, 'i'},
    {0, 0, 0, 0}
};

static void help()
{
    printf("help:\n");
    printf("[-h] [-p <tcp port>] [-b <bus-port:...>] [-e <ethernet address>] [-i <network_interface>]\n\n");
    printf("--help/-h                                  Show this help\n");
    printf("--port/-p <tcp port>                       Tcp port for usbip server\n");
    printf("--bus/-b <bus-port:...>                    Usb bus and port for emulation\n");
    printf("--ethernet_address/-e <xx:xx:xx:xx:xx:xx>  Ethernet address for emulated device\n");
    printf("--interface/-i <network_interface>         Network interface to bind for emulator data plane\n");
    printf("\n");
}

int main(int argc, char **argv)
{
    int32_t iOption = -1;

    /* Get options */
    do
    {
        iOption = getopt_long(argc, argv, pcShortOptions, stLongOptions, 0);
        switch (iOption)
        {
            case 'h':
                help();
                return 0;
            case 'p':
                server_usbip_tcp_port = atoi(optarg);
                break;
            case 'b':
                strncpy(usb_bus_port, optarg, MAX_USB_BUS_PORT_SIZE-1);
                break;
            case 'e':
                strncpy(usb_ethernet_address, optarg, ETH_ALEN * 3 - 1);
                break;
            case 'i':
                strncpy(if_name, optarg, IFNAMSIZ-1);
                break;
            case -1:
                break;
            default:
                printf("unknown command\n");
                exit(255);
        }
   }
   while (iOption != -1);
   printf("cdc-ether started....\n");
   printf("server usbip tcp port: %d\n", server_usbip_tcp_port);
   printf("Bus-Port: %s\n", usb_bus_port);
   configure_usb_bus_port();
   printf("Ethernet address: %s\n", usb_ethernet_address);
   configure_usb_ethernet_address();
   printf("Manufacturer: %s\n", manufacturer);
   printf("Network interface to bind: %s\n", if_name);
   configure_tx_data(); 
   configure_rx_data(); 
   usbip_run(&dev_dsc);
}
