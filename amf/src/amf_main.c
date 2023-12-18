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
#include "Namf_msg_handler.h"

amf_config_t   g__amf_config;

uint8_t  g__n1_n2_send_msg_buffer[MSG_BUFFER_LEN];
uint8_t  g__n1_n2_rcvd_msg_buffer[MSG_BUFFER_LEN];

uint8_t  g__n4_send_msg_buffer[MSG_BUFFER_LEN];
uint8_t  g__n4_rcvd_msg_buffer[MSG_BUFFER_LEN];

uint8_t  g__Namf_send_msg_buffer[MSG_BUFFER_LEN];
uint8_t  g__Namf_rcvd_msg_buffer[MSG_BUFFER_LEN];

uint8_t  g__my_Namf_ip_is_set = 0;
uint8_t  g__my_n1_n2_ip_is_set = 0;
uint8_t  g__Nnrf_ip_is_set = 0;
uint8_t  g__Nsmf_ip_is_set = 0;

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
        else if(0 == strcmp(argv[arg_index], "-Nsmfip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-Nsmfip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__amf_config.Nsmf_addr.ip_version = IP_VER_IS_V4;
                g__amf_config.Nsmf_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__Nsmf_ip_is_set = 1;
            }
            else
            {
                printf("-Nsmfip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
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

    // Check if Nsmf interface IP address is set.
    if(g__Nsmf_ip_is_set)
    {
        get_ipv4_addr_string(g__amf_config.Nsmf_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "Nsmf Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide Nsmf interface ipv4 address \n");
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
    g__amf_config.smf_id = SMF_ID_BASE;
    g__amf_config.nrf_id = NRF_ID_BASE;

    // Create N1/N2 interface socket for incoming NMP messages from UE/gnodeB
    if(-1 == create_amf_n1_n2_nmp_listener_socket())
    {
        return -1;
    }

    // Create Namf interface socket for communication with interfaces like Nsmf, Nnrf, etc.
    if(-1 == create_amf_Namf_nmp_listener_socket())
    {
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // Register NRF with AMF
    //////////////////////////////////////////////////////////////////////////////////////
    g__amf_config.nrf_id = NRF_ID_BASE;
    memset(&(g__amf_config.Nnrf_sockaddr), 0x0, sizeof(struct sockaddr_in));
    g__amf_config.Nnrf_sockaddr.sin_family      = AF_INET;	
    g__amf_config.Nnrf_sockaddr.sin_addr.s_addr = htonl(g__amf_config.Nnrf_addr.u.v4_addr);
    g__amf_config.Nnrf_sockaddr.sin_port        = htons(UDP_PORT_IS_NMP);
     

    memset(&(g__amf_config.Nsmf_sockaddr), 0x0, sizeof(struct sockaddr_in));
    g__amf_config.Nsmf_sockaddr.sin_family      = AF_INET;
    g__amf_config.Nsmf_sockaddr.sin_addr.s_addr = htonl(g__amf_config.Nsmf_addr.u.v4_addr);
    g__amf_config.Nsmf_sockaddr.sin_port        = htons(UDP_PORT_IS_NMP);


    printf("Simulating AMF ..... \n");

    printf("Send service registration message to NRF \n");
    send_service_registration_msg_to_nrf(g__amf_config.debug_switch); 
    
    printf("Listen for N1/N2 messages from gnodeB \n");
    listen_for_n1_n2_messages();

    printf("Done ....... \n\n");

    return 0;
}

