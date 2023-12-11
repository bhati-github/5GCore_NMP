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
#include "color_print.h"
#include "common_util.h"

#include "amf.h"
#include "Namf_msg_handler.h"

// json dependency
#include "cJSON.h" 

int
validate_rcvd_msg_on_Namf_interface(uint8_t *msg_ptr,
                                    int      msg_len,
                                    uint32_t request_identifier)
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
send_service_registration_msg_to_nrf(uint8_t  debug_flag)
{
    int n = 0;
    int ret = 0;
    int len = 0;
    int offset = 0;
    uint32_t msg_id = 0;
    uint16_t item_count = 0;
    uint16_t amf_service_info_len = 0;
    uint32_t nrf_addr = 0;
    uint16_t nrf_port = 0;
    uint32_t request_identifier = 0;
    uint8_t amf_service_info[2048];
    char string[128];
    uint8_t *ptr =  g__Namf_send_msg_buffer;
    struct sockaddr_in  nrf_sockaddr;
    struct sockaddr_in  target_service_sockaddr;
    nmp_msg_data_t nmp_Namf_send_msg_data;
    nmp_msg_data_t nmp_Namf_rcvd_msg_data;
    
    cJSON *member;
    cJSON *members;

    ///////////////////////////////////////////////////////////////////////////
    // Step 1: Send NRF_SERVICE_REGISTRATION_REQ message to NRF
    ///////////////////////////////////////////////////////////////////////////
    offset = 0;
    item_count = 0;
    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;

    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__NRF);
    nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__amf_config.nrf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__NRF_SERVICE_REGISTRATION_REQ);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__amf_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    // Save request identifier
    request_identifier = htonl(nmp_hdr_ptr->msg_identifier);

    offset = sizeof(nmp_hdr_t);

    ///////////////////////////////////////////////////////////////////////////
    // Add AMF service info data (json data)
    // NRF will extract this json data and register that info in its database.
    ///////////////////////////////////////////////////////////////////////////

    /*

    {
        "nfInstanceId": "AMF",
        "nfType":       {
                "NFType":       "AMF"
        },
        "nfStatus":     {
                "NFStatus":     "REGISTERED"
        },
        "IPv4Addresses":        ["50.50.50.1:1208"],
        "AllowedNFTypes":       ["AMF", "SMF", "UDM"],
        "fqdn": "amf.3gpp.org",
        "interPlmnFqdn":        "amf.3gpp.org",
        "priority":     5,
        "Capacity":     10,
        "Load": 8,
        "Locality":     "Home PLMN"
    }

    */

    cJSON *amf_json_info_for_nrf = cJSON_CreateObject();

    cJSON_AddStringToObject(amf_json_info_for_nrf, "nfInstanceId", "AMF");
    member = cJSON_CreateObject();
    cJSON_AddItemToObject(member, "NFType", cJSON_CreateString("AMF"));
    cJSON_AddItemToObject(amf_json_info_for_nrf, "nfType", member);

    member = cJSON_CreateObject();
    cJSON_AddItemToObject(member, "NFStatus", cJSON_CreateString("REGISTERED"));
    cJSON_AddItemToObject(amf_json_info_for_nrf, "nfStatus", member);

    members = cJSON_CreateArray();
    cJSON_AddItemToArray(members, cJSON_CreateString("50.50.50.1:1208"));
    cJSON_AddItemToObject(amf_json_info_for_nrf, "IPv4Addresses", members);   
  
    members = cJSON_CreateArray();
    cJSON_AddItemToArray(members, cJSON_CreateString("AMF"));
    cJSON_AddItemToArray(members, cJSON_CreateString("SMF"));
    cJSON_AddItemToArray(members, cJSON_CreateString("UDM"));
    cJSON_AddItemToObject(amf_json_info_for_nrf, "AllowedNFTypes", members);
 
    cJSON_AddStringToObject(amf_json_info_for_nrf, "fqdn", "amf.3gpp.org");
    cJSON_AddStringToObject(amf_json_info_for_nrf, "interPlmnFqdn", "amf.3gpp.org");

    cJSON_AddNumberToObject(amf_json_info_for_nrf, "priority", 5);
    cJSON_AddNumberToObject(amf_json_info_for_nrf, "Capacity", 10);
    cJSON_AddNumberToObject(amf_json_info_for_nrf, "Load", 8);
    cJSON_AddStringToObject(amf_json_info_for_nrf, "Locality", "Home PLMN");

    char *json_string = cJSON_Print(amf_json_info_for_nrf);
    
    if(debug_flag)
    {
        printf("AMF service information to be sent towards NRF = \n%s\n", json_string);
    }

    memset((char *)amf_service_info, 0x0, 2048);
    strcpy((char *)amf_service_info, json_string);
    
    amf_service_info_len = strlen(json_string);
    ret = nmp_add_item__service_info_as_json_data(ptr + offset, 
                                                  amf_service_info, 
                                                  amf_service_info_len);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    cJSON_free(json_string);
    cJSON_Delete(amf_json_info_for_nrf);

    // All items are added. Update NMP message header.
    nmp_hdr_ptr->msg_item_len   = htons(offset - sizeof(nmp_hdr_t));
    nmp_hdr_ptr->msg_item_count = htons(item_count);

    if(-1 == parse_nmp_msg(g__Namf_send_msg_buffer,
                           offset,
                           &(nmp_Namf_send_msg_data),
                           debug_flag))
    {
        printf("%s: Send message parsing error.. \n", __func__);
        return -1;
    }

    // Send this message to NRF
    target_service_sockaddr.sin_addr.s_addr = g__amf_config.Nnrf_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__amf_config.Nnrf_sockaddr.sin_port;
    n = sendto(g__amf_config.my_Namf_socket_id,
               (char *)g__Namf_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));

    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to NRF \n", __func__);
        return -1;
    }
    if(debug_flag)
    {
        MAGENTA_PRINT("Service Registration Message sent to NRF ! \n");
        YELLOW_PRINT("Waiting for response from NRF............... \n");
        printf("\n");
    }


    ///////////////////////////////////////////////////////////////////////////
    // Step 2: Wait for reponse from NRF. 
    //         We must receive Service Registration Response message from NRF
    //         MSG_TYPE: MSG_TYPE__NRF_SERVICE_REGISTRATION_RESP
    ///////////////////////////////////////////////////////////////////////////
    len = sizeof(struct sockaddr_in);
    memset(&nrf_sockaddr, 0x0, sizeof(struct sockaddr_in));
    n = recvfrom(g__amf_config.my_Namf_socket_id,
                 (char *)g__Namf_rcvd_msg_buffer,
                 MSG_BUFFER_LEN,
                 MSG_WAITALL,
                 (struct sockaddr *)&(nrf_sockaddr),
                 (socklen_t *)&len);

    if(debug_flag)
    {
        nrf_addr = htonl(nrf_sockaddr.sin_addr.s_addr);
        nrf_port = htons(nrf_sockaddr.sin_port);

        get_ipv4_addr_string(nrf_addr, string);
        printf("<----------- Rcvd response (%u bytes) from NRF (%s:%u) \n",
                n, string, nrf_port);
    }

    if(-1 == validate_rcvd_msg_on_Namf_interface(g__Namf_rcvd_msg_buffer,
                                                 n,
                                                 request_identifier))
    {
        printf("%s: Rcvd message validation error.. \n", __func__);
        return -1;
    }

    if(-1 == parse_nmp_msg(g__Namf_rcvd_msg_buffer,
                           n,
                           &(nmp_Namf_rcvd_msg_data),
                           debug_flag))
    {
        printf("%s: Rcvd message parsing error.. \n", __func__);
        return -1;
    }

    GREEN_PRINT("------------------------------------------------\n");
    GREEN_PRINT("Service Registration Procedure is [OK] with NRF \n");
    GREEN_PRINT("------------------------------------------------\n");
    printf("\n\n");
    return 0;
}

