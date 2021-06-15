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

#include "gnb.h"
#include "n1_msg_handler.h"
#include "ue_attach.h"

gnb_config_t   g__gnb_config;

uint8_t  g__n1_send_msg_buffer[MSG_BUFFER_LEN];
uint8_t  g__n1_rcvd_msg_buffer[MSG_BUFFER_LEN];

uint8_t  g__gnb_n1_ip_is_set = 0;
uint8_t  g__gnb_n3_ip_is_set = 0;
uint8_t  g__amf_n1_ip_is_set = 0;
uint32_t g__user_count = 0;

char *help_string = " ...... ";

static int
create_gnb_n1_listener_socket()
{
    struct sockaddr_in  n1_listener_addr;

    // Creating socket file descriptor
    g__gnb_config.gnb_n1_socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    if(g__gnb_config.gnb_n1_socket_id < 0)
    {
        perror("gnodeB: N1 interface Listener Socket Creation failed");
        return -1;
    }

    memset(&n1_listener_addr, 0, sizeof(struct sockaddr_in));
    n1_listener_addr.sin_family      = AF_INET;
    n1_listener_addr.sin_addr.s_addr = htonl(g__gnb_config.gnb_n1_addr.u.v4_addr);
    n1_listener_addr.sin_port        = htons(UDP_PORT_IS_NMP);

    if(bind(g__gnb_config.gnb_n1_socket_id,
           (struct sockaddr *)&n1_listener_addr,
           sizeof(struct sockaddr_in)) < 0)
    {
        printf("%s: Error in bind() operation \n", __func__);
        perror("Bind_Operation: ");
        close(g__gnb_config.gnb_n1_socket_id);
        return -1;
    }

    perror("Bind_Operation: ");
    return 0;
}


int 
main(int argc, char **argv)
{
    int arg_num = 0, arg_index = 0;
    uint16_t user_id = 0;
    char string[128];
    struct in_addr  v4_addr;

    memset(&g__gnb_config, 0x0, sizeof(gnb_config_t));

    arg_num   = argc - 1;
    arg_index = 1;

    while(0 != arg_num)
    {
        if(0 == strcmp(argv[arg_index], "-gnbn1ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-gnbn1ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__gnb_config.gnb_n1_addr.ip_version = IP_VER_IS_V4;
                g__gnb_config.gnb_n1_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__gnb_n1_ip_is_set = 1;
            }
            else
            {
                printf("-gnbn1ip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-gnbn3ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-gnbn3ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__gnb_config.gnb_n3_addr.ip_version = IP_VER_IS_V4;
                g__gnb_config.gnb_n3_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__gnb_n3_ip_is_set = 1;
            }
            else
            {
                printf("-gnbn3ip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-amfn1ip"))
        {
            if(NULL != argv[arg_index + 1])
            {
                if(0 == inet_aton(argv[arg_index + 1], &v4_addr))
                {
                    printf("-amfn1ip %s is not valid \n", argv[arg_index + 1]);
                    return -1;
                }
                g__gnb_config.amf_n1_addr.ip_version = IP_VER_IS_V4;
                g__gnb_config.amf_n1_addr.u.v4_addr  = htonl(v4_addr.s_addr);
                g__amf_n1_ip_is_set = 1;
            }
            else
            {
                printf("-amfip %s is not valid \n", argv[arg_index + 1]);
                return -1;
            }
            arg_num   -= 2;
            arg_index += 2;
            continue;
        }
        else if(0 == strcmp(argv[arg_index], "-c"))
        {
            if(NULL != argv[arg_index + 1])
            {
                g__user_count = strtoul(argv[arg_index + 1], NULL, 10);
                if(g__user_count > MAX_USERS)
                {
                    printf("This enodeb simulator supports upto %u sessions \n", MAX_USERS);
                    return -1;
                }
            }
            else
            {
                printf("Provide a value for -c <value> \n");
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
                g__gnb_config.pkt_delay = strtoul(argv[arg_index + 1], NULL, 10);
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
            g__gnb_config.debug_switch = 1;
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

    // Check if ip address of gnodeb N1 interface is set by user
    if(g__gnb_n1_ip_is_set)
    {
        get_ipv4_addr_string(g__gnb_config.gnb_n1_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "gnodeB N1 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide gnodeB N1 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }


    // Check if ip address of gnodeb N3 interface is set by user
    if(g__gnb_n3_ip_is_set)
    {
        get_ipv4_addr_string(g__gnb_config.gnb_n3_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "gnodeB N3 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide gnodeB N3 interface ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }


    // Check if ip address of Master Node is set by user
    if(g__amf_n1_ip_is_set)
    {
        get_ipv4_addr_string(g__gnb_config.amf_n1_addr.u.v4_addr, string);
        printf("%-32s : %s \n", "AMF N1 Interface IPv4 Addr", string);
    }
    else
    {
        printf("Provide AMF N1 ipv4 address \n");
        printf("%s \n", help_string);
        return -1;
    }

    printf("\n");

    // Create N1 interface socket for incoming NMP messages...
    if(-1 == create_gnb_n1_listener_socket())
    {
        return -1;
    }

    memset(&(g__gnb_config.amf_n1_sockaddr), 0, sizeof(struct sockaddr_in));
    g__gnb_config.amf_n1_sockaddr.sin_family      = AF_INET;
    g__gnb_config.amf_n1_sockaddr.sin_addr.s_addr = htonl(g__gnb_config.amf_n1_addr.u.v4_addr);
    g__gnb_config.amf_n1_sockaddr.sin_port        = htons(UDP_PORT_IS_NMP);

    g__gnb_config.amf_id = AMF_ID_BASE;

    // First enode id starts from ENB_ID_BASE
    // If a multi-thread gnodeB simulator exists, each 
    // gnodeB thread will have an incremental id
    g__gnb_config.my_id  = ENB_ID_BASE;

    printf("We have to send User Attach Request for %u users \n\n", g__user_count);

    for(user_id = 0; user_id < g__user_count; user_id++)
    {
        if(-1 == send_ue_attach_request_msg_to_amf(user_id, g__gnb_config.debug_switch))
        {
            printf("Failed to send UE attach message for user id %u \n\n", user_id);
        }
        else
        {
            printf("UE attach successful for user id %u \n\n", user_id);
        }
    }

    printf("Done ....... \n");

    return 0;
}

