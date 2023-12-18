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

#include "smf.h"
#include "n4_msg_handler.h"
#include "Nsmf_msg_handler.h"

// json dependency
#include "cJSON.h"

smf_config_t   g__smf_config;

uint8_t  g__n4_send_msg_buffer[MSG_BUFFER_LEN];
uint8_t  g__n4_rcvd_msg_buffer[MSG_BUFFER_LEN];

uint8_t  g__Nsmf_send_msg_buffer[MSG_BUFFER_LEN];
uint8_t  g__Nsmf_rcvd_msg_buffer[MSG_BUFFER_LEN];

uint32_t g__ue_ipv4_addr_subnet = 0xc0a80000;
uint32_t g__ue_ipv4_addr_mask   = 0xffff0000;
uint32_t g__ue_ipv4_addr_base   = 0xc0a80001;

uint8_t  g__my_Nsmf_ip_is_set = 0;
uint8_t  g__my_n4_ip_is_set = 0;
uint8_t  g__Nnrf_ip_is_set = 0;
uint8_t  g__Namf_ip_is_set = 0;
uint8_t  g__upf_n3_ip_is_set = 0;
uint8_t  g__upf_n4_ip_is_set = 0;

char *help_string = " AMF...... \n";

int
create_smf_n4_nmp_listener_socket()
{
    struct sockaddr_in  n4_listener_addr;

    // Creating socket file descriptor
    g__smf_config.my_n4_socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    if(g__smf_config.my_n4_socket_id < 0)
    {
        perror("SMF: N4 interface Listener Socket Creation failed");
        return -1;
    }

    memset(&n4_listener_addr, 0, sizeof(struct sockaddr_in));
    n4_listener_addr.sin_family      = AF_INET;
    n4_listener_addr.sin_addr.s_addr = htonl(g__smf_config.my_n4_addr.u.v4_addr);
    n4_listener_addr.sin_port        = htons(UDP_PORT_IS_NMP);

    if(bind(g__smf_config.my_n4_socket_id,
                (struct sockaddr *)&n4_listener_addr,
                sizeof(struct sockaddr_in)) < 0)
    {
        printf("%s: Error in bind() operation \n", __func__);
        perror("Bind_Operation: ");
        close(g__smf_config.my_n4_socket_id);
        return -1;
    }

    printf("N4 interface NMP socket created [Ok] \n");
    return 0;
}


///////////////////////////////////////////////////////////////////
// SMF sends service registration message to NRF at init time.
///////////////////////////////////////////////////////////////////
int
create_smf_Nsmf_nmp_listener_socket()
{
    struct sockaddr_in  Nsmf_listener_addr;

    // Creating socket file descriptor
    g__smf_config.my_Nsmf_socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    if(g__smf_config.my_Nsmf_socket_id < 0)
    {
        perror("SMF: Nsmf interface Listener Socket Creation failed");
        return -1;
    }

    memset(&Nsmf_listener_addr, 0, sizeof(struct sockaddr_in));
    Nsmf_listener_addr.sin_family      = AF_INET;
    Nsmf_listener_addr.sin_addr.s_addr = htonl(g__smf_config.my_Nsmf_addr.u.v4_addr);
    Nsmf_listener_addr.sin_port        = htons(UDP_PORT_IS_NMP);

    if(bind(g__smf_config.my_Nsmf_socket_id,
            (struct sockaddr *)&Nsmf_listener_addr,
            sizeof(struct sockaddr_in)) < 0)
    {
        printf("%s: Error in bind() operation \n", __func__);
        perror("Bind_Operation: ");
        close(g__smf_config.my_Nsmf_socket_id);
        return -1;
    }

    printf("Nsmf interface NMP socket created [Ok] \n");
    return 0;
}


int
validate_rcvd_nmp_msg_on_n4_interface(uint8_t *msg_ptr,
                                      int      buf_len)
{
    if(buf_len < sizeof(nmp_hdr_t))
    {
        printf("%s: Invalid size \n", __func__);
        return -1;
    }

    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)msg_ptr;

    if(NODE_TYPE__SMF != htons(nmp_hdr_ptr->dst_node_type))
    {
        printf("%s: Destination Node of NMP message is not SMF \n", __func__);
        return -1;
    }

    if(g__smf_config.my_id != htons(nmp_hdr_ptr->dst_node_id))
    {
        printf("%s: Destination node id is not equal to my id \n", __func__);
        return -1;
    }

    return 0;
}



