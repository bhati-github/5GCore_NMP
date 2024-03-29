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
#include "ue_reg_and_pdu_setup.h"


int
perform_ue_reg_and_pdu_setup_procedure(uint16_t user_id,
                                       uint8_t  debug_flag)
{
    int n = 0;
    int ret = 0;
    int len = 0;
    int offset = 0;
    char time_string[128];
    char string[128];
    uint8_t *ptr =  g__n1_n2_send_msg_buffer;
    uint8_t nas_pdu[128];
    memset(nas_pdu, 0x0, 128);
    uint8_t uplink_qos_profile = 0;
    uint8_t ue_context_request = 0;
    uint16_t item_count = 0;
    uint16_t nas_pdu_len = 0;
    uint32_t msg_id = 0;
    uint32_t amf_addr = 0;
    uint32_t tunnel_ipv4_addr = 0;
    uint32_t dnlink_teid = 0;
    uint16_t amf_port = 0;
    uint8_t rrc_etsablish_cause = 3;
    uint16_t mcc = 404;
    uint16_t mnc = 10;
    uint16_t amf_ue_ngap_id = 1;
    uint16_t ran_ue_ngap_id = 1;
    uint32_t tac = 100;
    data_64bit_t nr_cell_identity;
    nr_cell_identity.u64 = 100;
    struct sockaddr_in amf_sockaddr;
    struct sockaddr_in  target_service_sockaddr;
    uint32_t request_identifier = 0;
    nmp_msg_data_t nmp_n1_n2_send_msg_data;
    nmp_msg_data_t nmp_n1_n2_rcvd_msg_data;
   
    data_64bit_t user_imsi;
    memcpy(user_imsi.u8, g__gnb_config.ue_imsi_base.u8, 8);
    user_imsi.u64 += user_id;

 
    ///////////////////////////////////////////////////////////////////////////
    // Step 1: Send Initial UE Message to AMF
    //         MSG_TYPE: MSG_TYPE__INITIAL_UE_MSG_REGISTRATION_REQ
    ///////////////////////////////////////////////////////////////////////////
    offset = 0;
    item_count = 0;
    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;
    
    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__GNB);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->src_node_id    = htons(g__gnb_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__gnb_config.amf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__INITIAL_UE_MSG_REGISTRATION_REQ);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__gnb_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    // Save request identifier
    request_identifier = htonl(nmp_hdr_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    // Add Item: RAN-UE-NGAP-ID
    ret = nmp_add_item__ran_ue_ngap_id(ptr + offset, ran_ue_ngap_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add Item: NAS-PDU (This is a item which carrries a byte stream)
    // This byte stream consists of following data..
    // -> 5gs registration tye
    // -> nas key set identifier
    // -> 5gs mobile identity
    // -> scheme output
    // TBD: Fill nas pdu
    nas_pdu_len = 36;
    ret = nmp_add_item__nas_pdu(ptr + offset, nas_pdu, nas_pdu_len);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add Gropu of Items: UserLocation Information
    ret = nmp_add_item_group__user_location_info(ptr + offset, mcc, mnc, nr_cell_identity, tac);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;
     
    // Add Item: RRC Establishment Cause
    ret = nmp_add_item__rrc_establish_cause(ptr + offset, rrc_etsablish_cause);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;
   
    // Add Item: UEContextRequest 
    ret = nmp_add_item__ue_context_request(ptr + offset, ue_context_request);
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
    target_service_sockaddr.sin_addr.s_addr = g__gnb_config.amf_n1_n2_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__gnb_config.amf_n1_n2_sockaddr.sin_port;
    n = sendto(g__gnb_config.my_n1_n2_socket_id,
               (char *)g__n1_n2_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));
    
    get_current_time(time_string);

    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to AMF \n", __func__);
        return -1;
    }

    printf("[%s] \x1b[35m gnodeB \x1b[0m -------> \x1b[36m AMF \x1b[0m [ INITIAL_UE_MSG_REGISTRATION_REQ ] \n", time_string);

    ///////////////////////////////////////////////////////////////////////////
    // Step 2: Wait for reponse from AMF. We must receive Downlink NAS 
    //         Transport Messsage from AMF
    //         MSG_TYPE: MSG_TYPE__DNLINK_NAS_TRANSPORT_AUTH_REQ 
    ///////////////////////////////////////////////////////////////////////////
    len = sizeof(struct sockaddr_in);
    memset(&amf_sockaddr, 0x0, sizeof(struct sockaddr_in));
    n = recvfrom(g__gnb_config.my_n1_n2_socket_id,
                 (char *)g__n1_n2_rcvd_msg_buffer,
                 MSG_BUFFER_LEN,
                 MSG_WAITALL,
                 (struct sockaddr *)&(amf_sockaddr),
                 (socklen_t *)&len);

    get_current_time(time_string);
    
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

    if(MSG_TYPE__DNLINK_NAS_TRANSPORT_AUTH_REQ != nmp_n1_n2_rcvd_msg_data.msg_type)
    {
        printf("We did not received DNLINK_NAS_TRANSPORT_AUTH_REQ \n");
        printf("Something wrong in core network.. \n");
        return -1;
    }

    printf("[%s] \x1b[35m gnodeB \x1b[0m <------- \x1b[36m AMF \x1b[0m [ DNLINK_NAS_TRANSPORT_AUTH_REQ ] \n", time_string);

      
    ///////////////////////////////////////////////////////////////////////////
    // Step 3: Send Uplink NAS Transport message to AMF
    //         MSG_TYPE: MSG_TYPE__UPLINK_NAS_TRANSPORT_AUTH_RESP 
    ///////////////////////////////////////////////////////////////////////////
    item_count = 0;
    nmp_hdr_ptr = (nmp_hdr_t *)ptr;

    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__GNB);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->src_node_id    = htons(g__gnb_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__gnb_config.amf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__UPLINK_NAS_TRANSPORT_AUTH_RESP);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__gnb_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    // Save request identifier
    request_identifier = htonl(nmp_hdr_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    // Add AMF-UE-NGAP-ID
    ret = nmp_add_item__amf_ue_ngap_id(ptr + offset, amf_ue_ngap_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;      

    // Add RAN-UE-NGAP-ID
    ret = nmp_add_item__ran_ue_ngap_id(ptr + offset, ran_ue_ngap_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;
    
    // Add NAS-PDU (This PDU contains Authentication Response) (gNB --> AMF)
    // TBD: Fill NAS PDU
    nas_pdu_len = 36;
    ret = nmp_add_item__nas_pdu(ptr + offset, nas_pdu, nas_pdu_len);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;
    
    // Add UserLocation Information
    ret = nmp_add_item_group__user_location_info(ptr + offset, mcc, mnc, nr_cell_identity, tac);
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
    target_service_sockaddr.sin_addr.s_addr = g__gnb_config.amf_n1_n2_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__gnb_config.amf_n1_n2_sockaddr.sin_port;
    n = sendto(g__gnb_config.my_n1_n2_socket_id,
               (char *)g__n1_n2_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));
    
    get_current_time(time_string);

    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to AMF \n", __func__);
        return -1;
    }
    
    printf("[%s] \x1b[35m gnodeB \x1b[0m -------> \x1b[36m AMF \x1b[0m [ UPLINK_NAS_TRANSPORT_AUTH_RESP ] \n", time_string);

    ///////////////////////////////////////////////////////////////////////////
    // Step 4: Wait for reponse from AMF. We must receive Downlink NAS
    //         Transport Messsage from AMF
    //         MSG_TYPE: MSG_TYPE__DNLINK_NAS_TRANSPORT_REGISTRATION_ACCEPT 
    ///////////////////////////////////////////////////////////////////////////
    len = sizeof(struct sockaddr_in);
    memset(&amf_sockaddr, 0x0, sizeof(struct sockaddr_in));
    n = recvfrom(g__gnb_config.my_n1_n2_socket_id,
                 (char *)g__n1_n2_rcvd_msg_buffer,
                 MSG_BUFFER_LEN,
                 MSG_WAITALL,
                 (struct sockaddr *)&(amf_sockaddr),
                 (socklen_t *)&len);
    
    get_current_time(time_string);

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

    if(MSG_TYPE__DNLINK_NAS_TRANSPORT_REGISTRATION_ACCEPT != nmp_n1_n2_rcvd_msg_data.msg_type)
    {
        printf("We did not received DNLINK_NAS_TRANSPORT_REGISTRATION_ACCEPT \n");
        printf("Something wrong in core network.. \n");
        return -1;
    }

    printf("[%s] \x1b[35m gnodeB \x1b[0m <------- \x1b[36m AMF \x1b[0m [ DNLINK_NAS_TRANSPORT_REGISTRATION_ACCEPT ] \n", time_string);

    

    ///////////////////////////////////////////////////////////////////////////
    // Step 5: Send Uplink NAS Transport message to AMF
    //         MSG_TYPE: MSG_TYPE__UPLINK_NAS_TRANSPORT_REGISTRATION_COMPLETE
    ///////////////////////////////////////////////////////////////////////////
    item_count = 0;
    nmp_hdr_ptr = (nmp_hdr_t *)ptr;

    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__GNB);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->src_node_id    = htons(g__gnb_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__gnb_config.amf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__UPLINK_NAS_TRANSPORT_REGISTRATION_COMPLETE);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__gnb_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    // Save request identifier
    request_identifier = htonl(nmp_hdr_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    // Add AMF-UE-NGAP-ID
    ret = nmp_add_item__amf_ue_ngap_id(ptr + offset, amf_ue_ngap_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add RAN-UE-NGAP-ID
    ret = nmp_add_item__ran_ue_ngap_id(ptr + offset, ran_ue_ngap_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add NAS-PDU (This PDU contains Registration Complete)
    // TBD: Fill NAS PDU
    nas_pdu_len = 36;
    ret = nmp_add_item__nas_pdu(ptr + offset, nas_pdu, nas_pdu_len);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add UserLocation Information
    ret = nmp_add_item_group__user_location_info(ptr + offset, mcc, mnc, nr_cell_identity, tac);
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
    target_service_sockaddr.sin_addr.s_addr = g__gnb_config.amf_n1_n2_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__gnb_config.amf_n1_n2_sockaddr.sin_port;
    n = sendto(g__gnb_config.my_n1_n2_socket_id,
               (char *)g__n1_n2_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));
    
    get_current_time(time_string);

    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to AMF \n", __func__);
        return -1;
    }
    
    printf("[%s] \x1b[35m gnodeB \x1b[0m -------> \x1b[36m AMF \x1b[0m [ UPLINK_NAS_TRANSPORT_REGISTRATION_COMPLETE ] \n", time_string);
    


    

    ///////////////////////////////////////////////////////////////////////////
    // Step 6: Send Uplink NAS Transport message to AMF
    //         MSG_TYPE: MSG_TYPE__UPLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REQ
    ///////////////////////////////////////////////////////////////////////////
    item_count = 0;
    nmp_hdr_ptr = (nmp_hdr_t *)ptr;

    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__GNB);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->src_node_id    = htons(g__gnb_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__gnb_config.amf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__UPLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REQ);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__gnb_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    // Save request identifier
    request_identifier = htonl(nmp_hdr_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    // Add AMF-UE-NGAP-ID
    ret = nmp_add_item__amf_ue_ngap_id(ptr + offset, amf_ue_ngap_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add RAN-UE-NGAP-ID
    ret = nmp_add_item__ran_ue_ngap_id(ptr + offset, ran_ue_ngap_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add NAS-PDU (This PDU contains PDU Session Establishment Request)
    // TBD: Fill NAS PDU
    nas_pdu_len = 36;
    ret = nmp_add_item__nas_pdu(ptr + offset, nas_pdu, nas_pdu_len);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add UserLocation Information
    ret = nmp_add_item_group__user_location_info(ptr + offset, mcc, mnc, nr_cell_identity, tac);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add user IMSI
    ret = nmp_add_item__imsi(ptr+ offset, user_imsi);
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
    target_service_sockaddr.sin_addr.s_addr = g__gnb_config.amf_n1_n2_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__gnb_config.amf_n1_n2_sockaddr.sin_port;
    n = sendto(g__gnb_config.my_n1_n2_socket_id,
               (char *)g__n1_n2_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));

    get_current_time(time_string);
    
    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to AMF \n", __func__);
        return -1;
    }
    
    printf("[%s] \x1b[35m gnodeB \x1b[0m -------> \x1b[36m AMF \x1b[0m [ UPLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REQ ] \n", time_string);

    ///////////////////////////////////////////////////////////////////////////
    // Step 7: Wait for reponse from AMF. We must receive Downlink NAS
    //         Transport Messsage from AMF
    //         MSG_TYPE: MSG_TYPE__DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_ACCEPT
    ///////////////////////////////////////////////////////////////////////////
    len = sizeof(struct sockaddr_in);
    memset(&amf_sockaddr, 0x0, sizeof(struct sockaddr_in));
    n = recvfrom(g__gnb_config.my_n1_n2_socket_id,
                 (char *)g__n1_n2_rcvd_msg_buffer,
                 MSG_BUFFER_LEN,
                 MSG_WAITALL,
                 (struct sockaddr *)&(amf_sockaddr),
                 (socklen_t *)&len);
    
    get_current_time(time_string);

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

    if(MSG_TYPE__DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_ACCEPT != nmp_n1_n2_rcvd_msg_data.msg_type)
    {
        printf("We did not received DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_ACCEPT \n");
        printf("Something wrong in core network.. \n");
        return -1;
    }
    
    printf("[%s] \x1b[35m gnodeB \x1b[0m <------- \x1b[36m AMF \x1b[0m [ DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_ACCEPT ] \n", time_string);
    
    // We must have received uplink teid endpoint info
    // Store in gnb teid database.. 
    g__gnb_config.ue_session_data[user_id].upf_n3_addr = nmp_n1_n2_rcvd_msg_data.upf_n3_addr; 
    g__gnb_config.ue_session_data[user_id].upf_n3_teid = nmp_n1_n2_rcvd_msg_data.upf_n3_teid;




    ///////////////////////////////////////////////////////////////////////////
    // Step 8: Send PDUSessionResourceSetupResponse to AMF
    //         MSG_TYPE: MSG_TYPE__PDU_SESSION_RESOURCE_SETUP_RESP
    ///////////////////////////////////////////////////////////////////////////
    item_count = 0;
    nmp_hdr_ptr = (nmp_hdr_t *)ptr;

    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__GNB);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->src_node_id    = htons(g__gnb_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__gnb_config.amf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__PDU_SESSION_RESOURCE_SETUP_RESP);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__gnb_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    // Save request identifier
    request_identifier = htonl(nmp_hdr_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    // Add AMF-UE-NGAP-ID
    ret = nmp_add_item__amf_ue_ngap_id(ptr + offset, amf_ue_ngap_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add RAN-UE-NGAP-ID
    ret = nmp_add_item__ran_ue_ngap_id(ptr + offset, ran_ue_ngap_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add user IMSI
    ret = nmp_add_item__imsi(ptr+ offset, user_imsi);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // gnodeB will create a TEID endpoint on its N3 interface.
    // This TEID endpoint is used for sending downlink packets by UPF.
    // Add GTP-U TEID endpoint for downlink direction (gtp-u tunnel from UPF to gnodeB)
    tunnel_ipv4_addr = g__gnb_config.my_n3_addr.u.v4_addr;  // gnodeB N3 interface IP
    dnlink_teid = GNB_DNLINK_TEID_BASE + user_id;

    // Save this teid info into gnb database also..
    g__gnb_config.ue_session_data[user_id].gnb_n3_addr = tunnel_ipv4_addr;
    g__gnb_config.ue_session_data[user_id].gnb_n3_teid = dnlink_teid;

    // Add actual item into NMP payload..  
    ret = nmp_add_item__dnlink_gtpu_ipv4_endpoint(ptr + offset, 
                                                  tunnel_ipv4_addr, 
                                                  dnlink_teid);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add Qos flow Identifier
    ret = nmp_add_item__uplink_qos_profile(ptr + offset, uplink_qos_profile);
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
    target_service_sockaddr.sin_addr.s_addr = g__gnb_config.amf_n1_n2_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__gnb_config.amf_n1_n2_sockaddr.sin_port;
    n = sendto(g__gnb_config.my_n1_n2_socket_id,
               (char *)g__n1_n2_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));
    
    get_current_time(time_string);

    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to AMF \n", __func__);
        return -1;
    }

    printf("[%s] \x1b[35m gnodeB \x1b[0m -------> \x1b[36m AMF \x1b[0m [ PDU_SESSION_RESOURCE_SETUP_RESP ] \n", time_string);


    ///////////////////////////////////////////////////////////////////////////
    // Step 9: Wait for final reponse from AMF. 
    //         We must receive message Type OK from AMF
    //         It will indicate end of procedure (UE Registration and PDU 
    //         Session Establishment) between gNB and AMF.
    //         MSG_TYPE: MSG_TYPE__ALL_OK
    ///////////////////////////////////////////////////////////////////////////
    len = sizeof(struct sockaddr_in);
    memset(&amf_sockaddr, 0x0, sizeof(struct sockaddr_in));
    n = recvfrom(g__gnb_config.my_n1_n2_socket_id,
                 (char *)g__n1_n2_rcvd_msg_buffer,
                 MSG_BUFFER_LEN,
                 MSG_WAITALL,
                 (struct sockaddr *)&(amf_sockaddr),
                 (socklen_t *)&len);
    
    get_current_time(time_string);

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

    if(MSG_TYPE__ALL_OK == nmp_n1_n2_rcvd_msg_data.msg_type)
    {
        get_current_time(time_string);
        printf("[%s] \x1b[35m gnodeB \x1b[0m <------- \x1b[36m AMF \x1b[0m [ All Ok ] \n", time_string);
        return 0;
    }
    else
    {
        printf("\x1b[35m gnodeB \x1b[0m <------- \x1b[36m AMF \x1b[0m \x1b[31m [ Not Ok ] \x1b[0m \n");
        return -1;
    }
}

