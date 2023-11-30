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
#include "color_print.h"

#include "gnb.h"
#include "n1_n2_msg_handler.h"
#include "ng_setup.h"


int
perform_ng_setup_procedure(uint8_t  debug_flag)
{
    int n = 0;
    int ret = 0;
    int len = 0;
    int offset = 0;
    char string[128];
    uint8_t *ptr =  g__n1_n2_send_msg_buffer;
    uint8_t default_paging_drx = 3; // 0 for 32, 1 for 64, 2 for 128, 3 for 256 ..
    uint8_t id_string[256];
    uint8_t ran_node_name[256];
    ta_item_t ta_items[4];
    uint16_t ta_item_count = 0;
    uint16_t item_count = 0;
    uint32_t ran_node_id = 0;
    uint32_t msg_id = 0;
    uint32_t amf_addr = 0;
    uint16_t amf_port = 0;
    uint16_t mcc = 404;
    uint16_t mnc = 10;
    struct sockaddr_in amf_sockaddr;
    uint32_t request_identifier = 0;
    nmp_msg_data_t nmp_n1_n2_send_msg_data;
    nmp_msg_data_t nmp_n1_n2_rcvd_msg_data;
    
    ///////////////////////////////////////////////////////////////////////////
    // Step 1: Send NGSetupRequest message to AMF
    //         (This is a one time packet sent from gnodeB to AMF for NGSetup)
    //         MSG_TYPE: MSG_TYPE__NG_SETUP_REQ
    ///////////////////////////////////////////////////////////////////////////
    offset = 0;
    item_count = 0;
    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;
    
    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__GNB);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->src_node_id    = htons(g__gnb_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__gnb_config.amf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__NG_SETUP_REQ);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__gnb_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    // Save request identifier
    request_identifier = htonl(nmp_hdr_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    // Add GLOBAL-RAN-NODE-ID
    ran_node_id = g__gnb_config.my_id;
    ret = nmp_add_item_group__global_ran_node_id(ptr + offset, mcc, mnc, ran_node_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add RAN-NODE-NAME
    sprintf((char *)id_string, "%u", g__gnb_config.my_id);
    strcpy((char *)ran_node_name, "gnodeB-");
    strcat((char *)ran_node_name, (const char *)id_string);
    ret = nmp_add_item__ran_node_name(ptr + offset, ran_node_name, strlen((const char *)ran_node_name));
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add Supported TA List (Adding just 1 TA item)
    ta_item_count = 1;
    ta_items[0].mcc = 404;
    ta_items[0].mnc = 10;
    ta_items[0].tai_slice_support_item_count = 1;
    ta_items[0].tai_slice_support_item[0].nssai_sst = 1;
    ta_items[0].tai_slice_support_item[0].nssai_sd  = 1;
    ret = nmp_add_item_group__supported_ta_list(ptr + offset, ta_items, ta_item_count);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;
     
    // Add Default Paging DRX
    ret = nmp_add_item__default_paging_drx(ptr + offset, default_paging_drx);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;
    
    // All items are added. Update NMP message header.
    nmp_hdr_ptr->msg_item_len   = htons(offset - sizeof(nmp_hdr_t));
    nmp_hdr_ptr->msg_item_count = htons(item_count);

    if(-1 == parse_nmp_msg(g__n1_n2_send_msg_buffer,
                           offset,
                           &(nmp_n1_n2_send_msg_data),
                           debug_flag))
    {
        printf("%s: Send message parsing error.. \n", __func__);
        return -1;
    }

    // Send this message to AMF
    n = sendto(g__gnb_config.gnb_n1_n2_socket_id,
               (char *)g__n1_n2_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(g__gnb_config.amf_n1_n2_sockaddr),
               sizeof(struct sockaddr_in));

    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to AMF \n", __func__);
        return -1;
    }
    if(debug_flag) 
    {
        MAGENTA_PRINT("NG Setup Request sent to AMF ! \n");
        YELLOW_PRINT("Waiting for response from AMF............... \n");
        printf("\n");
    }

    ///////////////////////////////////////////////////////////////////////////
    // Step 2: Wait for reponse from AMF. We must receive NGSetupResponse 
    //         message from AMF
    //         MSG_TYPE: MSG_TYPE__NG_SETUP_RESP 
    ///////////////////////////////////////////////////////////////////////////
    len = sizeof(struct sockaddr_in);
    memset(&amf_sockaddr, 0x0, sizeof(struct sockaddr_in));
    n = recvfrom(g__gnb_config.gnb_n1_n2_socket_id,
                 (char *)g__n1_n2_rcvd_msg_buffer,
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

    if(-1 == validate_rcvd_msg_on_n1_n2_interface(g__n1_n2_rcvd_msg_buffer,
                                                  n,
                                                  request_identifier))
    {
        printf("%s: Rcvd message validation error.. \n", __func__);
        return -1;
    }

    if(-1 == parse_nmp_msg(g__n1_n2_rcvd_msg_buffer,
                           n,
                           &(nmp_n1_n2_rcvd_msg_data),
                           debug_flag))
    {
        printf("%s: Rcvd message parsing error.. \n", __func__);
        return -1;
    }

    GREEN_PRINT("-------------------------------------------\n");
    GREEN_PRINT("NG Setup Procedure is [OK] with target AMF \n");
    GREEN_PRINT("-------------------------------------------\n");
    printf("\n\n");
    return 0;
}

