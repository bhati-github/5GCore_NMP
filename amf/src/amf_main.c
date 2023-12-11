/******************************************************************************
 * MIT License
 *
 * Copyright (c) [2021] [Abhishek Bhati]
 *
 * Linkedin : https://www.linkedin.com/in/abhishek-bhati-2166b15a/
 *
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE. 
 *
 *****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "app.h"
#include "nmp.h"
#include "nmp_msg_parser.h"
#include "common_util.h"

#include "amf.h"
#include "n1_n2_msg_handler.h"
#include "n4_msg_handler.h"
#include "Namf_msg_handler.h"

amf_config_t   g__amf_config;

uint8_t  g__n1_n2_send_msg_buffer[MSG_BUFFER_LEN];
uint8_t  g__n1_n2_rcvd_msg_buffer[MSG_BUFFER_LEN];

uint8_t  g__n4_send_msg_buffer[MSG_BUFFER_LEN];
uint8_t  g__n4_rcvd_msg_buffer[MSG_BUFFER_LEN];

uint8_t  g__Namf_send_msg_buffer[MSG_BUFFER_LEN];
uint8_t  g__Namf_rcvd_msg_buffer[MSG_BUFFER_LEN];

uint32_t g__ue_ipv4_addr_subnet = 0xc0a80000;
uint32_t g__ue_ipv4_addr_mask   = 0xffff0000;
uint32_t g__ue_ipv4_addr_base   = 0xc0a80000;



uint8_t  g__my_Namf_ip_is_set = 0;
uint8_t  g__my_n1_n2_ip_is_set = 0;
uint8_t  g__smf_n4_ip_is_set = 0;
uint8_t  g__Nnrf_ip_is_set = 0;
uint8_t  g__upf_n3_ip_is_set = 0;
uint8_t  g__upf_n4_ip_is_set = 0;

char *help_string = " AMF...... \n";

int
create_amf_n1_n2_nmp_listener_socket()
{
    struct sockaddr_in  n1_n2_listener_addr;

    // Creating socket file descriptor
    g__amf_config.my_n1_n2_socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    if(g__amf_config.my_n1_n2_socket_id < 0)
    {
        perror("AMF: N1/N2 interface Listener Socket Creation failed");
        return -1;
    }

    memset(&n1_n2_listener_addr, 0, sizeof(struct sockaddr_in));
    n1_n2_listener_addr.sin_family      = AF_INET;
    n1_n2_listener_addr.sin_addr.s_addr = htonl(g__amf_config.my_n1_n2_addr.u.v4_addr);
    n1_n2_listener_addr.sin_port        = htons(UDP_PORT_IS_NMP);

    if(bind(g__amf_config.my_n1_n2_socket_id,
                (struct sockaddr *)&n1_n2_listener_addr,
                sizeof(struct sockaddr_in)) < 0)
    {
        printf("%s: Error in bind() operation \n", __func__);
        perror("Bind_Operation: ");
        close(g__amf_config.my_n1_n2_socket_id);
        return -1;
    }

    printf("N1/N2 interface NMP socket created [Ok] \n");
    return 0;
}


// SMF is integrated into AMF for simplicity in demostration of NMP protocol
// This is the reason N4 interface is connecting AMF directly to UPF.
// In actual 5G core network, AMF connects to SMF via Namf -- Nsmf network.
// SMF in-turns connects to UPF via N4 interface. 
int
create_amf_n4_nmp_listener_socket()
{
    struct sockaddr_in  n4_listener_addr;

    // Creating socket file descriptor
    g__amf_config.smf_n4_socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    if(g__amf_config.smf_n4_socket_id < 0)
    {
        perror("AMF: N4 interface Listener Socket Creation failed");
        return -1;
    }

    memset(&n4_listener_addr, 0, sizeof(struct sockaddr_in));
    n4_listener_addr.sin_family      = AF_INET;
    n4_listener_addr.sin_addr.s_addr = htonl(g__amf_config.smf_n4_addr.u.v4_addr);
    n4_listener_addr.sin_port        = htons(UDP_PORT_IS_NMP);

    if(bind(g__amf_config.smf_n4_socket_id,
            (struct sockaddr *)&n4_listener_addr,
            sizeof(struct sockaddr_in)) < 0)
    {
        printf("%s: Error in bind() operation \n", __func__);
        perror("Bind_Operation: ");
        close(g__amf_config.smf_n4_socket_id);
        return -1;
    }

    printf("N4 interface NMP socket created [Ok] \n");
    return 0;
}


///////////////////////////////////////////////////////////////////
// AMF sends a service registration message to NRF at init time.
///////////////////////////////////////////////////////////////////
int
create_amf_Namf_nmp_listener_socket()
{
    struct sockaddr_in  Namf_listener_addr;

    // Creating socket file descriptor
    g__amf_config.my_Namf_socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    if(g__amf_config.my_Namf_socket_id < 0)
    {
        perror("AMF: Namf interface Listener Socket Creation failed");
        return -1;
    }

    memset(&Namf_listener_addr, 0, sizeof(struct sockaddr_in));
    Namf_listener_addr.sin_family      = AF_INET;
    Namf_listener_addr.sin_addr.s_addr = htonl(g__amf_config.my_Namf_addr.u.v4_addr);
    Namf_listener_addr.sin_port        = htons(UDP_PORT_IS_NMP);

    if(bind(g__amf_config.my_Namf_socket_id,
            (struct sockaddr *)&Namf_listener_addr,
            sizeof(struct sockaddr_in)) < 0)
    {
        printf("%s: Error in bind() operation \n", __func__);
        perror("Bind_Operation: ");
        close(g__amf_config.my_Namf_socket_id);
        return -1;
    }

    printf("Namf interface NMP socket created [Ok] \n");
    return 0;
}


int
validate_rcvd_nmp_msg_on_n1_n2_interface(uint8_t *msg_ptr,
                                         int      buf_len)
{
    if(buf_len < sizeof(nmp_hdr_t))
    {
        printf("%s: Invalid size \n", __func__);
        return -1;
    }

    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)msg_ptr;

    if(NODE_TYPE__AMF != htons(nmp_hdr_ptr->dst_node_type))
    {
        printf("%s: Destination Node of NMP message is not AMF \n", __func__);
        return -1;
    }

    if(g__amf_config.my_id != htons(nmp_hdr_ptr->dst_node_id))
    {
        printf("%s: Destination node id is not equal to my id \n", __func__);
        return -1;
    }

    return 0;
}


// AMF listens for incoming messages on N1/N2 interface 
// from RAN network.
int
listen_for_n1_n2_messages()
{
    int n = 0;
    int len = 0;
    char string[128];
    uint32_t gnb_addr = 0;
    uint16_t gnb_port = 0;
    struct sockaddr_in  gnb_sockaddr;
    nmp_msg_data_t nmp_n1_n2_rcvd_msg_data;

    while(1)
    {	
        ///////////////////////////////////////////////
        // Wait for request messages from gnodeB..
        ///////////////////////////////////////////////
        len = sizeof(struct sockaddr_in);
        memset(&gnb_sockaddr, 0x0, sizeof(struct sockaddr_in));
        n = recvfrom(g__amf_config.my_n1_n2_socket_id,
                    (char *)g__n1_n2_rcvd_msg_buffer,
                     MSG_BUFFER_LEN,
                     MSG_WAITALL,
                    (struct sockaddr *)&(gnb_sockaddr),
                    (socklen_t *)&len);

        gnb_addr = htonl(gnb_sockaddr.sin_addr.s_addr);
        gnb_port = htons(gnb_sockaddr.sin_port);

        if(g__amf_config.debug_switch)
        {
            get_ipv4_addr_string(gnb_addr, string);
            printf("-----------> Rcvd request (%u bytes) from gnodeB (%s:%u) \n", 
                    n, string, gnb_port);
        }

        if(-1 == validate_rcvd_nmp_msg_on_n1_n2_interface(g__n1_n2_rcvd_msg_buffer, n))
        {
            printf("%s: Rcvd N1/N2 message validation failed..Ignore and Continue.. \n", __func__); 
            continue;
        }

        if(-1 == parse_nmp_msg(g__n1_n2_rcvd_msg_buffer, 
                               n, 
                               &(nmp_n1_n2_rcvd_msg_data),
                               g__amf_config.debug_switch))
        {
            printf("%s: Rcvd N1/N2 message parse error..Ignore and Continue..  \n", __func__);
            continue;
        }


        if(-1 == process_rcvd_n1_n2_msg(&(nmp_n1_n2_rcvd_msg_data),
                                        gnb_addr, 
                                        g__amf_config.debug_switch))
        {
            printf("Unable to process rcvd N1/N2 message \n\n");
        }
    }

    return 0;
}


int 
main(int argc, char **argv)
{
    uint16_t gnb_index = 0;
    int arg_num = 0, arg_index = 0;
    char string[128];
    struct in_addr  v4_addr;
    struct in_addr  v4_addr_1;
    struct in_addr  v4_addr_2;
    gnb_data_t *gnb_data_ptr = NULL;

    memset(&g__amf_config, 0x0, sizeof(amf_config_t));

    arg_num   = argc - 1;
    arg_index = 1;

    while(0 != arg_num)
    {
        if(0 == strcmp(argv[arg_index], "-myn1n2ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-myn1n2ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__amf_config.my_n1_n2_addr.ip_version = IP_VER_IS_V4;
                g__amf_config.my_n1_n2_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__my_n1_n2_ip_is_set = 1;
            }
            else
            {
                printf("-myn1n2ip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-myNamfip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-myNamfip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__amf_config.my_Namf_addr.ip_version = IP_VER_IS_V4;
                g__amf_config.my_Namf_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__my_Namf_ip_is_set = 1;
            }
            else
            {
                printf("-myNamfip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-Nnrfip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-Nnrfip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__amf_config.Nnrf_addr.ip_version = IP_VER_IS_V4;
                g__amf_config.Nnrf_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__Nnrf_ip_is_set = 1;
            }
            else
            {
                printf("-Nnrfip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-smfn4ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-smfn4ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__amf_config.smf_n4_addr.ip_version = IP_VER_IS_V4;
                g__amf_config.smf_n4_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__smf_n4_ip_is_set = 1;
            }
            else
            {
                printf("-smfn4ip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-upfn4ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-upfn4ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__amf_config.upf_n4_addr.ip_version = IP_VER_IS_V4;
                g__amf_config.upf_n4_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__upf_n4_ip_is_set = 1;
            }
            else
            {
                printf("-upfn4ip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num    -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-upfn3ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-upfn3ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__amf_config.upf_n3_addr.ip_version = IP_VER_IS_V4;
                g__amf_config.upf_n3_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__upf_n3_ip_is_set = 1;
            }
            else
            {
                printf("-upfn3ip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num    -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-gnbreg"))
        {
            if(NULL == argv[arg_index + 1] || NULL == argv[arg_index + 2])
            {
                printf("-gnbreg <gnodeB_N1_interface_ip> <gnodeB_N3_interface_ip> \n");
                printf("gnodeB registration: Please provide two argumnets for -gnbreg option.. \n");
                return -1;
            }
            else
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr_1))
                {
                    printf("N1 interface IP address for -gnbreg option is not valid [%s] \n", argv[arg_index + 1]);
                    printf("-gnbreg <gnodeB_N1_interface_ip> <gnodeB_N3_interface_ip> \n");
                    printf("gnodeB registration: Please provide two argumnets for -gnbreg option.. \n");
                    return -1;
                }
                if(0 == inet_aton(argv[arg_index + 2], &v4_addr_2))
                {
                    printf("N3 interface IP address for -gnbreg option is not valid [%s] \n", argv[arg_index + 2]);
                    printf("-gnbreg <gnodeB_N1_interface_ip> <gnodeB_N3_interface_ip> \n");
                    printf("gnodeB registration: Please provide two argumnets for -gnbreg option.. \n");
                    return -1;
                }
                gnb_index = g__amf_config.gnb_count;
                gnb_data_ptr = &(g__amf_config.gnb_data[gnb_index]);
                gnb_data_ptr->gnb_id = GNB_ID_BASE + gnb_index;
                gnb_data_ptr->gnb_n1_n2_addr.ip_version = IP_VER_IS_V4;
                gnb_data_ptr->gnb_n1_n2_addr.u.v4_addr = htonl(v4_addr_1.s_addr);
                gnb_data_ptr->gnb_n3_addr.ip_version = IP_VER_IS_V4;
                gnb_data_ptr->gnb_n3_addr.u.v4_addr = htonl(v4_addr_2.s_addr);
                memset(&(gnb_data_ptr->gnb_n1_n2_sockaddr), 0, sizeof(struct sockaddr_in));
                gnb_data_ptr->gnb_n1_n2_sockaddr.sin_family      = AF_INET;
                gnb_data_ptr->gnb_n1_n2_sockaddr.sin_addr.s_addr = htonl(gnb_data_ptr->gnb_n1_n2_addr.u.v4_addr);
                gnb_data_ptr->gnb_n1_n2_sockaddr.sin_port        = htons(UDP_PORT_IS_NMP);
                g__amf_config.gnb_count += 1;
                printf("gnodeB with Interfaces [ N1/N2 = %s,  N3 = %s ] is registered in AMF \n", argv[arg_index + 1], argv[arg_index + 2]);
            }
            arg_num   -= 3;
            arg_index += 3;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-d"))
        {
            if(NULL != argv[arg_index + 1])
            {
                g__amf_config.pkt_delay = strtoul(argv[arg_index + 1], NULL, 10);
            }
            else
            {
                printf("Provide a value for -d <value> \n");
                return -1;
            }

            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-debug"))
        {
            g__amf_config.debug_switch = 1;
            arg_num   -= 1;
            arg_index += 1;
            continue;
        }
        else
        {
            printf("Invalid argument [ %s ] \n", argv[arg_index]);
            printf("%s", help_string);
            return -1;
        }
    } /* while(0 != argnum) */


    printf("\n");


    // Check if my N1/N2 interface IP address is set by user
    if(g__my_n1_n2_ip_is_set)
    {
        get_ipv4_addr_string(g__amf_config.my_n1_n2_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "AMF N1/N2 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide AMF N1/N2 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }


    // Check if my Namf interface IP address is set.
    if(g__my_Namf_ip_is_set)
    {
        get_ipv4_addr_string(g__amf_config.my_Namf_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "AMF Namf Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide AMF Namf interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }

    // Check if Nnrf interface IP address is set.
    if(g__Nnrf_ip_is_set)
    {
        get_ipv4_addr_string(g__amf_config.Nnrf_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "Nnrf Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide Nnrf interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }

    // Check if ip address of SMF N4 interface is set by user
    // Since SMF is integrated inside AMF, this is the reason to
    // use N4 interface connection between AMF and UPF directly.
    // This if for simplicity in demonstration of NMP packet flow.
    // In reality, N4 interface belongs to SMF 
    // and AMF has no N4 interface.
    if(g__smf_n4_ip_is_set)
    {
        get_ipv4_addr_string(g__amf_config.smf_n4_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "SMF N4 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide AMF N4 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }


    // Check if ip address of UPF N4 interface is set by user
    if(g__upf_n4_ip_is_set)
    {
        get_ipv4_addr_string(g__amf_config.upf_n4_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "UPF N4 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide UPF N4 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }

    // Check if ip address of UPF N3 interface is set by user
    if(g__upf_n3_ip_is_set)
    {
        get_ipv4_addr_string(g__amf_config.upf_n3_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "UPF N3 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide UPF N3 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }


    // Check if at least one gnodeB is registered by user into AMF
    if(0 == g__amf_config.gnb_count)
    {
        printf("You have to register a gnodeB also...\n");
        printf("You can use -gnbreg option for this purpose... It takes two IPv4 addresses \n");
        printf("-gnbreg <gnodeB_N1_N2_interface_ip> <gnodeB_N3_interface_ip> \n");
        printf("%s \n", help_string);
        return -1;
    }
    else
    {
        printf("Total %u gnodeB's are registered with AMF \n", g__amf_config.gnb_count);
    }

    printf("\n");

    g__amf_config.my_id  = AMF_ID_BASE;
    g__amf_config.upf_id = UPF_ID_BASE;
    g__amf_config.nrf_id = NRF_ID_BASE;

    // Create N1/N2 interface socket for incoming NMP messages from UE/gnodeB
    if(-1 == create_amf_n1_n2_nmp_listener_socket())
    {
        return -1;
    }

    // Create N4 interface socket for incoming NMP messages from UPF
    if(-1 == create_amf_n4_nmp_listener_socket())
    {
        return -1;
    }

    // Create Namf interface socket for sending service registration message to NRF.
    if(-1 == create_amf_Namf_nmp_listener_socket())
    {
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // Register single UPF with AMF
    //////////////////////////////////////////////////////////////////////////////////////
    g__amf_config.upf_id = UPF_ID_BASE;
    memset(&(g__amf_config.upf_n4_sockaddr), 0x0, sizeof(struct sockaddr_in));
    g__amf_config.upf_n4_sockaddr.sin_family      = AF_INET;	
    g__amf_config.upf_n4_sockaddr.sin_addr.s_addr = htonl(g__amf_config.upf_n4_addr.u.v4_addr);
    g__amf_config.upf_n4_sockaddr.sin_port        = htons(UDP_PORT_IS_NMP);

    //////////////////////////////////////////////////////////////////////////////////////
    // Register NRF with AMF
    //////////////////////////////////////////////////////////////////////////////////////
    g__amf_config.nrf_id = NRF_ID_BASE;
    memset(&(g__amf_config.Nnrf_sockaddr), 0x0, sizeof(struct sockaddr_in));
    g__amf_config.Nnrf_sockaddr.sin_family      = AF_INET;	
    g__amf_config.Nnrf_sockaddr.sin_addr.s_addr = htonl(g__amf_config.Nnrf_addr.u.v4_addr);
    g__amf_config.Nnrf_sockaddr.sin_port        = htons(UDP_PORT_IS_NMP);
     
    printf("Simulating AMF ..... \n");

    printf("Send service registration message to NRF \n");
    send_service_registration_msg_to_nrf(g__amf_config.debug_switch); 
    
    printf("Listen for N1/N2 messages from gnodeB \n");
    listen_for_n1_n2_messages();

    printf("Done ....... \n\n");

    return 0;
}

