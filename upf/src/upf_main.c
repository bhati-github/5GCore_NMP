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
#include "color_print.h"

#include "upf.h"
#include "n4_msg_handler.h"

upf_config_t   g__upf_config;

uint8_t  g__n4_send_msg_buffer[MSG_BUFFER_LEN];
uint8_t  g__n4_rcvd_msg_buffer[MSG_BUFFER_LEN];

uint8_t  g__my_n3_ip_is_set = 0;
uint8_t  g__my_n4_ip_is_set = 0;
uint8_t  g__my_n6_ip_is_set = 0;
uint8_t  g__smf_n4_ip_is_set = 0;

char *help_string = " ...... ";

int
create_upf_n4_nmp_listener_socket()
{
    struct sockaddr_in  n4_listener_addr;

    // Creating socket file descriptor
    g__upf_config.my_n4_socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    if(g__upf_config.my_n4_socket_id < 0)
    {
        perror("UPF: My N4 interface NMP Listener Socket Creation failed");
        return -1;
    }

    memset(&n4_listener_addr, 0, sizeof(struct sockaddr_in));
    n4_listener_addr.sin_family      = AF_INET;
    n4_listener_addr.sin_addr.s_addr = htonl(g__upf_config.my_n4_addr.u.v4_addr);
    n4_listener_addr.sin_port        = htons(UDP_PORT_IS_NMP);

    if(bind(g__upf_config.my_n4_socket_id,
           (struct sockaddr *)&n4_listener_addr,
            sizeof(struct sockaddr_in)) < 0)
    {
        printf("%s: Error in bind() operation \n", __func__);
        perror("Bind_Operation: ");
        close(g__upf_config.my_n4_socket_id);
        return -1;
    }

    printf("N4 Interface NMP socket created.. \n");
    return 0;
}


int 
main(int argc, char **argv)
{
    int arg_num = 0, arg_index = 0;
    char string[128];
    struct in_addr  v4_addr;

    memset(&g__upf_config, 0x0, sizeof(upf_config_t));

    arg_num   = argc - 1;
    arg_index = 1;

    while(0 != arg_num)
    {
        if(0 == strcmp(argv[arg_index], "-myn3ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-myn3ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__upf_config.my_n3_addr.ip_version = IP_VER_IS_V4;
                g__upf_config.my_n3_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__my_n3_ip_is_set = 1;
            }
            else
            {
                printf("-myn3ip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-myn4ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-myn4ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__upf_config.my_n4_addr.ip_version = IP_VER_IS_V4;
                g__upf_config.my_n4_addr.u.v4_addr  = htonl(v4_addr.s_addr);
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
        else if(0 == strcmp(argv[arg_index], "-smfn4ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-smfn4ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__upf_config.smf_n4_addr.ip_version = IP_VER_IS_V4;
                g__upf_config.smf_n4_addr.u.v4_addr  = htonl(v4_addr.s_addr);
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
        else if(0 == strcmp(argv[arg_index], "-myn6ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-myn6ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__upf_config.my_n6_addr.ip_version = IP_VER_IS_V4;
                g__upf_config.my_n6_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__my_n6_ip_is_set = 1;
            }
            else
            {
                printf("-myn6ip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-d"))
        {
            if(NULL != argv[arg_index + 1])
            {
                g__upf_config.pkt_delay = strtoul(argv[arg_index + 1], NULL, 10);
            }
            else
            {
                printf("Provide pkt delay value as -d <value> \n");
                return -1;
            }

            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-debug"))
        {
            g__upf_config.debug_switch = 1;
            arg_num   -= 1;
            arg_index += 1;
            continue;
        }
        else
        {
            printf("Invalid argument \n");
            printf("%s", help_string);
            return -1;
        }
    } /* while(0 != argnum) */


    printf("\n");

    /////////////////////////////////////////////////////
    // Check if my N3 interface IP address is set by user
    /////////////////////////////////////////////////////
    if(g__my_n3_ip_is_set)
    {
        get_ipv4_addr_string(g__upf_config.my_n3_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "UPF N3 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide UPF N3 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }

    /////////////////////////////////////////////////////
    // Check if my N4 interface IP address is set by user
    /////////////////////////////////////////////////////
    if(g__my_n4_ip_is_set)
    {
        get_ipv4_addr_string(g__upf_config.my_n4_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "UPF N4 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide UPF N4 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }

    ///////////////////////////////////////////////////////////
    // Check if ip address of SMF N4 interface is set by user
    ///////////////////////////////////////////////////////////
    if(g__smf_n4_ip_is_set)
    {
        get_ipv4_addr_string(g__upf_config.smf_n4_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "SMF N4 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide SMF N4 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }


    /////////////////////////////////////////////////////
    // Check if my N6 interface IP address is set by user
    /////////////////////////////////////////////////////
    if(g__my_n6_ip_is_set)
    {
        get_ipv4_addr_string(g__upf_config.my_n6_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "UPF N6 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide UPF N6 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }


    printf("\n");

    // Create N4 interface socket for incoming NMP messages...
    if(-1 == create_upf_n4_nmp_listener_socket())
    {
        return -1;
    }

    memset(&(g__upf_config.smf_n4_sockaddr), 0, sizeof(struct sockaddr_in));
    g__upf_config.smf_n4_sockaddr.sin_family      = AF_INET;
    g__upf_config.smf_n4_sockaddr.sin_addr.s_addr = htonl(g__upf_config.smf_n4_addr.u.v4_addr);
    g__upf_config.smf_n4_sockaddr.sin_port        = htons(UDP_PORT_IS_NMP);

    g__upf_config.smf_id = SMF_ID_BASE;  
    g__upf_config.my_id  = UPF_ID_BASE;

    g__upf_config.current_uplink_teid = UPF_UPLINK_TEID_BASE;

    GREEN_PRINT("Start listening for data-path setup messages on N4 interface. \n");
    printf("\n");

    listen_for_n4_messages();	

    printf("Done ....... \n");

    return 0;
}

