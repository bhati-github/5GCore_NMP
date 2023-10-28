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

#include "amf.h"
#include "n2_msg_handler.h"
#include "upf_session.h"

int
send_session_modify_msg_to_upf(uint32_t     ue_ipv4_addr,
                               data_64bit_t imsi,
                               uint32_t     new_gnb_v4_addr,
                               uint32_t     upf_v4_addr,
                               uint8_t      debug_flag)
{
    int n = 0;
    int ret = 0;
    int offset = 0;
    int len = 0;
    char string[128];
    uint32_t session_index = 0;
    uint32_t msg_id = 0;
    uint16_t item_count = 0;
    uint32_t upf_addr = 0;
    uint16_t upf_port = 0;	
    struct sockaddr_in upf_sockaddr;
    uint32_t request_identifier = 0;

    nmp_msg_data_t nmp_n2_send_msg_data;
    nmp_msg_data_t nmp_n2_rcvd_msg_data;

    /////////////////////////////////////////
    // Get pdr and far for this user session
    /////////////////////////////////////////
    if(-1 == get_pdr_and_far_for_session_modify(ue_ipv4_addr,
                                                new_gnb_v4_addr,
                                                upf_v4_addr,
                                                &session_index))
    {
        printf("%s: Unable to get pdr and far for session creation in upf \n", __func__);
        return -1;
    }

    uint8_t *ptr = g__n2_send_msg_buffer;
    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;
    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__UPF);
    nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__amf_config.upf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__SESSION_CREATE_REQUEST);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__amf_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    // Save request identifier
    request_identifier = htonl(nmp_hdr_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    // Add UE IPv4
    ret = nmp_add_item__ue_ipv4_addr(ptr + offset, ue_ipv4_addr);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add IMSI
    ret = nmp_add_item__imsi(ptr + offset, imsi);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;


    // Create N3 PDR
    ret = nmp_add_item_group__n3_pdr(ptr + offset, session_index); 
    if(-1 == ret)
    {
        printf("%s: Unable to create N3 PDR Item group. \n", __func__);
        return -1;
    }
    offset += ret;
    item_count += 1;


    // Create N6 PDR
    ret = nmp_add_item_group__n6_pdr(ptr + offset, session_index);
    if(-1 == ret)
    {
        printf("%s: Unable to create N6 PDR Item group. \n", __func__);
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Create N3 FAR
    ret = nmp_add_item_group__n3_far(ptr + offset, session_index);
    if(-1 == ret)
    {
        printf("%s: Unable to create N3 FAR Item group. \n", __func__);
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Create N6 FAR
    ret = nmp_add_item_group__n6_far(ptr + offset, session_index);
    if(-1 == ret)
    {
        printf("%s: Unable to create N6 FAR Item group. \n", __func__);
        return -1;
    }
    offset += ret;
    item_count += 1;

    nmp_hdr_ptr->msg_item_len   = htons(offset - sizeof(nmp_hdr_t));
    nmp_hdr_ptr->msg_item_count = htons(item_count);

    if(-1 == parse_nmp_msg(g__n2_send_msg_buffer,
                           offset,
                           &(nmp_n2_send_msg_data),
                           debug_flag))
    {
        printf("%s: NMP message parsing error.. \n", __func__);
        return -1;
    }

    // Send this message to UPF
    n = sendto(g__amf_config.amf_n2_socket_id,
               (char *)g__n2_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(g__amf_config.upf_n2_sockaddr),
               sizeof(struct sockaddr_in));

    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to UPF \n", __func__);
        return -1;
    }

    ///////////////////////////////////////////////
    // Wait for reponse from upf
    ///////////////////////////////////////////////
    len = sizeof(struct sockaddr_in);
    memset(&upf_sockaddr, 0x0, sizeof(struct sockaddr_in));
    n = recvfrom(g__amf_config.amf_n2_socket_id,
                 (char *)g__n2_rcvd_msg_buffer,
                 MSG_BUFFER_LEN,
                 MSG_WAITALL,
                 (struct sockaddr *)&(upf_sockaddr),
                 (socklen_t *)&len);

    if(g__amf_config.debug_switch)
    {
        upf_addr = htonl(upf_sockaddr.sin_addr.s_addr);
        upf_port = htons(upf_sockaddr.sin_port);

        get_ipv4_addr_string(upf_addr, string);
        printf("<----------- Rcvd response (%u bytes) from UPF (%s:%u) \n", 
                n, string, upf_port);
    }

    if(-1 == validate_rcvd_msg_on_n2_interface(g__n2_rcvd_msg_buffer, 
                                               n, 
                                               request_identifier))
    {
        printf("%s: NMP response msg validation failed. \n", __func__);
        return -1;
    }

    if(-1 == parse_nmp_msg(g__n2_rcvd_msg_buffer,
                           n,
                           &(nmp_n2_rcvd_msg_data),
                           debug_flag))
    {
        printf("%s: NMP message parsing error.. \n", __func__);
        return -1;
    }

    if(MSG_RESPONSE_IS_OK == nmp_n2_rcvd_msg_data.msg_response)
    {
        printf("%s: Msg response from UPF is [Ok] \n", __func__);
        return 0;
    }
    else
    {
        printf("%s: Msg response from UPF is [Not Ok] \n", __func__);
        return -1;
    }
}