int 
main(int argc, char **argv)
{
    int arg_num = 0, arg_index = 0;
    char string[128];
    struct in_addr  v4_addr;

    memset(&g__smf_config, 0x0, sizeof(smf_config_t));

    arg_num   = argc - 1;
    arg_index = 1;

    while(0 != arg_num)
    {
        if(0 == strcmp(argv[arg_index], "-myn4ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-myn4ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__smf_config.my_n4_addr.ip_version = IP_VER_IS_V4;
                g__smf_config.my_n4_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__my_n4_ip_is_set = 1;
            }
            else
            {
                printf("-myn4ip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-myNsmfip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-myNsmfip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__smf_config.my_Nsmf_addr.ip_version = IP_VER_IS_V4;
                g__smf_config.my_Nsmf_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__my_Nsmf_ip_is_set = 1;
            }
            else
            {
                printf("-myNsmfip %s is not valid \n", argv[arg_index + 1]);
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
                g__smf_config.Nnrf_addr.ip_version = IP_VER_IS_V4;
                g__smf_config.Nnrf_addr.u.v4_addr  = htonl(v4_addr.s_addr);
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
        else if(0 == strcmp(argv[arg_index], "-Namfip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-Namfip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__smf_config.Namf_addr.ip_version = IP_VER_IS_V4;
                g__smf_config.Namf_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__Namf_ip_is_set = 1;
            }
            else
            {
                printf("-Namfip %s is not valid \n", argv[arg_index + 1]);
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
                g__smf_config.upf_n4_addr.ip_version = IP_VER_IS_V4;
                g__smf_config.upf_n4_addr.u.v4_addr  = htonl(v4_addr.s_addr);
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
                g__smf_config.upf_n3_addr.ip_version = IP_VER_IS_V4;
                g__smf_config.upf_n3_addr.u.v4_addr  = htonl(v4_addr.s_addr);
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
        else if(0 == strcmp(argv[arg_index], "-d"))
        {
            if(NULL != argv[arg_index + 1])
            {
                g__smf_config.pkt_delay = strtoul(argv[arg_index + 1], NULL, 10);
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
            g__smf_config.debug_switch = 1;
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


    // Check if my N4 interface IP address is set by user
    if(g__my_n4_ip_is_set)
    {
        get_ipv4_addr_string(g__smf_config.my_n4_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "SMF N4 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide SMF N4 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }


    // Check if my Nsmf interface IP address is set.
    if(g__my_Nsmf_ip_is_set)
    {
        get_ipv4_addr_string(g__smf_config.my_Nsmf_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "SMF Nsmf Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide SMF Nsmf interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }

    // Check if Nnrf interface IP address is set.
    if(g__Nnrf_ip_is_set)
    {
        get_ipv4_addr_string(g__smf_config.Nnrf_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "Nnrf Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide Nnrf interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }

    // Check if Namf interface IP address is set.
    if(g__Namf_ip_is_set)
    {
        get_ipv4_addr_string(g__smf_config.Namf_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "Namf Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide Namf interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }


    // Check if ip address of UPF N4 interface is set by user
    if(g__upf_n4_ip_is_set)
    {
        get_ipv4_addr_string(g__smf_config.upf_n4_addr.u.v4_addr, string);
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
        get_ipv4_addr_string(g__smf_config.upf_n3_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "UPF N3 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide UPF N3 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }

    printf("\n");

    g__smf_config.my_id  = SMF_ID_BASE;
    g__smf_config.upf_id = UPF_ID_BASE;
    g__smf_config.nrf_id = NRF_ID_BASE;
    g__smf_config.amf_id = AMF_ID_BASE;

    // Create N4 interface socket for incoming NMP messages from UPF
    if(-1 == create_smf_n4_nmp_listener_socket())
    {
        return -1;
    }

    // Create Nsmf interface socket for communication with 
    // peer interfaces like Namf, Nnrf, Npcf etc.
    if(-1 == create_smf_Nsmf_nmp_listener_socket())
    {
        return -1;
    }

    //////////////////////////////////////////////////////////////////////////////////////
    // Register single UPF with SMF
    //////////////////////////////////////////////////////////////////////////////////////
    g__smf_config.upf_id = UPF_ID_BASE;
    memset(&(g__smf_config.upf_n4_sockaddr), 0x0, sizeof(struct sockaddr_in));
    g__smf_config.upf_n4_sockaddr.sin_family      = AF_INET;	
    g__smf_config.upf_n4_sockaddr.sin_addr.s_addr = htonl(g__smf_config.upf_n4_addr.u.v4_addr);
    g__smf_config.upf_n4_sockaddr.sin_port        = htons(UDP_PORT_IS_NMP);

    printf("UPF N4 Addr = %u \n", g__smf_config.upf_n4_sockaddr.sin_addr.s_addr);
    printf("UPF N4 Port = %u \n", g__smf_config.upf_n4_sockaddr.sin_port);

    //////////////////////////////////////////////////////////////////////////////////////
    // Register NRF with SMF
    //////////////////////////////////////////////////////////////////////////////////////
    g__smf_config.nrf_id = NRF_ID_BASE;
    memset(&(g__smf_config.Nnrf_sockaddr), 0x0, sizeof(struct sockaddr_in));
    g__smf_config.Nnrf_sockaddr.sin_family      = AF_INET;	
    g__smf_config.Nnrf_sockaddr.sin_addr.s_addr = htonl(g__smf_config.Nnrf_addr.u.v4_addr);
    g__smf_config.Nnrf_sockaddr.sin_port        = htons(UDP_PORT_IS_NMP);
    
    //////////////////////////////////////////////////////////////////////////////////////
    // Register AMF with SMF
    //////////////////////////////////////////////////////////////////////////////////////
    memset(&(g__smf_config.Namf_sockaddr), 0x0, sizeof(struct sockaddr_in));
    g__smf_config.Namf_sockaddr.sin_family      = AF_INET;
    g__smf_config.Namf_sockaddr.sin_addr.s_addr = htonl(g__smf_config.Namf_addr.u.v4_addr);
    g__smf_config.Namf_sockaddr.sin_port        = htons(UDP_PORT_IS_NMP);

 
    printf("Simulating AMF ..... \n");

    printf("Send service registration message to NRF \n");
    send_service_registration_msg_to_nrf(g__smf_config.debug_switch); 
    
    printf("Listen for messages on Nsmf interface \n");
    listen_for_Nsmf_messages();

    printf("Done ....... \n\n");

    return 0;
}

