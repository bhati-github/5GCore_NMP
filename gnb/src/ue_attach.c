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

#include "gnb.h"
#include "n1_msg_handler.h"
#include "ue_attach.h"

int
send_ue_attach_request_msg_to_amf(uint16_t user_id,
                                  uint8_t  debug_flag)
{
    int n = 0;
    int ret = 0;
    int len = 0;
    int offset = 0;
    char string[128];
    uint16_t item_count = 0;
    uint32_t msg_id = 0;
    uint32_t amf_addr = 0;
    uint16_t amf_port = 0;
    struct sockaddr_in amf_sockaddr;
    uint32_t request_identifier = 0;
    nmp_msg_data_t nmp_n1_send_msg_data;
    nmp_msg_data_t nmp_n1_rcvd_msg_data;

    //////////////////////////////////////
    data_64bit_t imsi;
    imsi.u8[0] = 0x11;
    imsi.u8[1] = 0x22;
    imsi.u8[2] = 0x33;
    imsi.u8[3] = 0x44;
    imsi.u8[4] = 0x55;
    imsi.u8[5] = 0x66;
    imsi.u16[3] = htons(user_id);

    uint16_t mcc = 405; 
    uint16_t mnc = 870;

    uint32_t tac = 1000;
    uint32_t cell_id = 1234;
    uint32_t tmsi = 12345678;
    //////////////////////////////////////

    uint8_t *ptr =	g__n1_send_msg_buffer;

    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;
    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__GNB);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->src_node_id    = htons(g__gnb_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__gnb_config.amf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__UE_ATTACH_REQUEST);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__gnb_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    // Save request identifier
    request_identifier = htonl(nmp_hdr_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    // Add IMSI
    ret = nmp_add_item__imsi(ptr + offset, imsi);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;


    // Add TMSI
    ret = nmp_add_item__tmsi(ptr + offset, tmsi);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;


    // Add MCC, MNC
    ret = nmp_add_item__mcc_mnc(ptr + offset, mcc, mnc);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1; 


    // Add Cell Id
    ret = nmp_add_item__cell_id(ptr + offset, cell_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add TAC
    ret = nmp_add_item__tac(ptr + offset, tac);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    nmp_hdr_ptr->msg_item_len   = htons(offset - sizeof(nmp_hdr_t));
    nmp_hdr_ptr->msg_item_count = htons(item_count);

    if(-1 == parse_nmp_msg(g__n1_send_msg_buffer,
                           offset,
                           &(nmp_n1_send_msg_data),
                           debug_flag))
    {
        printf("%s: Send message parsing error.. \n", __func__);
        return -1;
    }

    // Send this message to AMF
    n = sendto(g__gnb_config.gnb_n1_socket_id,
               (char *)g__n1_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(g__gnb_config.amf_n1_sockaddr),
               sizeof(struct sockaddr_in));

    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to AMF \n", __func__);
        return -1;
    }

    ///////////////////////////////////////////////
    // Wait for reponse from AMF
    ///////////////////////////////////////////////
    len = sizeof(struct sockaddr_in);
    memset(&amf_sockaddr, 0x0, sizeof(struct sockaddr_in));
    n = recvfrom(g__gnb_config.gnb_n1_socket_id,
                 (char *)g__n1_rcvd_msg_buffer,
                 MSG_BUFFER_LEN,
                 MSG_WAITALL,
                 (struct sockaddr *)&(amf_sockaddr),
                 (socklen_t *)&len);

    if(debug_flag)
    {
        amf_addr = htonl(amf_sockaddr.sin_addr.s_addr);
        amf_port = htons(amf_sockaddr.sin_port);

        get_ipv4_addr_string(amf_addr, string);
        printf("<----------- Rcvd response (%u bytes) from AMF (%s:%u) \n",
                n, string, amf_port);
    }

    if(-1 == validate_rcvd_msg_on_n1_interface(g__n1_rcvd_msg_buffer,
                                               n,
                                               request_identifier))
    {
        printf("%s: Rcvd message validation error.. \n", __func__);
        return -1;
    }

    if(-1 == parse_nmp_msg(g__n1_rcvd_msg_buffer,
                           n,
                           &(nmp_n1_rcvd_msg_data),
                           debug_flag))
    {
        printf("%s: Rcvd message parsing error.. \n", __func__);
        return -1;
    }

    if(MSG_RESPONSE_IS_OK == nmp_n1_rcvd_msg_data.msg_response)
    {
        printf("UE Attach Msg response is [Ok] \n");
        return 0;
    }
    else
    {
        printf("UE Attach Msg response is [Not Ok] \n");
        return -1;
    }
}

