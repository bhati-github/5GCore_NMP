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
#include "n1_n2_msg_handler.h"
#include "Namf_msg_handler.h"

int
validate_rcvd_nmp_msg_on_n1_n2_interface(uint8_t *msg_ptr,
                                         int      msg_len)
{
    if(msg_len < sizeof(nmp_hdr_t))
    {
        printf("Invalid size of NMP message.. \n");
        return -1;
    }

    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)msg_ptr;

    if(NODE_TYPE__AMF != htons(nmp_hdr_ptr->dst_node_type))
    {
        printf("Destination Node of NMP message is not AMF \n");
        return -1;
    }

    if(g__amf_config.my_id != htons(nmp_hdr_ptr->dst_node_id))
    {
        printf("Destination node id is not equal to my id \n");
        return -1;
    }

    return 0;
}


int
get_gnb_index_from_v4_addr(uint32_t  gnb_n1_n2_addr,
                           uint16_t *gnb_index)
{
    int i = 0;
    for(i = 0; i < g__amf_config.gnb_count; i++)
    {
        if(gnb_n1_n2_addr == g__amf_config.gnb_data[i].gnb_n1_n2_addr.u.v4_addr)
        {
            *gnb_index = i;
            return 0;
        }
    }

    return -1;
}


int
send_all_ok_msg_to_gnodeb(nmp_msg_data_t *nmp_n1_n2_rcvd_msg_data_ptr,
                          uint8_t         debug_flag)
{
    int n = 0;
    int offset = 0;
    int gnb_index = 0;
    uint8_t *ptr = g__n1_n2_send_msg_buffer;
    uint16_t dst_node_id = 0;
    uint16_t item_count = 0;
    nmp_msg_data_t nmp_n1_n2_send_msg_data;
    struct sockaddr_in  target_service_sockaddr;

    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;
    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__GNB);
    nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);

    dst_node_id = (nmp_n1_n2_rcvd_msg_data_ptr->msg_identifier >> 16) & 0xffff;
    nmp_hdr_ptr->dst_node_id    = htons(dst_node_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__ALL_OK);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    nmp_hdr_ptr->msg_identifier = htonl(nmp_n1_n2_rcvd_msg_data_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    nmp_hdr_ptr->msg_item_len   = htons(offset - sizeof(nmp_hdr_t));
    nmp_hdr_ptr->msg_item_count = htons(item_count);

    if(-1 == parse_nmp_msg(ptr,
                           offset,
                           &(nmp_n1_n2_send_msg_data),
                           debug_flag))
    {
        printf("%s: Msg parse error. \n", __func__);
        return -1;
    }

    ////////////////////////////////////////////////
    // write this msg on n1 socket (towards gnodeb)
    ////////////////////////////////////////////////
    gnb_index = nmp_n1_n2_rcvd_msg_data_ptr->gnb_index;
    target_service_sockaddr.sin_addr.s_addr = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_port;
    n = sendto(g__amf_config.my_n1_n2_socket_id,
               (char *)ptr,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));
    if(n != offset)
    {
        printf("%s: sendto() failed \n", __func__);
        return -1;
    }

    printf("%s: Msg sent successfully to gnodeb \n", __func__);
    return 0;
}

int
send_pdu_setup_failure_msg_to_gnodeb(nmp_msg_data_t *nmp_n1_n2_rcvd_msg_data_ptr,
                                     uint8_t         debug_flag)
{
    int n = 0;
    int offset = 0;
    int gnb_index = 0;
    uint8_t *ptr = g__n1_n2_send_msg_buffer;
    uint16_t dst_node_id = 0;
    uint16_t item_count = 0;
    nmp_msg_data_t nmp_n1_n2_send_msg_data;
    struct sockaddr_in  target_service_sockaddr;

    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;
    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__GNB);
    nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);

    dst_node_id = (nmp_n1_n2_rcvd_msg_data_ptr->msg_identifier >> 16) & 0xffff;
    nmp_hdr_ptr->dst_node_id    = htons(dst_node_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REJECT);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    nmp_hdr_ptr->msg_identifier = htonl(nmp_n1_n2_rcvd_msg_data_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    nmp_hdr_ptr->msg_item_len   = htons(offset - sizeof(nmp_hdr_t));
    nmp_hdr_ptr->msg_item_count = htons(item_count);

    if(-1 == parse_nmp_msg(ptr,
                           offset,
                           &(nmp_n1_n2_send_msg_data),
                           debug_flag))
    {
        printf("%s: Msg parse error. \n", __func__);
        return -1;
    }

    ////////////////////////////////////////////////
    // write this msg on n1 socket (towards gnodeb)
    ////////////////////////////////////////////////
    gnb_index = nmp_n1_n2_rcvd_msg_data_ptr->gnb_index;
    target_service_sockaddr.sin_addr.s_addr = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_port;
    n = sendto(g__amf_config.my_n1_n2_socket_id,
               (char *)ptr,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));
    if(n != offset)
    {
        printf("%s: sendto() failed \n", __func__);
        return -1;
    }

    printf("%s: Msg sent successfully to gnodeb \n", __func__);
    return 0;

}


