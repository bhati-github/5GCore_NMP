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
#include "bearer_setup.h"

int
validate_rcvd_msg_on_n1_interface(uint8_t *msg_ptr,
                                  int      msg_len,
                                  uint32_t request_identifier)
{
    if(msg_len < sizeof(nmp_hdr_t))
    {
        printf("Invalid size of NMP message.. \n");
        return -1;
    }

    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)msg_ptr;

    if(NODE_TYPE__GNB != htons(nmp_hdr_ptr->dst_node_type))
    {
        printf("Destination Node of NMP message is not GNB \n");
        return -1;
    }

    if(g__gnb_config.my_id != htons(nmp_hdr_ptr->dst_node_id))
    {
        printf("Destination node id is not equal to my id \n");
        return -1;
    }

    return 0;
}


int
process_rcvd_n1_msg(nmp_msg_data_t *nmp_n1_rcvd_msg_data_ptr,
                    uint8_t         debug_flag)
{
    if(MSG_TYPE__BEARER_SETUP_REQUEST == nmp_n1_rcvd_msg_data_ptr->msg_type)
    {
        return process_bearer_setup_request_msg(nmp_n1_rcvd_msg_data_ptr, debug_flag);	
    }
    else
    {
        printf("%s: Unknown N1 message \n", __func__);
        return -1;
    }

    return 0;
}

int
listen_for_n1_messages()
{
    int n = 0;
    int len = 0;
    char string[128];
    uint32_t amf_addr = 0;
    uint16_t amf_port = 0;
    struct sockaddr_in  amf_sockaddr;
    nmp_msg_data_t nmp_n1_rcvd_msg_data;

    while(1)
    {
        ///////////////////////////////////////////////
        // Wait for request messages from AMF..
        ///////////////////////////////////////////////
        len = sizeof(struct sockaddr_in);
        memset(&amf_sockaddr, 0x0, sizeof(struct sockaddr_in));
        n = recvfrom(g__gnb_config.gnb_n1_socket_id,
                     (char *)g__n1_rcvd_msg_buffer,
                     MSG_BUFFER_LEN,
                     MSG_WAITALL,
                     (struct sockaddr *)&(amf_sockaddr),
                     (socklen_t *)&len);

        amf_addr = htonl(amf_sockaddr.sin_addr.s_addr);
        amf_port = htons(amf_sockaddr.sin_port);

        if(g__gnb_config.debug_switch)
        {
            get_ipv4_addr_string(amf_addr, string);
            printf("-----------> Rcvd request (%u bytes) from AMF (%s:%u) \n",
                    n, string, amf_port);
        }

        if(-1 == validate_rcvd_msg_on_n1_interface(g__n1_rcvd_msg_buffer, n, 0))
        {
            continue;
        }

        if(-1 == parse_nmp_msg(g__n1_rcvd_msg_buffer,
                               n,
                               &(nmp_n1_rcvd_msg_data),
                               g__gnb_config.debug_switch))
        {
            printf("%s: Rcvd N1 message parse error.. \n", __func__);
            continue;
        }


        if(-1 == process_rcvd_n1_msg(&(nmp_n1_rcvd_msg_data),
                                     g__gnb_config.debug_switch))
        {
            printf("Unable to process rcvd N1 message \n");
        }
        else
        {
            printf("Successfully processed rcvd N1 message \n");
        }
    }

    return 0;
}

