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

uint32_t g__amf_ue_session_index = 1;


 
int
validate_rcvd_nmp_msg_on_Namf_interface(uint8_t *msg_ptr,
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

    if(-1 == validate_rcvd_nmp_msg_on_Namf_interface(g__Namf_rcvd_msg_buffer,
                                                     n))
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





int
session_create_req_json_data_maker(uint8_t      *ptr,
                                   data_64bit_t  imsi,
                                   uint8_t      *session_create_json_info,
                                   uint16_t     *session_create_json_info_len,
                                   uint8_t       debug_flag)
{
    char imsi_string[128];
    char final_imsi_string[128];
    cJSON *member;
    cJSON *plmn_member;
    cJSON *amf_json_info_for_smf = cJSON_CreateObject();

    //////////////////////////////////////////////
    memset(final_imsi_string, 0x0, 128);
    sprintf(final_imsi_string, "imsi-");

    memset(imsi_string, 0x0, 128);
    get_imsi_string(imsi.u8, imsi_string);

    strcat(final_imsi_string, imsi_string);
    cJSON_AddStringToObject(amf_json_info_for_smf, "supi", final_imsi_string);
    //////////////////////////////////////////////
    
    //////////////////////////////////////////////
    cJSON_AddNumberToObject(amf_json_info_for_smf, "pduSessionId", g__amf_ue_session_index);
    //////////////////////////////////////////////
    
    //////////////////////////////////////////////
    cJSON_AddStringToObject(amf_json_info_for_smf, "dnn", "default");
    //////////////////////////////////////////////
    
    //////////////////////////////////////////////
    cJSON_AddStringToObject(amf_json_info_for_smf, "servingNfId", "AMF");
    //////////////////////////////////////////////

    //////////////////////////////////////////////
    cJSON *nssai_member = cJSON_CreateObject();
    cJSON_AddNumberToObject(nssai_member, "sst", 2);     
    cJSON_AddNumberToObject(nssai_member, "sd",  2);
    cJSON_AddItemToObject(amf_json_info_for_smf, "sNssai", nssai_member);
    //////////////////////////////////////////////
     
    //////////////////////////////////////////////
    cJSON *guami_member = cJSON_CreateObject();
    plmn_member = cJSON_CreateObject();
    cJSON_AddNumberToObject(plmn_member, "mcc", 404);
    cJSON_AddNumberToObject(plmn_member, "mnc",  10);
    cJSON_AddItemToObject(guami_member, "plmnId", plmn_member);
    cJSON_AddNumberToObject(guami_member, "amfId", g__amf_config.my_id);
    cJSON_AddItemToObject(amf_json_info_for_smf, "guami", guami_member);
    //////////////////////////////////////////////
   
    //////////////////////////////////////////////
    member = cJSON_CreateObject();
    cJSON_AddStringToObject(member, "ServiceName", "Namf_amf");
    cJSON_AddItemToObject(amf_json_info_for_smf, "serviceName", member);
    //////////////////////////////////////////////

    //////////////////////////////////////////////
    member = cJSON_CreateObject();
    cJSON_AddNumberToObject(member, "mcc", 404);
    cJSON_AddNumberToObject(member, "mnc",  10);    
    cJSON_AddItemToObject(amf_json_info_for_smf, "servingNetwork", member);
    //////////////////////////////////////////////

    //////////////////////////////////////////////
    member = cJSON_CreateObject();
    cJSON_AddStringToObject(member, "RequestType", "INITIAL_REQUEST");
    cJSON_AddItemToObject(amf_json_info_for_smf, "requestType", member);
    //////////////////////////////////////////////

    //////////////////////////////////////////////
    member = cJSON_CreateObject();
    cJSON_AddStringToObject(member, "contentId", "n1SmMsg==");
    cJSON_AddItemToObject(amf_json_info_for_smf, "n1SmMsg", member); 
    //////////////////////////////////////////////

    //////////////////////////////////////////////
    member = cJSON_CreateObject();
    cJSON_AddStringToObject(member, "AccessType", "3GPP_ACCESS");
    cJSON_AddItemToObject(amf_json_info_for_smf, "anType", member); 
    //////////////////////////////////////////////
    
    //////////////////////////////////////////////
    member = cJSON_CreateObject();
    cJSON_AddStringToObject(member, "RatType", "NR");
    cJSON_AddItemToObject(amf_json_info_for_smf, "ratType", member);
    //////////////////////////////////////////////

    //////////////////////////////////////////////
    cJSON *tai_member = cJSON_CreateObject();
    plmn_member = cJSON_CreateObject();
    cJSON_AddNumberToObject(plmn_member, "mcc", 404);
    cJSON_AddNumberToObject(plmn_member, "mnc",  10);
    cJSON_AddItemToObject(tai_member, "plmnId", plmn_member);
    cJSON_AddNumberToObject(tai_member, "tac", 100);

    cJSON *ncgi_member = cJSON_CreateObject();
    plmn_member = cJSON_CreateObject();
    cJSON_AddNumberToObject(plmn_member, "mcc", 404);
    cJSON_AddNumberToObject(plmn_member, "mnc",  10);
    cJSON_AddItemToObject(ncgi_member, "plmnId", plmn_member);
    cJSON_AddNumberToObject(ncgi_member, "nrCellId", 100);

    cJSON *globalgnbid_member = cJSON_CreateObject();
    plmn_member = cJSON_CreateObject();
    cJSON_AddNumberToObject(plmn_member, "mcc", 404);
    cJSON_AddNumberToObject(plmn_member, "mnc",  10);
    cJSON_AddItemToObject(globalgnbid_member, "plmnId", plmn_member);

    cJSON *gnbid_member = cJSON_CreateObject();
    cJSON_AddNumberToObject(gnbid_member, "bitLength",  16);
    cJSON_AddNumberToObject(gnbid_member, "gNBValue", 100);
    cJSON_AddItemToObject(globalgnbid_member, "gNbId", gnbid_member);

    cJSON *nrlocation_member = cJSON_CreateObject();
    cJSON_AddItemToObject(nrlocation_member, "tai", tai_member);
    cJSON_AddItemToObject(nrlocation_member, "ncgi", ncgi_member);
    cJSON_AddItemToObject(nrlocation_member, "globalGnbId", globalgnbid_member);

    cJSON *uelocation_member = cJSON_CreateObject();
    cJSON_AddItemToObject(uelocation_member, "ueLocation", nrlocation_member); 

    cJSON_AddItemToObject(amf_json_info_for_smf, "ueLocation", uelocation_member); 
    //////////////////////////////////////////////


    //////////////////////////////////////////////
    cJSON *hostate_member = cJSON_CreateObject();
    cJSON_AddStringToObject(hostate_member, "HoState", "NONE");
    cJSON_AddItemToObject(amf_json_info_for_smf, "hoState", hostate_member);
    //////////////////////////////////////////////

    //////////////////////////////////////////////
    cJSON *selmode_member = cJSON_CreateObject();
    cJSON_AddStringToObject(selmode_member, "DnnSelectionMode", "NONE");
    cJSON_AddItemToObject(amf_json_info_for_smf, "selMode", selmode_member);
    //////////////////////////////////////////////

    //////////////////////////////////////////////
    cJSON *epsinterworkingind_member = cJSON_CreateObject();
    cJSON_AddStringToObject(epsinterworkingind_member, "EpsInterworkingIndication", "NONE");
    cJSON_AddItemToObject(amf_json_info_for_smf, "epsInterworkingInd", epsinterworkingind_member);
    //////////////////////////////////////////////


    char *json_string = cJSON_Print(amf_json_info_for_smf);

    if(debug_flag)
    {
        printf("AMF service information to be sent towards NRF = \n%s\n", json_string);
    }

    strcpy((char *)session_create_json_info, json_string);
    *session_create_json_info_len = strlen(json_string);

    cJSON_free(json_string);
    cJSON_Delete(amf_json_info_for_smf);

    return 0;
}



int
send_session_create_msg_to_smf(data_64bit_t imsi,
                               uint8_t      debug_flag)
{
    int n = 0;
    int ret = 0;
    int len = 0;
    int offset = 0;
    uint32_t msg_id = 0;
    uint16_t item_count = 0;
    uint32_t smf_addr = 0;
    uint16_t smf_port = 0;
    char string[128];
    uint8_t *ptr =  g__Namf_send_msg_buffer;
    struct sockaddr_in  smf_sockaddr;
    struct sockaddr_in  target_service_sockaddr;
    nmp_msg_data_t nmp_Namf_send_msg_data;
    nmp_msg_data_t nmp_Namf_rcvd_msg_data;
    uint8_t session_create_json_info[2048];
    memset(session_create_json_info, 0x0, 2048);
    uint16_t session_create_json_info_len;

    offset = 0;
    item_count = 0;
    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;

    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__SMF);
    nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__amf_config.smf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__SMF_SESSION_CREATE_REQ);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__amf_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    offset = sizeof(nmp_hdr_t);

    if(-1 == session_create_req_json_data_maker(ptr + offset,
                                                imsi,
                                                session_create_json_info,
                                               &session_create_json_info_len,
                                                debug_flag))
    {
        printf("%s: Unable to prepare json data towards SMF \n", __func__);
        return -1;
    }

    /////////////////////////////////////////////
    // Add session create json data as NMP item
    /////////////////////////////////////////////
    ret = nmp_add_item__session_create_req_info_as_json_data(ptr + offset,
                                                             session_create_json_info,
                                                             session_create_json_info_len);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    // Add user IMSI
    ret = nmp_add_item__imsi(ptr+ offset, imsi);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;


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

    // Send this message to SMF
    target_service_sockaddr.sin_addr.s_addr = g__amf_config.Nsmf_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__amf_config.Nsmf_sockaddr.sin_port;
    n = sendto(g__amf_config.my_Namf_socket_id,
               (char *)g__Namf_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));

    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to SMF \n", __func__);
        return -1;
    }
    if(debug_flag)
    {
        MAGENTA_PRINT("Session Create Message sent to SMF ! \n");
        YELLOW_PRINT("Waiting for response from SMF............... \n");
        printf("\n");
    }


    ///////////////////////////////////////////////////////////////////////////
    // Step 2: Wait for reponse from SMF.
    //         We must receive Session Create Response from SMF
    //         MSG_TYPE: MSG_TYPE__SMF_SESSION_CREATE_RESP
    ///////////////////////////////////////////////////////////////////////////
    len = sizeof(struct sockaddr_in);
    memset(&smf_sockaddr, 0x0, sizeof(struct sockaddr_in));
    n = recvfrom(g__amf_config.my_Namf_socket_id,
                 (char *)g__Namf_rcvd_msg_buffer,
                 MSG_BUFFER_LEN,
                 MSG_WAITALL,
                 (struct sockaddr *)&(smf_sockaddr),
                 (socklen_t *)&len);

    if(debug_flag)
    {
        smf_addr = htonl(smf_sockaddr.sin_addr.s_addr);
        smf_port = htons(smf_sockaddr.sin_port);

        get_ipv4_addr_string(smf_addr, string);
        printf("<----------- Rcvd response (%u bytes) from SMF (%s:%u) \n",
                n, string, smf_port);
    }

    if(-1 == validate_rcvd_nmp_msg_on_Namf_interface(g__Namf_rcvd_msg_buffer,
                                                     n))
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

    // We must have received gtp-u teid endpoint of UPF N3 interface(for uplink packets).
    // Store into amf user session database..
    // This information is also required to be sent to gnodeB as one of the NMP item
    g__amf_config.smf_sessions[g__amf_ue_session_index].upf_n3_addr = nmp_Namf_rcvd_msg_data.upf_n3_addr; 
    g__amf_config.smf_sessions[g__amf_ue_session_index].upf_n3_teid = nmp_Namf_rcvd_msg_data.upf_n3_teid;
    
    if(debug_flag)
    {
        printf("%s: UE Session Create Procedure is completed with SMF \n", __func__);
    } 

    return 0;
}