int
process_rcvd_n1_n2_msg(nmp_msg_data_t *nmp_n1_n2_rcvd_msg_data_ptr,
                       uint32_t        gnb_n1_n2_addr,
                       uint8_t         debug_flag)
{
    int n = 0;
    int ret = 0;
    int offset = 0;
    uint16_t item_count = 0;
    uint16_t dst_node_id = 0;
    nmp_hdr_t *nmp_hdr_ptr = NULL;

    char string[128];
    uint8_t nas_pdu[128];
    uint16_t nas_pdu_len = 0;
    uint8_t *ptr = g__n1_n2_send_msg_buffer;
    uint8_t relative_amf_capacity = 1;
    uint8_t amf_name[256];
    uint16_t gnb_index = 0;
    uint16_t amf_ue_ngap_id = 1;
    uint16_t ran_ue_ngap_id = 1;
    uint32_t msg_id = 0;
    guami_item_t guami_items[4];
    uint16_t guami_item_count = 0;
    plmn_item_t plmn_items[4];
    uint16_t plmn_item_count = 0;
    nmp_msg_data_t nmp_n1_n2_send_msg_data;
    struct sockaddr_in  target_service_sockaddr;

    if(MSG_TYPE__NG_SETUP_REQ == nmp_n1_n2_rcvd_msg_data_ptr->msg_type)
    {
        if(debug_flag) printf("%s: Rcvd MsgType = NG_SETUP_REQ \n", __func__);
        if(-1 == get_gnb_index_from_v4_addr(gnb_n1_n2_addr, &gnb_index))
        {
            get_ipv4_addr_string(gnb_n1_n2_addr, string);
            printf("Unable to find a registered gnodeb with ipv4 address %s \n", string);
            printf("Rejecting this message from Unknown gnodeb \n");
            return -1;
        }
        nmp_n1_n2_rcvd_msg_data_ptr->gnb_index = gnb_index;

        if(debug_flag) printf("%s: gnb_index = %u \n", __func__, gnb_index);

        //////////////////////////////////////////////////////
        // Send Authentication Request message (Downlink NAS
        // transport message) back to gnodeb
        //////////////////////////////////////////////////////
        offset = 0;
        item_count = 0;
        nmp_hdr_ptr = (nmp_hdr_t *)ptr;

        nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
        nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__GNB);
        nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);
        dst_node_id = (nmp_n1_n2_rcvd_msg_data_ptr->msg_identifier >> 16) & 0xffff;
        nmp_hdr_ptr->dst_node_id    = htons(dst_node_id);

        nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__NG_SETUP_RESP);
        nmp_hdr_ptr->msg_item_len   = 0;
        nmp_hdr_ptr->msg_item_count = 0;

        msg_id = g__amf_config.my_id << 16;
        msg_id |= (uint16_t )rand();
        nmp_hdr_ptr->msg_identifier = htonl(msg_id);

        offset = sizeof(nmp_hdr_t);

        // Add AMF-Name
        strcpy((char *)amf_name, "amf.5gcore.mcc404.mnc10.3gppnetwork.org");
        ret = nmp_add_item__amf_name(ptr + offset, amf_name, strlen((const char *)amf_name));
        if(-1 == ret)
        {
            return -1;
        }
        offset += ret;
        item_count += 1;

        // Add Served GUAMI List
        guami_items[0].mcc = htons(404); 
        guami_items[0].mnc = htons(10);
        guami_items[0].amf_region_id = htons(100); 
        guami_items[0].amf_set_id    = htons(100);
        guami_items[0].amf_pointer   = htons(23);
        guami_item_count = 1;
        ret = nmp_add_item_group__guami_list(ptr + offset, guami_items, guami_item_count);
        if(-1 == ret)
        {
            return -1;
        }
        offset += ret;
        item_count += 1;

        // Add RelativeAMFCapacity
        ret = nmp_add_item__relative_amf_capacity(ptr + offset, relative_amf_capacity);
        if(-1 == ret)
        {
            return -1;
        }
        offset += ret;
        item_count += 1;

        // PLMN Support List
        plmn_item_count = 1;
        plmn_items[0].mcc = 404; 
        plmn_items[0].mnc = 10;
        plmn_items[0].slice_support_item_count = 1;
        plmn_items[0].slice_support_item[0].nssai_sst = 1; 
        plmn_items[0].slice_support_item[0].nssai_sd  = 1;
        ret = nmp_add_item_group__plmn_support_list(ptr + offset, plmn_items, plmn_item_count);
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

        ////////////////////////////////////////////////
        // write this msg on n1 socket (towards gnodeb)
        ////////////////////////////////////////////////
        target_service_sockaddr.sin_addr.s_addr = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_addr.s_addr;
        target_service_sockaddr.sin_port = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_port;
        n = sendto(g__amf_config.my_n1_n2_socket_id,
                   (char *)ptr,
                   offset,
                   MSG_WAITALL,
                   (struct sockaddr *)&(target_service_sockaddr),
                   sizeof(struct sockaddr_in));
        if(n != offset)
        {
            printf("%s: sendto() failed \n", __func__);
            return -1;
        }

        return 0;
    }
    else if(MSG_TYPE__INITIAL_UE_MSG_REGISTRATION_REQ == nmp_n1_n2_rcvd_msg_data_ptr->msg_type)
    {
        if(debug_flag) printf("%s: Rcvd MsgType = INITIAL_UE_MSG_REGISTRATION_REQ \n", __func__);
 
        if(-1 == get_gnb_index_from_v4_addr(gnb_n1_n2_addr, &gnb_index))
        {
            get_ipv4_addr_string(gnb_n1_n2_addr, string);
            printf("Unable to find a registered gnodeb with ipv4 address %s \n", string);
            printf("Rejecting this message from Unknown gnodeb \n");
            return -1;
        }
        nmp_n1_n2_rcvd_msg_data_ptr->gnb_index = gnb_index;

        if(debug_flag) printf("%s: gnb_index = %u \n", __func__, gnb_index);

        //////////////////////////////////////////////////////
        // Send Authentication Request message (Downlink NAS 
        // transport message) back to gnodeb 
        //////////////////////////////////////////////////////
        offset = 0;
        item_count = 0;
        nmp_hdr_ptr = (nmp_hdr_t *)ptr;

        nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
        nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__GNB);
        nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);
        dst_node_id = (nmp_n1_n2_rcvd_msg_data_ptr->msg_identifier >> 16) & 0xffff;
        nmp_hdr_ptr->dst_node_id    = htons(dst_node_id);

        nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__DNLINK_NAS_TRANSPORT_AUTH_REQ);
        nmp_hdr_ptr->msg_item_len   = 0;
        nmp_hdr_ptr->msg_item_count = 0;

        msg_id = g__amf_config.my_id << 16;
        msg_id |= (uint16_t )rand();
        nmp_hdr_ptr->msg_identifier = htonl(msg_id);

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

        // Add NAS-PDU (This PDU contains Authentication Request) (AMF --> gNB)
        // TBD: Fill nas pdu
        nas_pdu_len = 36;  // dummy 36 bytes
        ret = nmp_add_item__nas_pdu(ptr + offset, nas_pdu, nas_pdu_len);
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

        ////////////////////////////////////////////////
        // write this msg on n1 socket (towards gnodeb)
        ////////////////////////////////////////////////
        target_service_sockaddr.sin_addr.s_addr = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_addr.s_addr;
        target_service_sockaddr.sin_port = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_port;
        n = sendto(g__amf_config.my_n1_n2_socket_id,
                   (char *)ptr,
                   offset,
                   MSG_WAITALL,
                   (struct sockaddr *)&(target_service_sockaddr),
                   sizeof(struct sockaddr_in));
        if(n != offset)
        {
            printf("%s: sendto() failed \n", __func__);
            return -1;
        }

        return 0;
    }
    else if(MSG_TYPE__UPLINK_NAS_TRANSPORT_AUTH_RESP == nmp_n1_n2_rcvd_msg_data_ptr->msg_type)
    {
        if(debug_flag) printf("%s: Rcvd MsgType = UPLINK_NAS_TRANSPORT_AUTH_RESP \n", __func__);

        if(-1 == get_gnb_index_from_v4_addr(gnb_n1_n2_addr, &gnb_index))
        {
            get_ipv4_addr_string(gnb_n1_n2_addr, string);
            printf("Unable to find a registered gnodeb with ipv4 address %s \n", string);
            printf("Rejecting this message from Unknown gnodeb \n");
            return -1;
        }
        nmp_n1_n2_rcvd_msg_data_ptr->gnb_index = gnb_index;

        if(debug_flag) printf("%s: gnb_index = %u \n", __func__, gnb_index);

        //////////////////////////////////////////////////////
        // Send Registration Accept message (Downlink NAS
        // transport message) back to gnodeb
        //////////////////////////////////////////////////////
        offset = 0;
        item_count = 0;
        nmp_hdr_ptr = (nmp_hdr_t *)g__n1_n2_send_msg_buffer;

        nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
        nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__GNB);
        nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);
        dst_node_id = (nmp_n1_n2_rcvd_msg_data_ptr->msg_identifier >> 16) & 0xffff;
        nmp_hdr_ptr->dst_node_id    = htons(dst_node_id);

        nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__DNLINK_NAS_TRANSPORT_REGISTRATION_ACCEPT);
        nmp_hdr_ptr->msg_item_len   = 0;
        nmp_hdr_ptr->msg_item_count = 0;

        msg_id = g__amf_config.my_id << 16;
        msg_id |= (uint16_t )rand();
        nmp_hdr_ptr->msg_identifier = htonl(msg_id);

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

        // Add NAS-PDU (This PDU contains Registration Accept) (AMF --> gNB)
        // TBD: Fill nas pdu
        nas_pdu_len = 36;  // dummy 36 bytes
        ret = nmp_add_item__nas_pdu(ptr + offset, nas_pdu, nas_pdu_len);
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

        ////////////////////////////////////////////////
        // write this msg on n1 socket (towards gnodeb)
        ////////////////////////////////////////////////
        target_service_sockaddr.sin_addr.s_addr = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_addr.s_addr;
        target_service_sockaddr.sin_port = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_port;
        n = sendto(g__amf_config.my_n1_n2_socket_id,
                   (char *)ptr,
                   offset,
                   MSG_WAITALL,
                   (struct sockaddr *)&(target_service_sockaddr),
                   sizeof(struct sockaddr_in));
        if(n != offset)
        {
            printf("%s: sendto() failed \n", __func__);
            return -1;
        }

        return 0;
    }
    else if(MSG_TYPE__UPLINK_NAS_TRANSPORT_REGISTRATION_COMPLETE == nmp_n1_n2_rcvd_msg_data_ptr->msg_type)
    {
        if(debug_flag) printf("%s: Rcvd MsgType = UPLINK_NAS_TRANSPORT_REGISTRATION_COMPLETE \n", __func__);

        if(-1 == get_gnb_index_from_v4_addr(gnb_n1_n2_addr, &gnb_index))
        {
            get_ipv4_addr_string(gnb_n1_n2_addr, string);
            printf("Unable to find a registered gnodeb with ipv4 address %s \n", string);
            printf("Rejecting this message from Unknown gnodeb \n");
            return -1;
        }
        nmp_n1_n2_rcvd_msg_data_ptr->gnb_index = gnb_index;

        if(debug_flag) printf("%s: gnb_index = %u \n", __func__, gnb_index);

        // No need to send back any message to gnodeb. UE Registration is complete at this stage.
        return 0;
    }
    else if(MSG_TYPE__UPLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REQ == nmp_n1_n2_rcvd_msg_data_ptr->msg_type)
    {
        if(debug_flag) printf("%s: Rcvd MsgType = UPLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REQ \n", __func__);

        if(-1 == get_gnb_index_from_v4_addr(gnb_n1_n2_addr, &gnb_index))
        {
            get_ipv4_addr_string(gnb_n1_n2_addr, string);
            printf("Unable to find a registered gnodeb with ipv4 address %s \n", string);
            printf("Rejecting this message from Unknown gnodeb \n");
            return -1;
        }
        nmp_n1_n2_rcvd_msg_data_ptr->gnb_index = gnb_index;

        if(debug_flag) printf("%s: gnb_index = %u \n", __func__, gnb_index);

       
        /////////////////////////////////////////////// 
        // Step-1: Send Session Create message to SMF 
        /////////////////////////////////////////////// 
        if(-1 == send_session_create_msg_to_smf(nmp_n1_n2_rcvd_msg_data_ptr->imsi, debug_flag))
        {
            // Send failure message back to gnodeb
            return send_pdu_setup_failure_msg_to_gnodeb(nmp_n1_n2_rcvd_msg_data_ptr, debug_flag);
        }
    
        
        /////////////////////////////////////////////////// 
        // Step-2: Send PDU Session Establishment Accept 
        //         message (Downlink NAS Transport message) 
        //         back to gnodeb
        /////////////////////////////////////////////////// 
        offset = 0;
        item_count = 0;
        nmp_hdr_ptr = (nmp_hdr_t *)g__n1_n2_send_msg_buffer;

        nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
        nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__GNB);
        nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);
        dst_node_id = (nmp_n1_n2_rcvd_msg_data_ptr->msg_identifier >> 16) & 0xffff;
        nmp_hdr_ptr->dst_node_id    = htons(dst_node_id);

        nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__DNLINK_NAS_TRANSPORT_REGISTRATION_ACCEPT);
        nmp_hdr_ptr->msg_item_len   = 0;
        nmp_hdr_ptr->msg_item_count = 0;

        msg_id = g__amf_config.my_id << 16;
        msg_id |= (uint16_t )rand();
        nmp_hdr_ptr->msg_identifier = htonl(msg_id);

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

        // Add NAS-PDU (This PDU contains Registration Accept) (AMF --> gNB)
        // TBD: Fill nas pdu
        nas_pdu_len = 36;  // dummy 36 bytes
        ret = nmp_add_item__nas_pdu(ptr + offset, nas_pdu, nas_pdu_len);
        if(-1 == ret)
        {
            return -1;
        }
        offset += ret;
        item_count += 1;

        // Add uplink teid info (gnodeB will use this for making uplink gtp-u tunnel packets)
        ret = nmp_add_item__uplink_gtpu_ipv4_endpoint(ptr + offset, 
                                                      g__amf_config.smf_sessions[g__amf_ue_session_index].upf_n3_addr,
                                                      g__amf_config.smf_sessions[g__amf_ue_session_index].upf_n3_teid);
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

        ////////////////////////////////////////////////
        // write this msg on n1 socket (towards gnodeb)
        ////////////////////////////////////////////////
        target_service_sockaddr.sin_addr.s_addr = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_addr.s_addr;
        target_service_sockaddr.sin_port = g__amf_config.gnb_data[gnb_index].gnb_n1_n2_sockaddr.sin_port;
        n = sendto(g__amf_config.my_n1_n2_socket_id,
                   (char *)ptr,
                   offset,
                   MSG_WAITALL,
                   (struct sockaddr *)&(target_service_sockaddr),
                   sizeof(struct sockaddr_in));
        if(n != offset)
        {
            printf("%s: sendto() failed \n", __func__);
            return -1;
        }

        return 0;
    }
    else if(MSG_TYPE__PDU_SESSION_RESOURCE_SETUP_RESP == nmp_n1_n2_rcvd_msg_data_ptr->msg_type)
    {
        if(debug_flag) printf("%s: Rcvd MsgType = PDU_SESSION_RESOURCE_SETUP_RESP \n", __func__);

        if(-1 == get_gnb_index_from_v4_addr(gnb_n1_n2_addr, &gnb_index))
        {
            get_ipv4_addr_string(gnb_n1_n2_addr, string);
            printf("Unable to find a registered gnodeb with ipv4 address %s \n", string);
            printf("Rejecting this message from Unknown gnodeb \n");
            return -1;
        }
        nmp_n1_n2_rcvd_msg_data_ptr->gnb_index = gnb_index;

        if(debug_flag) printf("%s: gnb_index = %u \n", __func__, gnb_index);

        // gnodeB sends its own N3 interface teid endpoint. 
        // UPF will use this for making downlink gtp-u tunnel.
        g__amf_config.smf_sessions[g__amf_ue_session_index].gnb_n3_addr = nmp_n1_n2_rcvd_msg_data_ptr->gnb_n3_addr;
        g__amf_config.smf_sessions[g__amf_ue_session_index].gnb_n3_teid = nmp_n1_n2_rcvd_msg_data_ptr->gnb_n3_teid;

        ///////////////////////////////////////////////
        // Step-1: Send Session Modify message to SMF
        //         SMF will in-turn send a message 
        //         to UPF for datapath setup.
        ///////////////////////////////////////////////
        if(-1 == send_session_modify_msg_to_smf(nmp_n1_n2_rcvd_msg_data_ptr->imsi,
                                                g__amf_config.smf_sessions[g__amf_ue_session_index].gnb_n3_addr,
                                                g__amf_config.smf_sessions[g__amf_ue_session_index].gnb_n3_teid,
                                                debug_flag))
        {
            // Send failure message back to gnodeb
            return send_pdu_setup_failure_msg_to_gnodeb(nmp_n1_n2_rcvd_msg_data_ptr, debug_flag);
        }
         

        // Send All Ok message back to gnodeb for completion of procedure.
        return send_all_ok_msg_to_gnodeb(nmp_n1_n2_rcvd_msg_data_ptr,
                                         debug_flag);
    }
    else
    {
        printf("Unknown message type on N1/N2 interface.. \n");
        return -1;
    }
    return 0;	
}


