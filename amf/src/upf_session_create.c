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
#include "n4_msg_handler.h"
#include "upf_session.h"


int
send_session_create_msg_to_upf(uint32_t      ue_ipv4_addr,
                               data_64bit_t  imsi,
                               uint32_t      gnb_v4_addr,
                               uint32_t      upf_v4_addr,
                               uint32_t     *ul_teid,
                               uint32_t     *dl_teid,
                               uint8_t       debug_flag)
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
    uint32_t request_identifier = 0;	
    struct sockaddr_in upf_sockaddr;
    struct sockaddr_in  target_service_sockaddr;
    pdr_t *pdr_ptr = NULL;
    far_t *far_ptr = NULL;

    nmp_msg_data_t nmp_n4_send_msg_data;
    nmp_msg_data_t nmp_n4_rcvd_msg_data;

    /////////////////////////////////////////
    // Get pdr and far for this user session
    /////////////////////////////////////////
    if(-1 == get_pdr_and_far_for_session_create(ue_ipv4_addr,
                                                gnb_v4_addr,
                                                upf_v4_addr,
                                                &session_index,
                                                ul_teid,
                                                dl_teid))
    {
        printf("%s: Unable to get pdr and far for session creation in upf \n", __func__);
        return -1;
    }

    uint8_t *ptr = g__n4_send_msg_buffer;
    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;
    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__UPF);
    nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__amf_config.upf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__UPF_SESSION_CREATE_REQ);
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
    pdr_ptr = &(g__amf_config.upf_session_data[session_index].n3_pdr);
    ret = nmp_add_item_group__n3_pdr(ptr + offset, 
                                     pdr_ptr->rule_id, 
                                     pdr_ptr->precedence,
                                     pdr_ptr->pdi.f_teid.ip_addr.u.v4_addr,
                                     pdr_ptr->pdi.f_teid.teid,
                                     pdr_ptr->action,
                                     pdr_ptr->far_id); 
    if(-1 == ret)
    {
        printf("%s: Unable to create N3 PDR Item group. \n", __func__);
        return -1;
    }
    offset += ret;
    item_count += 1;


    // Create N6 PDR
    pdr_ptr = &(g__amf_config.upf_session_data[session_index].n6_pdr);
    ret = nmp_add_item_group__n6_pdr(ptr + offset, 
                                     pdr_ptr->rule_id,
                                     pdr_ptr->precedence,
                                     pdr_ptr->pdi.ue_ip_addr.u.v4_addr,
                                     pdr_ptr->action,
                                     pdr_ptr->far_id);
    if(-1 == ret)
    {
        printf("%s: Unable to create N6 PDR Item group. \n", __func__);
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Create N3 FAR
    far_ptr = &(g__amf_config.upf_session_data[session_index].n3_far);
    ret = nmp_add_item_group__n3_far(ptr + offset,
                                     far_ptr->far_id,
                                     far_ptr->action_flags,
                                     far_ptr->forward_param.dst_interface,
                                     far_ptr->forward_param.f_teid.ip_addr.u.v4_addr,
                                     far_ptr->forward_param.f_teid.teid);
    if(-1 == ret)
    {
        printf("%s: Unable to create N3 FAR Item group. \n", __func__);
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Create N6 FAR
    far_ptr = &(g__amf_config.upf_session_data[session_index].n6_far);
    ret = nmp_add_item_group__n6_far(ptr + offset,
                                     far_ptr->far_id,
                                     far_ptr->action_flags,
                                     far_ptr->forward_param.dst_interface);
    if(-1 == ret)
    {
        printf("%s: Unable to create N6 FAR Item group. \n", __func__);
        return -1;
    }
    offset += ret;
    item_count += 1;

    nmp_hdr_ptr->msg_item_len   = htons(offset - sizeof(nmp_hdr_t));
    nmp_hdr_ptr->msg_item_count = htons(item_count);

    if(-1 == parse_nmp_msg(g__n4_send_msg_buffer,
                           offset,
                           &(nmp_n4_send_msg_data),
                           debug_flag))
    {
        printf("%s: msg parsing error. \n", __func__);
        return -1;
    }

    // Send this message to UPF
    target_service_sockaddr.sin_addr.s_addr = g__amf_config.upf_n4_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__amf_config.upf_n4_sockaddr.sin_port; 
    n = sendto(g__amf_config.smf_n4_socket_id,
               (char *)g__n4_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
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
    n = recvfrom(g__amf_config.smf_n4_socket_id,
                 (char *)g__n4_rcvd_msg_buffer,
                 MSG_BUFFER_LEN,
                 MSG_WAITALL,
                 (struct sockaddr *)&(upf_sockaddr),
                 (socklen_t *)&len);

    if(debug_flag)
    {
        upf_addr = htonl(upf_sockaddr.sin_addr.s_addr);
        upf_port = htons(upf_sockaddr.sin_port);

        get_ipv4_addr_string(upf_addr, string);
        printf("<----------- Rcvd response (%u bytes) from UPF (%s:%u) \n", 
                n, string, upf_port);
    }

    if(-1 == validate_rcvd_msg_on_n4_interface(g__n4_rcvd_msg_buffer, 
                                               n, 
                                               request_identifier))
    {
        printf("%s: NMP response msg validation failed. \n", __func__);
        return -1;
    }

    if(-1 == parse_nmp_msg(g__n4_rcvd_msg_buffer,
                           n,
                           &(nmp_n4_rcvd_msg_data),
                           debug_flag))
    {
        printf("%s: Message parsing error.. \n", __func__);
        return -1;
    }

    if(MSG_RESPONSE_CODE__OK == nmp_n4_rcvd_msg_data.msg_response_code)
    {
        g__amf_config.upf_session_data[session_index].ue_ipv4_addr = ue_ipv4_addr;
        memcpy(g__amf_config.upf_session_data[session_index].imsi.u8, imsi.u8, 8);
        if(debug_flag) 
        {
            printf("%s: Rcvd response from UPF is [Ok] \n", __func__);
            printf("Response Description: %s \n", nmp_n4_rcvd_msg_data.msg_response_description);
        }
        return 0;
    }
    else
    {
        printf("%s: Rcvd response from UPF is [Not Ok] \n", __func__);
        return -1;
    }
}