int
send_session_modify_msg_to_smf(data_64bit_t  imsi,
                               uint32_t   gnb_n3_iface_v4_addr,
                               uint32_t   dnlink_teid,
                               uint8_t    debug_flag)
{
    int n = 0;
    int ret = 0;
    int len = 0;
    int offset = 0;
    uint32_t msg_id = 0;
    uint16_t item_count = 0;
    uint32_t smf_addr = 0;
    uint16_t smf_port = 0;
    char string[128];
    uint8_t *ptr =  g__Namf_send_msg_buffer;
    struct sockaddr_in  smf_sockaddr;
    struct sockaddr_in  target_service_sockaddr;
    nmp_msg_data_t nmp_Namf_send_msg_data;
    nmp_msg_data_t nmp_Namf_rcvd_msg_data;

    offset = 0;
    item_count = 0;
    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;

    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__AMF);
    nmp_hdr_ptr->dst_node_type  = htons(NODE_TYPE__SMF);
    nmp_hdr_ptr->src_node_id    = htons(g__amf_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(g__amf_config.smf_id);

    nmp_hdr_ptr->msg_type       = htons(MSG_TYPE__SMF_SESSION_MODIFY_REQ);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    msg_id = g__amf_config.my_id << 16;
    msg_id |= (uint16_t )rand();
    nmp_hdr_ptr->msg_identifier = htonl(msg_id);

    offset = sizeof(nmp_hdr_t);


    // gnodeB sent its own N3 interface TEID endpoint (used for sending downlink packets by UPF)
    // Send this teid information to SMF so that SMF can modify the session in UPF.
    ret = nmp_add_item__dnlink_gtpu_ipv4_endpoint(ptr + offset, 
                                                  gnb_n3_iface_v4_addr, 
                                                  dnlink_teid);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;


    // Add user IMSI
    ret = nmp_add_item__imsi(ptr+ offset, imsi);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;


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

    // Send this message to SMF
    target_service_sockaddr.sin_addr.s_addr = g__amf_config.Nsmf_sockaddr.sin_addr.s_addr;
    target_service_sockaddr.sin_port = g__amf_config.Nsmf_sockaddr.sin_port;
    n = sendto(g__amf_config.my_Namf_socket_id,
               (char *)g__Namf_send_msg_buffer,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));

    if(n != offset)
    {
        printf("%s: sendto() failed during msg send to SMF \n", __func__);
        return -1;
    }
    if(debug_flag)
    {
        MAGENTA_PRINT("Session Modify Message sent to SMF ! \n");
        YELLOW_PRINT("Waiting for response from SMF............... \n");
        printf("\n");
    }

    ///////////////////////////////////////////////////////////////////////////
    // Step 2: Wait for reponse from SMF.
    //         We must receive Session Modify Response from SMF
    //         MSG_TYPE: MSG_TYPE__SMF_SESSION_MODIFY_RESP
    ///////////////////////////////////////////////////////////////////////////
    len = sizeof(struct sockaddr_in);
    memset(&smf_sockaddr, 0x0, sizeof(struct sockaddr_in));
    n = recvfrom(g__amf_config.my_Namf_socket_id,
                 (char *)g__Namf_rcvd_msg_buffer,
                 MSG_BUFFER_LEN,
                 MSG_WAITALL,
                 (struct sockaddr *)&(smf_sockaddr),
                 (socklen_t *)&len);

    if(debug_flag)
    {
        smf_addr = htonl(smf_sockaddr.sin_addr.s_addr);
        smf_port = htons(smf_sockaddr.sin_port);

        get_ipv4_addr_string(smf_addr, string);
        printf("<----------- Rcvd response (%u bytes) from SMF (%s:%u) \n",
                n, string, smf_port);
    }

    if(-1 == validate_rcvd_nmp_msg_on_Namf_interface(g__Namf_rcvd_msg_buffer,
                                                     n))
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

    if(debug_flag)
    {
        printf("%s: UE Session Modify Procedure is completed with SMF \n", __func__);
    }

    // Now, we can increment UE session index inside AMF.
    g__amf_ue_session_index += 1; 
    return 0;
}