// AMF listens for incoming messages on N1/N2 interface
// from RAN network.
int
listen_for_n1_n2_messages()
{
    int n = 0;
    int len = 0;
    char string[128];
    uint32_t gnb_addr = 0;
    uint16_t gnb_port = 0;
    struct sockaddr_in  gnb_sockaddr;
    nmp_msg_data_t nmp_n1_n2_rcvd_msg_data;

    while(1)
    {
        ///////////////////////////////////////////////
        // Wait for request messages from gnodeB..
        ///////////////////////////////////////////////
        len = sizeof(struct sockaddr_in);
        memset(&gnb_sockaddr, 0x0, sizeof(struct sockaddr_in));
        n = recvfrom(g__amf_config.my_n1_n2_socket_id,
                    (char *)g__n1_n2_rcvd_msg_buffer,
                     MSG_BUFFER_LEN,
                     MSG_WAITALL,
                    (struct sockaddr *)&(gnb_sockaddr),
                    (socklen_t *)&len);

        gnb_addr = htonl(gnb_sockaddr.sin_addr.s_addr);
        gnb_port = htons(gnb_sockaddr.sin_port);

        if(g__amf_config.debug_switch)
        {
            get_ipv4_addr_string(gnb_addr, string);
            printf("-----------> Rcvd request (%u bytes) from gnodeB (%s:%u) \n",
                    n, string, gnb_port);
        }

        if(-1 == validate_rcvd_nmp_msg_on_n1_n2_interface(g__n1_n2_rcvd_msg_buffer, n))
        {
            printf("%s: Rcvd N1/N2 message validation failed..Ignore and Continue.. \n", __func__);
            continue;
        }

        if(-1 == parse_nmp_msg(g__n1_n2_rcvd_msg_buffer,
                               n,
                               &(nmp_n1_n2_rcvd_msg_data),
                               g__amf_config.debug_switch))
        {
            printf("%s: Rcvd N1/N2 message parse error..Ignore and Continue..  \n", __func__);
            continue;
        }

        if(-1 == process_rcvd_n1_n2_msg(&(nmp_n1_n2_rcvd_msg_data),
                                        gnb_addr,
                                        g__amf_config.debug_switch))
        {
            printf("Unable to process rcvd N1/N2 message \n\n");
        }
    }

    return 0;
}

