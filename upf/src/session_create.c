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
#include "nmp_item.h"
#include "nmp_msg_parser.h"
#include "common_util.h"

#include "upf.h"
#include "session_create.h"

int
process_session_create_request_msg(nmp_msg_data_t *nmp_n2_rcvd_msg_data_ptr,
                                   uint8_t         debug_flag)
{
    int i = 0;
    int n = 0;
    int ret = 0;
    int offset = 0;
    char string[128];
    uint16_t item_count = 0;
    uint16_t dst_node_id = 0;
    nmp_msg_data_t nmp_n2_send_msg_data;

    printf("Session Created for IMSI:IP = ");
    for(i = 0; i < 8; i++)
    {
        printf("%02x", nmp_n2_rcvd_msg_data_ptr->imsi.u8[i]);
    }

    get_ipv4_addr_string(nmp_n2_rcvd_msg_data_ptr->ue_ip_addr.u.v4_addr, string);
    printf(" : %s \n", string);


    // Send response back to AMF
    uint8_t *ptr = g__n2_send_msg_buffer; 

    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;
    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__UPF);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->src_node_id    = htons(g__upf_config.my_id);

    dst_node_id = (nmp_n2_rcvd_msg_data_ptr->msg_identifier >> 16) & 0xffff;
    nmp_hdr_ptr->dst_node_id    = htons(dst_node_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__SESSION_CREATE_RESPONSE);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    nmp_hdr_ptr->msg_identifier = htonl(nmp_n2_rcvd_msg_data_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    // Item: Response
    ret = nmp_add_item__msg_response(ptr + offset, MSG_RESPONSE_IS_OK);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    nmp_hdr_ptr->msg_item_len   = htons(offset - sizeof(nmp_hdr_t));
    nmp_hdr_ptr->msg_item_count = htons(item_count);

    if(-1 == parse_nmp_msg(ptr,
                           offset,
                           &(nmp_n2_send_msg_data),
                           debug_flag))
    {
        printf("%s: Msg parse error. \n", __func__);
        return -1;
    }

    ////////////////////////////////////////////////
    // write this msg on n2 socket (towards AMF)
    ////////////////////////////////////////////////
    n = sendto(g__upf_config.upf_n2_socket_id,
               (char *)ptr,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(g__upf_config.amf_n2_sockaddr),
               sizeof(struct sockaddr_in));
    if(n != offset)
    {
        printf("%s: sendto() failed for Session Create Response Message. \n", __func__);
        return -1;
    }

    printf("%s: Session Create Response Msg Sent.. \n", __func__);
    return 0;	
}


