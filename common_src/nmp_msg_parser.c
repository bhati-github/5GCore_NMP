/******************************************************************************
 * MIT License
 *
 * Copyright (c) [2021] [Abhishek Bhati]
 *
 * Email    : ab.bhati@gmail.com
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

// json dependency
#include "cJSON.h"

uint8_t g__byte_debug_flag = 0;

void
get_node_type_string(uint16_t node_type, 
                     char    *string)
{
    switch (node_type)
    {
        case NODE_TYPE__AMF:
            strcpy(string, "AMF");
            break;

        case NODE_TYPE__GNB:
            strcpy(string, "GNB");
            break;
        
        case NODE_TYPE__NRF:
            strcpy(string, "NRF");
            break;
        
        case NODE_TYPE__SMF:
            strcpy(string, "SMF");
            break;

        case NODE_TYPE__UE:
            strcpy(string, "UE");
            break;
        
        case NODE_TYPE__UPF:
            strcpy(string, "UPF");
            break;

        default:
            strcpy(string, "Unknown");
            break;
    }
}

static void
get_far_dst_interface_string(uint8_t  dst_interface, 
                             char    *string)
{
    if(0 == dst_interface)
    {
        strcpy(string, "N3");
    }
    else if(1 == dst_interface)
    {
        strcpy(string, "N6");
    }
    else if(2 == dst_interface)
    {
        strcpy(string, "N9");
    }
    else
    {
        strcpy(string, "Unknown Interface");
    }
}

static void
get_default_paging_drx_string(uint8_t  default_paging_drx,
                              char    *string)
{
    if(0 == default_paging_drx)
    {
        strcpy(string, "v32");
    }
    else if(1 == default_paging_drx)
    {
        strcpy(string, "v64");
    }
    else if(2 == default_paging_drx)
    {
        strcpy(string, "v128");
    }
    else if(3 == default_paging_drx)
    {
        strcpy(string, "v256");
    }
    else
    {
        strcpy(string, "Unknown Paging DRX");
    }
}


static void
dump_src_to_dst_node_data(uint16_t src_node_type,
                          uint16_t dst_node_type,
                          uint16_t src_node_id,
                          uint16_t dst_node_id)
{
    char src_string[128];
    char dst_string[128];

    get_node_type_string(src_node_type, src_string);
    get_node_type_string(dst_node_type, dst_string);

    printf("%s (%u) --------> %s (%u) \n", src_string, src_node_id, dst_string, dst_node_id);
}


static void
dump_msg_type(uint16_t msg_type)
{
    switch (msg_type)
    {
        case MSG_TYPE__KEEPALIVE_REQ:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "KEEPALIVE_REQ"); printf("\n");
            break;

        case MSG_TYPE__KEEPALIVE_RESP:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "KEEPALIVE_RESP"); printf("\n");
            break;

        case MSG_TYPE__ALL_OK:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "ALL_OK"); printf("\n");
            break;
        
        case MSG_TYPE__NOT_OK:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "NOT_OK"); printf("\n");
            break;

        case MSG_TYPE__NG_SETUP_REQ:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "NG_SETUP_REQ"); printf("\n");
            break;

        case MSG_TYPE__NG_SETUP_RESP:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "NG_SETUP_RESP"); printf("\n");
            break;

        case MSG_TYPE__INITIAL_UE_MSG_REGISTRATION_REQ:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "INITIAL_UE_MSG_REGISTRATION_REQ"); printf("\n");
            break;

        case MSG_TYPE__DNLINK_NAS_TRANSPORT_AUTH_REQ:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "DNLINK_NAS_TRANSPORT_AUTH_REQ"); printf("\n");
            break;
        
        case MSG_TYPE__UPLINK_NAS_TRANSPORT_AUTH_RESP:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "UPLINK_NAS_TRANSPORT_AUTH_RESP"); printf("\n");
            break;

        case MSG_TYPE__DNLINK_NAS_TRANSPORT_REGISTRATION_ACCEPT:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "DNLINK_NAS_TRANSPORT_REGISTRATION_ACCEPT"); printf("\n");
            break;

        case MSG_TYPE__UPLINK_NAS_TRANSPORT_REGISTRATION_COMPLETE:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "UPLINK_NAS_TRANSPORT_REGISTRATION_COMPLETE"); printf("\n");
            break;

        case MSG_TYPE__UPLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REQ:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "UPLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REQ"); printf("\n");
            break;

        case MSG_TYPE__DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_ACCEPT:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_ACCEPT"); printf("\n");
            break;
        
        case MSG_TYPE__DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REJECT:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "DNLINK_NAS_TRANSPORT_PDU_SESSION_ESTABLISH_REJECT"); printf("\n");
            break;

        case MSG_TYPE__PDU_SESSION_RESOURCE_SETUP_RESP:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "PDU_SESSION_RESOURCE_SETUP_RESP"); printf("\n");
            break;

        case MSG_TYPE__NRF_SERVICE_REGISTRATION_REQ:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "NRF_SERVICE_REGISTRATION_REQ"); printf("\n");
            break;

        case MSG_TYPE__NRF_SERVICE_REGISTRATION_RESP:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "NRF_SERVICE_REGISTRATION_RESP"); printf("\n");
            break;

        case MSG_TYPE__NRF_SERVICE_DISCOVERY_REQ:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "NRF_SERVICE_DISCOVERY_REQ"); printf("\n");
            break;

        case MSG_TYPE__NRF_SERVICE_DISCOVERY_RESP:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "NRF_SERVICE_DISCOVERY_RESP"); printf("\n");
            break;

        case MSG_TYPE__SMF_SESSION_CREATE_REQ:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "SMF_SESSION_CREATE_REQ"); printf("\n");
            break;
        
        case MSG_TYPE__SMF_SESSION_CREATE_RESP:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "SMF_SESSION_CREATE_RESP"); printf("\n");
            break;

        case MSG_TYPE__SMF_SESSION_MODIFY_REQ:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "SMF_SESSION_MODIFY_REQ"); printf("\n");
            break;

        case MSG_TYPE__SMF_SESSION_MODIFY_RESP:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "SMF_SESSION_MODIFY_RESP"); printf("\n");
            break;

        case MSG_TYPE__UPF_SESSION_CREATE_REQ:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "UPF_SESSION_CREATE_REQ"); printf("\n");
            break;

        case MSG_TYPE__UPF_SESSION_CREATE_RESP:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "UPF_SESSION_CREATE_RESP"); printf("\n");
            break;

        case MSG_TYPE__UPF_SESSION_MODIFY_REQ:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "UPF_SESSION_MODIFY_REQ"); printf("\n");
            break;

        case MSG_TYPE__UPF_SESSION_MODIFY_RESP:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "UPF_SESSION_MODIFY_RESP"); printf("\n");
            break;

        case MSG_TYPE__UPF_SESSION_DELETE_REQ:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "UPF_SESSION_DELETE_REQ"); printf("\n");
            break;

        case MSG_TYPE__UPF_SESSION_DELETE_RESP:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "UPF_SESSION_DELETE_RESP"); printf("\n");
            break;
       
        default:
            printf("%-16s : ", "Msg Type"); RED_PRINT("%s", "Unknown"); printf("\n");
            break;
    }
}


static void
dump_item_id(char *space, uint16_t item_id)
{
    switch (item_id)
    {
        ////////////////////////////////////////////////////////////////////
        // Item id's carrying 1 byte value
        ////////////////////////////////////////////////////////////////////
        case ITEM_ID__UPLINK_QOS_PROFILE:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "UPLINK_QOS_PROFILE", ITEM_ID__UPLINK_QOS_PROFILE, ITEM_ID__UPLINK_QOS_PROFILE);
            break;

        case ITEM_ID__DNLINK_QOS_PROFILE:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "DNLINK_QOS_PROFILE", ITEM_ID__DNLINK_QOS_PROFILE, ITEM_ID__DNLINK_QOS_PROFILE);
            break;

        case ITEM_ID__PDR_ACTION:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "PDR_ACTION", ITEM_ID__PDR_ACTION, ITEM_ID__PDR_ACTION);
            break;

        case ITEM_ID__FAR_ACTION_FLAGS:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "FAR_ACTION_FLAGS", ITEM_ID__FAR_ACTION_FLAGS, ITEM_ID__FAR_ACTION_FLAGS);
            break;

        case ITEM_ID__FAR_DST_INTERFACE:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "FAR_DST_INTERFACE", ITEM_ID__FAR_DST_INTERFACE, ITEM_ID__FAR_DST_INTERFACE);
            break;
        
        case ITEM_ID__DEFAULT_PAGING_DRX:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "DEFAULT_PAGING_DRX", ITEM_ID__DEFAULT_PAGING_DRX, ITEM_ID__DEFAULT_PAGING_DRX);
            break;
        
        case ITEM_ID__RELATIVE_AMF_CAPACITY:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "RELATIVE_AMF_CAPACITY", ITEM_ID__RELATIVE_AMF_CAPACITY, ITEM_ID__RELATIVE_AMF_CAPACITY);
            break;


        ////////////////////////////////////////////////////////////////////
        // Item id's carrying 2 byte value
        ////////////////////////////////////////////////////////////////////
        case ITEM_ID__RAN_UE_NGAP_ID:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "RAN_UE_NGAP_ID", ITEM_ID__RAN_UE_NGAP_ID, ITEM_ID__RAN_UE_NGAP_ID);
            break;
        
        case ITEM_ID__AMF_UE_NGAP_ID:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "AMF_UE_NGAP_ID", ITEM_ID__AMF_UE_NGAP_ID, ITEM_ID__AMF_UE_NGAP_ID);
            break;

        case ITEM_ID__PDR_RULE_ID:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "PDR_RULE_ID", ITEM_ID__PDR_RULE_ID, ITEM_ID__PDR_RULE_ID);
            break;

        case ITEM_ID__PDR_PRECEDENCE:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "PDR_PRECEDENCE", ITEM_ID__PDR_PRECEDENCE, ITEM_ID__PDR_PRECEDENCE);
            break;

        case ITEM_ID__PDR_FAR_ID:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "PDR_FAR_ID", ITEM_ID__PDR_FAR_ID, ITEM_ID__PDR_FAR_ID);
            break;

        case ITEM_ID__FAR_RULE_ID:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "FAR_RULE_ID", ITEM_ID__FAR_RULE_ID, ITEM_ID__FAR_RULE_ID);
            break;

        case ITEM_ID__RRC_ESTABLISH_CAUSE:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "RRC_ESTABLISH_CAUSE", ITEM_ID__RRC_ESTABLISH_CAUSE, ITEM_ID__RRC_ESTABLISH_CAUSE);
            break;
        
        case ITEM_ID__MSG_RESPONSE_CODE:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "MSG_RESPONSE_CODE", ITEM_ID__MSG_RESPONSE_CODE, ITEM_ID__MSG_RESPONSE_CODE);
            break;


        ////////////////////////////////////////////////////////////////////
        // Item id's carrying more than 2 and upto 4 byte value
        ////////////////////////////////////////////////////////////////////
        case ITEM_ID__MCC_MNC:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "MCC_MNC", ITEM_ID__MCC_MNC, ITEM_ID__MCC_MNC);
            break;
        
        case ITEM_ID__GNODEB_ID:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "GNODEB_ID", ITEM_ID__GNODEB_ID, ITEM_ID__GNODEB_ID);
            break;

        case ITEM_ID__TAC:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "TAC (Tracking Area Code)", ITEM_ID__TAC, ITEM_ID__TAC);
            break;

        case ITEM_ID__CELL_ID:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "Cell ID", ITEM_ID__CELL_ID, ITEM_ID__CELL_ID);
            break;

        case ITEM_ID__TMSI:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "Mobile-TMSI", ITEM_ID__TMSI, ITEM_ID__TMSI);
            break;

        case ITEM_ID__UE_IPV4_ADDR:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "UE_IPV4_ADDR", ITEM_ID__UE_IPV4_ADDR, ITEM_ID__UE_IPV4_ADDR);
            break;

        case ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "PDR_PDI_MATCH_IPV4_ADDR", ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR, ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR);
            break;

        case ITEM_ID__PDR_PDI_MATCH_GTPU_TEID:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID",
                   "PDR_PDI_MATCH_GTPU_TEID", ITEM_ID__PDR_PDI_MATCH_GTPU_TEID, ITEM_ID__PDR_PDI_MATCH_GTPU_TEID);
            break;
        

        ////////////////////////////////////////////////////////////////////
        // Item id's carrying more than 4 and upto 8 byte value
        ////////////////////////////////////////////////////////////////////
        case ITEM_ID__IMSI:
            printf("%s%-16s : %s (%u) (0x%08x) \n", space, "Type-1 Item ID", 
                   "IMSI", ITEM_ID__IMSI, ITEM_ID__IMSI);
            break;
        
        case ITEM_ID__NSSAI:
            printf("%s%-16s : %s (%u) (0x%08x) \n", space, "Type-1 Item ID", 
                   "NSSAI", ITEM_ID__NSSAI, ITEM_ID__NSSAI);
            break;

        case ITEM_ID__GTPU_SELF_IPV4_ENDPOINT:
            printf("%s%-16s : %s (%u) (0x%08x) \n", space, "Type-1 Item ID", 
                   "GTPU_SELF_IPV4_ENDPOINT", ITEM_ID__GTPU_SELF_IPV4_ENDPOINT, ITEM_ID__GTPU_SELF_IPV4_ENDPOINT);
            break;

        case ITEM_ID__GTPU_PEER_IPV4_ENDPOINT:
            printf("%s%-16s : %s (%u) (0x%08x) \n", space, "Type-1 Item ID", 
                   "GTPU_PEER_IPV4_ENDPOINT", ITEM_ID__GTPU_PEER_IPV4_ENDPOINT, ITEM_ID__GTPU_PEER_IPV4_ENDPOINT);
            break;

        case ITEM_ID__UPLINK_GTPU_IPV4_ENDPOINT:
            printf("%s%-16s : %s (%u) (0x%08x) \n", space, "Type-1 Item ID",
                   "UPLINK_GTPU_IPV4_ENDPOINT", ITEM_ID__UPLINK_GTPU_IPV4_ENDPOINT, ITEM_ID__UPLINK_GTPU_IPV4_ENDPOINT);
            break;

        case ITEM_ID__DNLINK_GTPU_IPV4_ENDPOINT:
            printf("%s%-16s : %s (%u) (0x%08x) \n", space, "Type-1 Item ID",
                   "DNLINK_GTPU_IPV4_ENDPOINT", ITEM_ID__DNLINK_GTPU_IPV4_ENDPOINT, ITEM_ID__DNLINK_GTPU_IPV4_ENDPOINT);
            break;

        case ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE:
            printf("%s%-16s : %s (%u) (0x%08x) \n", space, "Type-1 Item ID", 
                   "FAR_OUTER_IPV4_HDR_CREATE", ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE, ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE);
            break;

        case ITEM_ID__USER_LOCATION_INFO_TAC:
            printf("%s%-16s : %s (%u) (0x%08x) \n", space, "Type-1 Item ID", 
                   "USER_LOCATION_INFO_TAC", ITEM_ID__USER_LOCATION_INFO_TAC, ITEM_ID__USER_LOCATION_INFO_TAC);
            break;


        ////////////////////////////////////////////////////////////////////
        // Item id's carrying more than 8 and upto 16 byte value 
        ////////////////////////////////////////////////////////////////////
        case ITEM_ID__UE_IDENTIFIER_SECRET:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "UE_IDENTIFIER_SECRET", ITEM_ID__UE_IDENTIFIER_SECRET, ITEM_ID__UE_IDENTIFIER_SECRET);
            break;

        case ITEM_ID__UE_IPV6_ADDR:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "UE_IPV6_ADDR", ITEM_ID__UE_IPV6_ADDR, ITEM_ID__UE_IPV6_ADDR);
            break;

        case ITEM_ID__USER_LOCATION_INFO_NR_CGI:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-1 Item ID", 
                   "USER_LOCATION_INFO_NR_CGI", ITEM_ID__USER_LOCATION_INFO_NR_CGI, ITEM_ID__USER_LOCATION_INFO_NR_CGI);
            break;

        ////////////////////////////////////////////////////////////////////
        // Item id's carrying more than 16 byte value (variable length)
        ////////////////////////////////////////////////////////////////////
        case ITEM_ID__MSG_RESPONSE_DESCRIPTION:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID",
                   "ITEM_ID__MSG_RESPONSE_DESCRIPTION", ITEM_ID__MSG_RESPONSE_DESCRIPTION, ITEM_ID__MSG_RESPONSE_DESCRIPTION);
            break;

        case ITEM_ID__GTPU_SELF_IPV6_ENDPOINT:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID", 
                   "GTPU_SELF_IPV6_ENDPOINT", ITEM_ID__GTPU_SELF_IPV6_ENDPOINT, ITEM_ID__GTPU_SELF_IPV6_ENDPOINT);
            break;

        case ITEM_ID__GTPU_PEER_IPV6_ENDPOINT:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID", 
                   "GTPU_PEER_IPV6_ENDPOINT", ITEM_ID__GTPU_PEER_IPV6_ENDPOINT, ITEM_ID__GTPU_PEER_IPV6_ENDPOINT);
            break;

        case ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID", 
                   "FAR_OUTER_IPV6_HDR_CREATE", ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE, ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE);
            break;

        case ITEM_ID__NAS_PDU:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID", 
                   "ITEM_ID__NAS_PDU", ITEM_ID__NAS_PDU, ITEM_ID__NAS_PDU);
            break;
        
        case ITEM_ID__RAN_NODE_NAME:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID", 
                   "ITEM_ID__RAN_NODE_NAME", ITEM_ID__RAN_NODE_NAME, ITEM_ID__RAN_NODE_NAME);
            break;
        
        case ITEM_ID__AMF_NAME:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID", 
                   "ITEM_ID__AMF_NAME", ITEM_ID__AMF_NAME, ITEM_ID__AMF_NAME);
            break;
        
        case ITEM_ID__GUAMI:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID", 
                   "ITEM_ID__GUAMI", ITEM_ID__GUAMI, ITEM_ID__GUAMI);
            break;
        
        case ITEM_ID__SERVICE_INFO_JSON_DATA:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID", 
                   "ITEM_ID__SERVICE_INFO_JSON_DATA", ITEM_ID__SERVICE_INFO_JSON_DATA, ITEM_ID__SERVICE_INFO_JSON_DATA);
            break;

        case ITEM_ID__SESSION_CREATE_REQ_JSON_DATA:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID",
                   "ITEM_ID__SESSION_CREATE_REQ_JSON_DATA", ITEM_ID__SESSION_CREATE_REQ_JSON_DATA, ITEM_ID__SESSION_CREATE_REQ_JSON_DATA);
            break;
        
        case ITEM_ID__SESSION_CREATE_RESP_JSON_DATA:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Type-2 Item ID",
                   "ITEM_ID__SESSION_CREATE_RESP_JSON_DATA", ITEM_ID__SESSION_CREATE_RESP_JSON_DATA, ITEM_ID__SESSION_CREATE_RESP_JSON_DATA);
            break;

        
        ////////////////////////////////////////////////////////////////////
        // Item id's carrying group of individual items 
        // Item group contains all possible combinations of type-1 and type-2 items.
        ////////////////////////////////////////////////////////////////////
        case ITEM_GROUP_ID__N3_PDR:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID",
                   "N3_PDR", ITEM_GROUP_ID__N3_PDR, ITEM_GROUP_ID__N3_PDR);
            break;

        case ITEM_GROUP_ID__N6_PDR:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID", 
                   "N6_PDR", ITEM_GROUP_ID__N6_PDR, ITEM_GROUP_ID__N6_PDR);
            break;

        case ITEM_GROUP_ID__N3_FAR:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID", 
                   "N3_FAR", ITEM_GROUP_ID__N3_FAR, ITEM_GROUP_ID__N3_FAR);
            break;

        case ITEM_GROUP_ID__N6_FAR:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID",
                   "N6_FAR", ITEM_GROUP_ID__N6_FAR, ITEM_GROUP_ID__N6_FAR);
            break;

        case ITEM_GROUP_ID__USER_LOCATION_INFO:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID", 
                   "USER_LOCATION_INFO", ITEM_GROUP_ID__USER_LOCATION_INFO, ITEM_GROUP_ID__USER_LOCATION_INFO);
            break;
        
        case ITEM_GROUP_ID__GLOBAL_RAN_NODE_ID:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID", 
                   "GLOBAL_RAN_NODE_ID", ITEM_GROUP_ID__GLOBAL_RAN_NODE_ID, ITEM_GROUP_ID__GLOBAL_RAN_NODE_ID);
            break;
        
        case ITEM_GROUP_ID__GUAMI_LIST:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID",
                   "GUAMI_LIST", ITEM_GROUP_ID__GUAMI_LIST, ITEM_GROUP_ID__GUAMI_LIST);
            break;
        
        case ITEM_GROUP_ID__SUPPORTED_TA_LIST:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID",
                   "SUPPORTED_TA_LIST", ITEM_GROUP_ID__SUPPORTED_TA_LIST, ITEM_GROUP_ID__SUPPORTED_TA_LIST);
            break;
        
        case ITEM_GROUP_ID__SUPPORTED_TA_LIST_ITEM:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID",
                   "SUPPORTED_TA_LIST_ITEM", ITEM_GROUP_ID__SUPPORTED_TA_LIST_ITEM, ITEM_GROUP_ID__SUPPORTED_TA_LIST_ITEM);
            break;
        
        case ITEM_GROUP_ID__PLMN_SUPPORT_LIST:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID",
                   "PLMN_SUPPORT_LIST", ITEM_GROUP_ID__PLMN_SUPPORT_LIST, ITEM_GROUP_ID__PLMN_SUPPORT_LIST);
            break;

        case ITEM_GROUP_ID__PLMN_SUPPORT_LIST_ITEM:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item Group ID",
                   "PLMN_SUPPORT_LIST_ITEM", ITEM_GROUP_ID__PLMN_SUPPORT_LIST_ITEM, ITEM_GROUP_ID__PLMN_SUPPORT_LIST_ITEM);
            break;
        
        ////////////////////////////////////////////////////////////////////

        default:
            printf("%s%-16s : %s (%u) (0x%04x) \n", space, "Item ID", "Unknown", item_id, item_id);
            break;
    }
}

static int
get_item_type(uint16_t item_id,
              uint8_t  *item_type)
{
    switch (item_id)
    {
        ///////////////////////////////
        case ITEM_ID__UPLINK_QOS_PROFILE:
        case ITEM_ID__DNLINK_QOS_PROFILE:
        case ITEM_ID__PDR_ACTION:
        case ITEM_ID__FAR_ACTION_FLAGS:
        case ITEM_ID__FAR_DST_INTERFACE:
        case ITEM_ID__DEFAULT_PAGING_DRX:
        case ITEM_ID__RELATIVE_AMF_CAPACITY:
            *item_type = ITEM_TYPE_IS_TYPE_1; // value is fixed size (1 byte)
            return 0;

        ///////////////////////////////
        case ITEM_ID__RAN_UE_NGAP_ID:
        case ITEM_ID__AMF_UE_NGAP_ID:
        case ITEM_ID__PDR_RULE_ID:
        case ITEM_ID__PDR_PRECEDENCE:
        case ITEM_ID__PDR_FAR_ID:
        case ITEM_ID__FAR_RULE_ID:
        case ITEM_ID__RRC_ESTABLISH_CAUSE:
        case ITEM_ID__MSG_RESPONSE_CODE:
            *item_type = ITEM_TYPE_IS_TYPE_1; // value is fixed size (2 bytes)
            return 0;

        ///////////////////////////////
        case ITEM_ID__MCC_MNC:
        case ITEM_ID__GNODEB_ID:
        case ITEM_ID__TAC:
        case ITEM_ID__CELL_ID:
        case ITEM_ID__TMSI:
        case ITEM_ID__UE_IPV4_ADDR:
        case ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR:
        case ITEM_ID__PDR_PDI_MATCH_GTPU_TEID:
            *item_type = ITEM_TYPE_IS_TYPE_1; // value is fixed size (4 bytes)
            return 0;

        ///////////////////////////////
        case ITEM_ID__IMSI:
        case ITEM_ID__NSSAI:
        case ITEM_ID__GTPU_SELF_IPV4_ENDPOINT:
        case ITEM_ID__GTPU_PEER_IPV4_ENDPOINT:
        case ITEM_ID__UPLINK_GTPU_IPV4_ENDPOINT:
        case ITEM_ID__DNLINK_GTPU_IPV4_ENDPOINT:
        case ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE:
        case ITEM_ID__USER_LOCATION_INFO_TAC:
            *item_type = ITEM_TYPE_IS_TYPE_1; // value is fixed size (8 bytes)
            return 0;

        ///////////////////////////////
        case ITEM_ID__UE_IDENTIFIER_SECRET:
        case ITEM_ID__UE_IPV6_ADDR:
        case ITEM_ID__USER_LOCATION_INFO_NR_CGI:
            *item_type = ITEM_TYPE_IS_TYPE_1;  // value is fixed size (16 bytes)
            return 0;

        ///////////////////////////////
        case ITEM_ID__MSG_RESPONSE_DESCRIPTION:
        case ITEM_ID__GTPU_SELF_IPV6_ENDPOINT:
        case ITEM_ID__GTPU_PEER_IPV6_ENDPOINT:
        case ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE:
        case ITEM_ID__NAS_PDU:
        case ITEM_ID__RAN_NODE_NAME:
        case ITEM_ID__AMF_NAME:
        case ITEM_ID__GUAMI:
        case ITEM_ID__SERVICE_INFO_JSON_DATA:
        case ITEM_ID__SESSION_CREATE_REQ_JSON_DATA:
        case ITEM_ID__SESSION_CREATE_RESP_JSON_DATA:
            *item_type = ITEM_TYPE_IS_TYPE_2; // value is now a bytestream of variable length 
            return 0;

        ///////////////////////////////
        case ITEM_GROUP_ID__N3_PDR:
        case ITEM_GROUP_ID__N6_PDR:
        case ITEM_GROUP_ID__N3_FAR:
        case ITEM_GROUP_ID__N6_FAR:
        case ITEM_GROUP_ID__USER_LOCATION_INFO:
        case ITEM_GROUP_ID__GLOBAL_RAN_NODE_ID:
        case ITEM_GROUP_ID__GUAMI_LIST:
        case ITEM_GROUP_ID__SUPPORTED_TA_LIST:
        case ITEM_GROUP_ID__SUPPORTED_TA_LIST_ITEM:
        case ITEM_GROUP_ID__PLMN_SUPPORT_LIST:
        case ITEM_GROUP_ID__PLMN_SUPPORT_LIST_ITEM:
            *item_type = ITEM_TYPE_IS_TYPE_3; // group of items (fixed-length and variable-length)
            return 0;

        ///////////////////////////////
        default:
            return -1;
    }

    return -1;
}



static int
get_type1_item_value(char           *space,
                     uint16_t        item_id,
                     uint8_t        *ptr,
                     nmp_msg_data_t *nmp_msg_parsed_data_ptr,
                     uint8_t         debug_flag)
{
    int i = 0;
    char string[128];

    // --------------------------------
    // | Fixed length item values     |
    // --------------------------------
    switch (item_id)
    {
        /////////////////////////////////////////////////////////////////////
        // Extract 1 byte items.
        /////////////////////////////////////////////////////////////////////
        case ITEM_ID__UPLINK_QOS_PROFILE:
            nmp_msg_parsed_data_ptr->ul_qos_profile = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));
                printf("%s%-32s :-> %u \n", space, "Uplink QoS Profile", nmp_msg_parsed_data_ptr->ul_qos_profile);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+1 bytes are parsed", ptr - 2, 3);
            return 1;

        case ITEM_ID__DNLINK_QOS_PROFILE:
            nmp_msg_parsed_data_ptr->dl_qos_profile = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));
                printf("%s%-32s :-> %u \n", space, "Dnlink QoS Profile", nmp_msg_parsed_data_ptr->dl_qos_profile);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+1 bytes are parsed", ptr - 2, 3);
            return 1;

        case ITEM_ID__PDR_ACTION:
            nmp_msg_parsed_data_ptr->pdr_action = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));
                printf("%s%-32s :-> %u \n", space, "PDR Action", nmp_msg_parsed_data_ptr->pdr_action);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+1 bytes are parsed", ptr - 2, 3);
            return 1;

        case ITEM_ID__FAR_ACTION_FLAGS:
            nmp_msg_parsed_data_ptr->far_action_flags = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));
                printf("%s%-32s :-> 0x%02x \n", space, "FAR Action Flags", 
                        nmp_msg_parsed_data_ptr->far_action_flags);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+1 bytes are parsed", ptr - 2, 3);
            return 1;

        case ITEM_ID__FAR_DST_INTERFACE:
            nmp_msg_parsed_data_ptr->far_dst_interface = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));
                get_far_dst_interface_string(nmp_msg_parsed_data_ptr->far_dst_interface, string);
                printf("%s%-32s :-> %s \n", space, "FAR Destination Interface", string);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+1 bytes are parsed", ptr - 2, 3);
            return 1;

        case ITEM_ID__DEFAULT_PAGING_DRX:
            nmp_msg_parsed_data_ptr->default_paging_drx = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));
                get_default_paging_drx_string(nmp_msg_parsed_data_ptr->default_paging_drx, string);
                printf("%s%-32s :-> %s (%u) \n", space, "Default Paging DRX", string, 
                              nmp_msg_parsed_data_ptr->default_paging_drx);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+1 bytes are parsed", ptr - 2, 3);
            return 1;

        case ITEM_ID__RELATIVE_AMF_CAPACITY:
            nmp_msg_parsed_data_ptr->relative_amf_capacity = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));
                printf("%s%-32s :-> %u \n", space, "Relative AMF Capacity", 
                                 nmp_msg_parsed_data_ptr->relative_amf_capacity);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+1 bytes are parsed", ptr - 2, 3);
            return 1; 



        /////////////////////////////////////////////////////////////////////
        // Extract 2 byte items.
        /////////////////////////////////////////////////////////////////////
        case ITEM_ID__RAN_UE_NGAP_ID:
            nmp_msg_parsed_data_ptr->ran_ue_ngap_id = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));
                printf("%s%-32s :-> %u \n", space, "RAN-UE-NGAP-ID", nmp_msg_parsed_data_ptr->ran_ue_ngap_id);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 4);
            return 2;

        case ITEM_ID__AMF_UE_NGAP_ID:
            nmp_msg_parsed_data_ptr->amf_ue_ngap_id = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));
                printf("%s%-32s :-> %u \n", space, "AMF-UE-NGAP-ID", nmp_msg_parsed_data_ptr->amf_ue_ngap_id);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 4);
            return 2;

        case ITEM_ID__PDR_RULE_ID:
            nmp_msg_parsed_data_ptr->pdr_rule_id = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));
                printf("%s%-32s :-> %u \n", space, "PDR Rule ID", nmp_msg_parsed_data_ptr->pdr_rule_id);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 4);
            return 2;

        case ITEM_ID__PDR_PRECEDENCE:
            nmp_msg_parsed_data_ptr->pdr_precedence = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));
                printf("%s%-32s :-> %u \n", space, "PDR Precedence", nmp_msg_parsed_data_ptr->pdr_precedence);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 4);
            return 2;

        case ITEM_ID__PDR_FAR_ID:
            nmp_msg_parsed_data_ptr->pdr_far_id = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));
                printf("%s%-32s :-> %u \n", space, "PDR FAR Id", nmp_msg_parsed_data_ptr->pdr_far_id);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 4);
            return 2;

        case ITEM_ID__FAR_RULE_ID:
            nmp_msg_parsed_data_ptr->far_rule_id = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));
                printf("%s%-32s :-> %u \n", space, "FAR Rule Id", nmp_msg_parsed_data_ptr->far_rule_id);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 4);
            return 2;

        case ITEM_ID__RRC_ESTABLISH_CAUSE:
            nmp_msg_parsed_data_ptr->rrc_establish_cause = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));
                printf("%s%-32s :-> %u \n", space, "RRC Establishment Cause", 
                             nmp_msg_parsed_data_ptr->rrc_establish_cause);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 4);
            return 2;

        case ITEM_ID__MSG_RESPONSE_CODE:
            nmp_msg_parsed_data_ptr->msg_response_code = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));
                printf("%s%-32s :-> %u \n", space, "Message Response Code",
                             nmp_msg_parsed_data_ptr->msg_response_code);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 4);
            return 2;



        /////////////////////////////////////////////////////////////////////
        // Extract 4 byte items.
        /////////////////////////////////////////////////////////////////////
        case ITEM_ID__MCC_MNC:
            nmp_msg_parsed_data_ptr->mcc = htons(*((uint16_t *)ptr));
            nmp_msg_parsed_data_ptr->mnc = htons(*((uint16_t *)(ptr + 2)));

            if(debug_flag)
            {
                printf("%s%-16s : 4 bytes (%02x%02x%02x%02x) \n", space, "Item Value", 
                        *(ptr), *(ptr + 1), *(ptr + 2), *(ptr + 3));

                printf("%s%-32s :-> 0x%x (%u) \n", space, "MCC (Mobile Country Code)",
                        nmp_msg_parsed_data_ptr->mcc, nmp_msg_parsed_data_ptr->mcc);

                printf("%s%-32s :-> 0x%x (%u) \n", space, "MNC (Mobile Network Code)",
                        nmp_msg_parsed_data_ptr->mnc, nmp_msg_parsed_data_ptr->mnc);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 6);
            return 4;

        case ITEM_ID__GNODEB_ID:
            nmp_msg_parsed_data_ptr->gnodeb_id = htonl(*((uint32_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 4 bytes (%02x%02x%02x%02x) \n", space, "Item Value",
                        *(ptr), *(ptr + 1), *(ptr + 2), *(ptr + 3));

                printf("%s%-32s :-> %u \n", space, "gNodeB ID",
                        nmp_msg_parsed_data_ptr->gnodeb_id);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 6);
            return 4;

        case ITEM_ID__TAC:
            nmp_msg_parsed_data_ptr->tac = htonl(*((uint32_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 4 bytes (%02x%02x%02x%02x) \n", space, "Item Value", 
                        *(ptr), *(ptr + 1), *(ptr + 2), *(ptr + 3));

                printf("%s%-32s :-> %u \n", space, "TAC (Tracking Area Code)", 
                        nmp_msg_parsed_data_ptr->tac);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 6);
            return 4; 

        case ITEM_ID__CELL_ID:
            nmp_msg_parsed_data_ptr->cell_id = htonl(*((uint32_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 4 bytes (%02x%02x%02x%02x) \n", space, "Item Value", 
                        *(ptr), *(ptr + 1), *(ptr + 2), *(ptr + 3));

                printf("%s%-32s :-> %u \n", space, "CELL_ID (Cell Identity)", 
                        nmp_msg_parsed_data_ptr->cell_id);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 6);
            return 4;

        case ITEM_ID__TMSI:
            nmp_msg_parsed_data_ptr->tmsi = htonl(*((uint32_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 4 bytes (%02x%02x%02x%02x) \n", space, "Item Value", 
                        *(ptr), *(ptr + 1), *(ptr + 2), *(ptr + 3));

                printf("%s%-32s :-> 0x%x (%u) \n", space, "Mobile-TMSI", 
                        nmp_msg_parsed_data_ptr->tmsi,
                        nmp_msg_parsed_data_ptr->tmsi);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 6);
            return 4;

        case ITEM_ID__UE_IPV4_ADDR:
            nmp_msg_parsed_data_ptr->ue_ip_addr.ip_version = IP_VER_IS_V4;
            nmp_msg_parsed_data_ptr->ue_ip_addr.u.v4_addr  = htonl(*((uint32_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 4 bytes (%02x%02x%02x%02x) \n", space, "Item Value", 
                        *(ptr), *(ptr + 1), *(ptr + 2), *(ptr + 3));

                get_ipv4_addr_string(nmp_msg_parsed_data_ptr->ue_ip_addr.u.v4_addr, string);
                printf("%s%-32s :-> %s \n", space, "Allocated UE IPv4 Address", string);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 6);
            return 4;

        case ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR:
            nmp_msg_parsed_data_ptr->pdr_pdi_match_ipv4_addr = htonl(*((uint32_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 4 bytes (%02x%02x%02x%02x) \n", space, "Item Value", 
                        *(ptr), *(ptr + 1), *(ptr + 2), *(ptr + 3));

                get_ipv4_addr_string(nmp_msg_parsed_data_ptr->pdr_pdi_match_ipv4_addr, string);
                printf("%s%-32s :-> %s \n", space, "PDR PDI Match IPv4 Addr", string);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 6);
            return 4;

        case ITEM_ID__PDR_PDI_MATCH_GTPU_TEID:
            nmp_msg_parsed_data_ptr->pdr_pdi_match_gtpu_teid = htonl(*((uint32_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 4 bytes (%02x%02x%02x%02x) \n", space, "Item Value", 
                        *(ptr), *(ptr + 1), *(ptr + 2), *(ptr + 3));

                printf("%s%-32s :-> 0x%x (%u) \n", space, "PDR PDI Match GTP-U TEID", 
                        nmp_msg_parsed_data_ptr->pdr_pdi_match_gtpu_teid,
                        nmp_msg_parsed_data_ptr->pdr_pdi_match_gtpu_teid);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+4 bytes are parsed", ptr - 2, 6);
            return 4;




        /////////////////////////////////////////////////////////////////////
        // Extract upto 8 byte items.
        /////////////////////////////////////////////////////////////////////
        case ITEM_ID__IMSI:
            memcpy(nmp_msg_parsed_data_ptr->imsi.u8, ptr, 8);

            if(debug_flag)
            {
                printf("%s%-16s : 8 bytes (", space, "Item Value");
                for(i = 0; i < 8; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                printf("%s%-32s :-> %02x%02x%02x%02x%02x%02x%02x%02x \n", space, "IMSI",
                        nmp_msg_parsed_data_ptr->imsi.u8[0], nmp_msg_parsed_data_ptr->imsi.u8[1],
                        nmp_msg_parsed_data_ptr->imsi.u8[2], nmp_msg_parsed_data_ptr->imsi.u8[3],
                        nmp_msg_parsed_data_ptr->imsi.u8[4], nmp_msg_parsed_data_ptr->imsi.u8[5],
                        nmp_msg_parsed_data_ptr->imsi.u8[6], nmp_msg_parsed_data_ptr->imsi.u8[7]);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+8 bytes are parsed", ptr - 2, 10);
            return 8;

        case ITEM_ID__NSSAI:
            // NSSAI consists of SST(1 byte) and SD(4 bytes).. 
            // So, relevant data is only present in first 5 bytes out of total 8 bytes.
            nmp_msg_parsed_data_ptr->nssai_sst = *ptr;
            nmp_msg_parsed_data_ptr->nssai_sd  = htonl(*((uint32_t *)(ptr + 1)));

            if(debug_flag)
            {
                printf("%s%-16s : 8 bytes (", space, "Item Value");
                for(i = 0; i < 8; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                printf("%s%-32s :-> %u \n", space, "NSSAI ST", nmp_msg_parsed_data_ptr->nssai_sst);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "NSSAI SD",
                        nmp_msg_parsed_data_ptr->nssai_sd,
                        nmp_msg_parsed_data_ptr->nssai_sd);

            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+8 bytes are parsed", ptr - 2, 10);
            return 8;   

        case ITEM_ID__GTPU_SELF_IPV4_ENDPOINT:
            nmp_msg_parsed_data_ptr->self_v4_endpoint.v4_addr = htonl(*((uint32_t *)ptr));
            nmp_msg_parsed_data_ptr->self_v4_endpoint.teid    = htonl(*((uint32_t *)(ptr + 4)));

            if(debug_flag)
            {
                printf("%s%-16s : 8 bytes (", space, "Item Value");
                for(i = 0; i < 8; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                get_ipv4_addr_string(nmp_msg_parsed_data_ptr->self_v4_endpoint.v4_addr, string);
                printf("%s%-32s :-> %s \n", space, "GTP endpoint (My Ipv4 Addr)", string);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "GTP endpoint (My TEID)",
                        nmp_msg_parsed_data_ptr->self_v4_endpoint.teid, 
                        nmp_msg_parsed_data_ptr->self_v4_endpoint.teid);

            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+8 bytes are parsed", ptr - 2, 10);
            return 8;

        case ITEM_ID__GTPU_PEER_IPV4_ENDPOINT:
            nmp_msg_parsed_data_ptr->peer_v4_endpoint.v4_addr = htonl(*((uint32_t *)ptr));
            nmp_msg_parsed_data_ptr->peer_v4_endpoint.teid    = htonl(*((uint32_t *)(ptr + 4)));

            if(debug_flag)
            {
                printf("%s%-16s : 8 bytes (", space, "Item Value");
                for(i = 0; i < 8; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                get_ipv4_addr_string(nmp_msg_parsed_data_ptr->peer_v4_endpoint.v4_addr, string);
                printf("%s%-32s :-> %s \n", space, "GTP endpoint (Peer Ipv4 Addr)", string);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "GTP endpoint (Peer TEID)",
                        nmp_msg_parsed_data_ptr->peer_v4_endpoint.teid, 
                        nmp_msg_parsed_data_ptr->peer_v4_endpoint.teid);

            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+8 bytes are parsed", ptr - 2, 10);
            return 8;

        case ITEM_ID__UPLINK_GTPU_IPV4_ENDPOINT:
            nmp_msg_parsed_data_ptr->upf_n3_addr = htonl(*((uint32_t *)ptr));
            nmp_msg_parsed_data_ptr->upf_n3_teid = htonl(*((uint32_t *)(ptr + 4)));

            if(debug_flag)
            {
                printf("%s%-16s : 8 bytes (", space, "Item Value");
                for(i = 0; i < 8; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                get_ipv4_addr_string(nmp_msg_parsed_data_ptr->upf_n3_addr, string);
                printf("%s%-32s :-> %s \n", space, "Uplink GTP endpoint (UPF N3 Ipv4 Addr)", string);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "Uplink GTP TEID (UPF N3 TEID)",
                        nmp_msg_parsed_data_ptr->upf_n3_addr,
                        nmp_msg_parsed_data_ptr->upf_n3_teid);

            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+8 bytes are parsed", ptr - 2, 10);
            return 8;

        case ITEM_ID__DNLINK_GTPU_IPV4_ENDPOINT:
            nmp_msg_parsed_data_ptr->gnb_n3_addr = htonl(*((uint32_t *)ptr));
            nmp_msg_parsed_data_ptr->gnb_n3_teid = htonl(*((uint32_t *)(ptr + 4)));

            if(debug_flag)
            {
                printf("%s%-16s : 8 bytes (", space, "Item Value");
                for(i = 0; i < 8; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                get_ipv4_addr_string(nmp_msg_parsed_data_ptr->gnb_n3_addr, string);
                printf("%s%-32s :-> %s \n", space, "Dnlink GTP endpoint (gnodeB N3 Ipv4 Addr)", string);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "Dnlink GTP TEID (gnodeB N3 TEID)",
                        nmp_msg_parsed_data_ptr->gnb_n3_addr,
                        nmp_msg_parsed_data_ptr->gnb_n3_teid);

            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+8 bytes are parsed", ptr - 2, 10);
            return 8;

        case ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE:
            nmp_msg_parsed_data_ptr->far_outer_v4_hdr_create_addr = htonl(*((uint32_t *)ptr));
            nmp_msg_parsed_data_ptr->far_outer_v4_hdr_create_teid = htonl(*((uint32_t *)(ptr + 4)));

            if(debug_flag)
            {
                printf("%s%-16s : 8 bytes (", space, "Item Value");
                for(i = 0; i < 8; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                get_ipv4_addr_string(nmp_msg_parsed_data_ptr->far_outer_v4_hdr_create_addr, string);
                printf("%s%-32s :-> %s \n", space, "FAR Outer Ipv4 Hdr Creation (Address)", string);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "FAR Outer Ipv4 Hdr Creation (TEID)",
                        nmp_msg_parsed_data_ptr->far_outer_v4_hdr_create_teid,
                        nmp_msg_parsed_data_ptr->far_outer_v4_hdr_create_teid);

            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+8 bytes are parsed", ptr - 2, 10);
            return 8;

        case ITEM_ID__USER_LOCATION_INFO_TAC:
            nmp_msg_parsed_data_ptr->user_loc_info_mcc = htons(*((uint16_t *)ptr));
            nmp_msg_parsed_data_ptr->user_loc_info_mnc = htons(*((uint16_t *)(ptr + 2)));
            nmp_msg_parsed_data_ptr->user_loc_info_tac = htonl(*((uint32_t *)(ptr + 4)));

            if(debug_flag)
            {
                printf("%s%-16s : 8 bytes (", space, "Item Value");
                for(i = 0; i < 8; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                printf("%s%-32s :-> 0x%x (%u) \n", space, "User Location Info (MCC)", 
                        nmp_msg_parsed_data_ptr->user_loc_info_mcc,
                        nmp_msg_parsed_data_ptr->user_loc_info_mcc);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "User Location Info (MNC)", 
                        nmp_msg_parsed_data_ptr->user_loc_info_mnc,
                        nmp_msg_parsed_data_ptr->user_loc_info_mnc);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "User Location Info (TAC)",
                        nmp_msg_parsed_data_ptr->user_loc_info_tac,
                        nmp_msg_parsed_data_ptr->user_loc_info_tac);

            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+8 bytes are parsed", ptr - 2, 10);
            return 8;


        /////////////////////////////////////////////////////////////////////
        // Extract 16 byte items.
        /////////////////////////////////////////////////////////////////////
        case ITEM_ID__UE_IDENTIFIER_SECRET:
            memcpy(nmp_msg_parsed_data_ptr->ue_identifier_secret, ptr, 16);

            if(debug_flag)
            {
                printf("%s%-16s : 16 bytes (", space, "Item Value");
                for(i = 0; i < 16; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                printf("%s%-32s :-> ", space, "UE Identifier Secret");
                for(i = 0; i < 16; i++)
                {
                    printf("%02x", *(ptr + i));
                } 
                printf("\n");
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+16 bytes are parsed", ptr - 2, 18);
            return 16;

        case ITEM_ID__UE_IPV6_ADDR:
            nmp_msg_parsed_data_ptr->ue_ip_addr.ip_version = IP_VER_IS_V6;
            memcpy(nmp_msg_parsed_data_ptr->ue_ip_addr.u.v6_addr.u8, ptr, 16);

            if(debug_flag)
            {
                printf("%s%-16s : 16 bytes (", space, "Item Value");
                for(i = 0; i < 16; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                get_ipv6_addr_string(nmp_msg_parsed_data_ptr->ue_ip_addr.u.v6_addr.u8, string);
                printf("%s%-32s : %s \n", space, "UE IPv6 Address", string);
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+16 bytes are parsed", ptr - 2, 18);
            return 16;

        case ITEM_ID__USER_LOCATION_INFO_NR_CGI:
            nmp_msg_parsed_data_ptr->user_loc_info_mcc = htons(*((uint16_t *)ptr));
            nmp_msg_parsed_data_ptr->user_loc_info_mnc = htons(*((uint16_t *)(ptr + 2)));
            memcpy(nmp_msg_parsed_data_ptr->user_loc_info_nr_cell_identity, ptr + 8, 8);

            if(debug_flag)
            {
                printf("%s%-16s : 16 bytes (", space, "Item Value");
                for(i = 0; i < 16; i++)
                {
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                printf("%s%-32s :-> 0x%x (%u) \n", space, "User Location Info (MCC)",
                        nmp_msg_parsed_data_ptr->user_loc_info_mcc,
                        nmp_msg_parsed_data_ptr->user_loc_info_mcc);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "User Location Info (MNC)",
                        nmp_msg_parsed_data_ptr->user_loc_info_mnc,
                        nmp_msg_parsed_data_ptr->user_loc_info_mnc);
                printf("%s%-32s :->  ", space, "User Location Info (NR Cell Identity)");
                for(i = 0; i < 8; i++)
                {
                    printf("%02x", *(ptr + 8 + i));
                }
            }
            if(g__byte_debug_flag) dump_bytes(space, "Type-1 item, 2+16 bytes are parsed", ptr - 2, 18);
            return 16;


        default:
            printf("Error: Unknown Type-1 Item \n");
            return -1;
    }

    return -1;
}


/////////////////////////////////////////////////////
// Type-2 items carry variable length item values
/////////////////////////////////////////////////////
static int
get_type2_item_value(char           *space,
                     uint16_t        item_id,
                     uint8_t        *ptr,
                     nmp_msg_data_t *nmp_msg_parsed_data_ptr,
                     uint8_t         debug_flag)
{
    int i = 0;
    char string[128];
    uint16_t item_len = htons(*((uint16_t *)ptr));
    
    // ----------------------------------------
    // | Item length | Actual item value bytes |
    // | (2 bytes)   |       n bytes           |
    // ----------------------------------------
    if(debug_flag) printf("%s%-16s : %u (0x%04x) \n", space, "Item Length", item_len, item_len);
    switch (item_id)
    {
        case ITEM_ID__MSG_RESPONSE_DESCRIPTION:
            memcpy(nmp_msg_parsed_data_ptr->msg_response_description, ptr + 2, item_len);

            if(debug_flag)
            {
                printf("%s%-16s : %u bytes (", space, "Item Value", item_len);
                printf("%s", (char *)(ptr + 2));
                printf(")\n");
            }
            return (2 + item_len);

        case ITEM_ID__GTPU_SELF_IPV6_ENDPOINT:

            if(item_len != 20)
            {
                printf("Invalid item length for ITEM_ID__GTPU_SELF_IPV6_ENDPOINT \n");
                return -1;
            }

            memcpy(nmp_msg_parsed_data_ptr->self_v6_endpoint.v6_addr.u8, ptr + 2, 16);
            nmp_msg_parsed_data_ptr->self_v6_endpoint.teid = htonl(*((uint32_t *)(ptr + 18)));

            if(debug_flag)
            {
                printf("%s%-16s : %u bytes (", space, "Item Value", item_len);
                for(i = 0; i < item_len; i++)
                {
                    printf("%02x", *(ptr + 2 + i));
                }
                printf(")\n");

                get_ipv6_addr_string(nmp_msg_parsed_data_ptr->self_v6_endpoint.v6_addr.u8, string);
                printf("%s%-32s :-> %s \n", space, "GTP endpoint (Self Ipv6 Addr)", string);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "GTP endpoint (Self TEID)",
                        nmp_msg_parsed_data_ptr->self_v6_endpoint.teid, 
                        nmp_msg_parsed_data_ptr->self_v6_endpoint.teid);

            }
            return (2 + item_len);


        case ITEM_ID__GTPU_PEER_IPV6_ENDPOINT:

            if(item_len != 20)
            {
                printf("Invalid item length for ITEM_ID__GTPU_PEER_IPV6_ENDPOINT \n");
                return -1;
            }

            memcpy(nmp_msg_parsed_data_ptr->peer_v6_endpoint.v6_addr.u8, ptr + 2, 16);
            nmp_msg_parsed_data_ptr->peer_v6_endpoint.teid = htonl(*((uint32_t *)(ptr + 18)));

            if(debug_flag)
            {
                printf("%s%-16s : %u bytes (", space, "Item Value", item_len);
                for(i = 0; i < item_len; i++)
                {
                    printf("%02x", *(ptr + 2 + i));
                }
                printf(")\n");

                get_ipv6_addr_string(nmp_msg_parsed_data_ptr->peer_v6_endpoint.v6_addr.u8, string);
                printf("%s%-32s :-> %s \n", space, "GTP endpoint (Peer Ipv6 Addr)", string);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "GTP endpoint (Peer TEID)",
                        nmp_msg_parsed_data_ptr->peer_v6_endpoint.teid,
                        nmp_msg_parsed_data_ptr->peer_v6_endpoint.teid);

            }
            return (2 + item_len);

        case ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE:

            if(item_len != 20)
            {
                printf("Invalid item length for ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE \n");
                return -1;
            }

            memcpy(nmp_msg_parsed_data_ptr->far_outer_v6_hdr_create_addr.u8, ptr + 2, 16);
            nmp_msg_parsed_data_ptr->far_outer_v6_hdr_create_teid = htonl(*((uint32_t *)(ptr + 18)));

            if(debug_flag)
            {
                printf("%s%-16s : %u bytes (", space, "Item Value", item_len);
                for(i = 0; i < item_len; i++)
                {
                    printf("%02x", *(ptr + 2 + i));
                }
                printf(")\n");

                get_ipv6_addr_string(nmp_msg_parsed_data_ptr->far_outer_v6_hdr_create_addr.u8, string);
                printf("%s%-32s :-> %s \n", space, "FAR Outer Ipv6 Hdr Create (Addr)", string);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "FAR Outer Ipv6 Hdr Create (TEID)",
                        nmp_msg_parsed_data_ptr->far_outer_v6_hdr_create_teid,
                        nmp_msg_parsed_data_ptr->far_outer_v6_hdr_create_teid);

            }
            return (2 + item_len);

        case ITEM_ID__NAS_PDU:
            memcpy(nmp_msg_parsed_data_ptr->nas_pdu, ptr + 2, item_len);

            if(debug_flag)
            {
                printf("%s%-16s : %u bytes (", space, "Item Value", item_len);
                for(i = 0; i < item_len; i++)
                {
                    printf("%02x", *(ptr + 2 + i));
                }
                printf(")\n");

                printf("%s%-32s :-> ", space, "NAS PDU");
                for(i = 0; i < item_len; i++)
                {
                    printf("%02x", *(ptr + 2 + i));
                }
                printf("\n");
            }
            return (2 + item_len);

        case ITEM_ID__RAN_NODE_NAME:
            memcpy(nmp_msg_parsed_data_ptr->ran_node_name, ptr + 2, item_len);

            if(debug_flag)
            {
                printf("%s%-16s : %u bytes (", space, "Item Value", item_len);
                for(i = 0; i < item_len; i++)
                {
                    printf("%02x", *(ptr + 2 + i));
                }
                printf(")\n");

                printf("%s%-32s :-> %s \n", space, "RAN Node Name", nmp_msg_parsed_data_ptr->ran_node_name);
            }
            return (2 + item_len);

        case ITEM_ID__AMF_NAME:
            memcpy(nmp_msg_parsed_data_ptr->amf_name, ptr + 2, item_len);

            if(debug_flag)
            {
                printf("%s%-16s : %u bytes (", space, "Item Value", item_len);
                for(i = 0; i < item_len; i++)
                {
                    printf("%02x", *(ptr + 2 + i));
                }
                printf(")\n");

                printf("%s%-32s :-> %s \n", space, "AMF Name", nmp_msg_parsed_data_ptr->amf_name);
            }
            return (2 + item_len);

        case ITEM_ID__GUAMI:
            nmp_msg_parsed_data_ptr->guami_mcc = htons(*((uint16_t *)(ptr + 2))); 
            nmp_msg_parsed_data_ptr->guami_mnc = htons(*((uint16_t *)(ptr + 4)));    
            nmp_msg_parsed_data_ptr->guami_amf_region_id = htons(*((uint16_t *)(ptr + 6)));
            nmp_msg_parsed_data_ptr->guami_amf_set_id    = htons(*((uint16_t *)(ptr + 8)));
            nmp_msg_parsed_data_ptr->guami_amf_pointer   = htons(*((uint16_t *)(ptr + 10)));

            if(debug_flag)
            {
                printf("%s%-16s : %u bytes (", space, "Item Value", item_len);
                for(i = 0; i < item_len; i++)
                {
                    printf("%02x", *(ptr + 2 + i));
                }
                printf(")\n");

                printf("%s%-32s :-> 0x%x(%u) \n", space, "Guami MCC", 
                                   nmp_msg_parsed_data_ptr->guami_mcc,
                                   nmp_msg_parsed_data_ptr->guami_mcc);
                printf("%s%-32s :-> 0x%x(%u) \n", space, "Guami MNC", 
                                   nmp_msg_parsed_data_ptr->guami_mnc,
                                   nmp_msg_parsed_data_ptr->guami_mnc);
                printf("%s%-32s :-> 0x%x(%u) \n", space, "Guami AMF Region ID",
                                   nmp_msg_parsed_data_ptr->guami_amf_region_id,
                                   nmp_msg_parsed_data_ptr->guami_amf_region_id);
                printf("%s%-32s :-> 0x%x(%u) \n", space, "Guami AMF Set ID",
                                   nmp_msg_parsed_data_ptr->guami_amf_set_id,
                                   nmp_msg_parsed_data_ptr->guami_amf_set_id);
                printf("%s%-32s :-> 0x%x(%u) \n", space, "Guami AMF Pointer",
                                   nmp_msg_parsed_data_ptr->guami_amf_pointer,
                                   nmp_msg_parsed_data_ptr->guami_amf_pointer);
            }
            return (2 + item_len);

        case ITEM_ID__SERVICE_INFO_JSON_DATA:
            memcpy(nmp_msg_parsed_data_ptr->json_data, ptr + 2, item_len);

            if(debug_flag)
            {
                cJSON *json_data = cJSON_Parse((const char *)(nmp_msg_parsed_data_ptr->json_data));
                if(NULL == json_data)
                {
                    printf("SERVICE_INFO_JSON_DATA: Unable to parse received data \n");
                    cJSON_Delete(json_data); 
                    return -1;
                }
                char *json_string = cJSON_Print(json_data);
                printf("%s%-16s : %u bytes   %s ", space, "Item Value", item_len, json_string);
                cJSON_free(json_string);
                cJSON_Delete(json_data);
            }
            return (2 + item_len);

        case ITEM_ID__SESSION_CREATE_REQ_JSON_DATA:
            memcpy(nmp_msg_parsed_data_ptr->json_data, ptr + 2, item_len);

            if(debug_flag)
            {
                cJSON *json_data = cJSON_Parse((const char *)(nmp_msg_parsed_data_ptr->json_data));
                if(NULL == json_data)
                {
                    printf("SESSION_CREATE_REQ_JSON_DATA: Unable to parse received data \n");
                    cJSON_Delete(json_data);
                    return -1;
                }
                char *json_string = cJSON_Print(json_data);
                printf("%s%-16s : %u bytes   %s ", space, "Item Value", item_len, json_string);
                cJSON_free(json_string);
                cJSON_Delete(json_data);
            }
            return (2 + item_len);

        case ITEM_ID__SESSION_CREATE_RESP_JSON_DATA:
            memcpy(nmp_msg_parsed_data_ptr->json_data, ptr + 2, item_len);

            if(debug_flag)
            {
                cJSON *json_data = cJSON_Parse((const char *)(nmp_msg_parsed_data_ptr->json_data));
                if(NULL == json_data)
                {
                    printf("SESSION_CREATE_RESP_JSON_DATA: Unable to parse received data \n");
                    cJSON_Delete(json_data);
                    return -1;
                }
                char *json_string = cJSON_Print(json_data);
                printf("%s%-16s : %u bytes   %s ", space, "Item Value", item_len, json_string);
                cJSON_free(json_string);
                cJSON_Delete(json_data);
            }
            return (2 + item_len);
 
        default:
            printf("Error: Unknown Type-2 Item \n");
            return -1;
    }

    return -1;
}





static int
get_item_group_values(char           *space,
                      uint8_t        *ptr,
                      nmp_msg_data_t *nmp_msg_parsed_data_ptr,
                      uint8_t         debug_flag)
{
    int i = 0;
    int ret = 0;
    int offset = 0;
    uint8_t item_type = 0;
    uint16_t item_id = 0;
    uint16_t item_count = htons(*((uint16_t *)ptr));;
    uint16_t item_len = htons(*((uint16_t *)(ptr + 2)));

    if(debug_flag)
    {
        printf("%s%-16s : %u (0x%04x) \n", space, "Item Count", item_count, item_count);
        printf("%s%-16s : %u (0x%04x) \n", space, "Item Length", item_len, item_len);
        printf("%s---------------------------------------------------------------\n", space);
    }
    
    offset = 4;  // 2 bytes of item_count + 2 bytes of item_len
    for(i = 0; i < item_count; i++)
    {
        item_id = htons(*((uint16_t *)(ptr + offset)));
        if(-1 == get_item_type(item_id, &item_type))
        {
            return -1;
        }

        if(ITEM_TYPE_IS_TYPE_1 == item_type)
        {
            if(debug_flag) dump_item_id(space, item_id);
            ret = get_type1_item_value(space,
                                       item_id,
                                       ptr + offset + 2,  
                                       nmp_msg_parsed_data_ptr,
                                       debug_flag);
            if(-1 == ret)
            {
                return -1;
            }
            else
            {
                offset += (2 + ret);
            }
        }
        else if(ITEM_TYPE_IS_TYPE_2 == item_type)
        {
            if(debug_flag) dump_item_id(space, item_id);
            ret = get_type2_item_value(space,
                                       item_id,
                                       ptr + offset + 2, 
                                       nmp_msg_parsed_data_ptr,
                                       debug_flag);
            if(-1 == ret)
            {
                return -1;
            }
            else
            {
                offset += (2 + ret);
            }
        }
        else if(ITEM_TYPE_IS_TYPE_3 == item_type)
        {
            strcat(space, "    ");
            if(debug_flag) dump_item_id(space, item_id);
            // Item group consists of item-groups... (Recursion)
            ret = get_item_group_values(space,
                                        ptr + offset + 2, // points to first byte of item_count
                                        nmp_msg_parsed_data_ptr,
                                        debug_flag);
            if(-1 == ret)
            {
                return -1;
            }
            else
            {
                offset += (2 + ret);
            }
        }
        else
        {
            printf("Item Group contains invalid item_type ... invalid case \n");
            return -1;
        }

        if(debug_flag)
        {
            if(i < (item_count - 1)) printf("\n");
        }
    }
    if(debug_flag) printf("%s---------------------------------------------------------------\n", space);	
    return offset;	
}

		 		

static int
get_item_group_value(char           *space,
                     uint16_t        item_id,
                     uint8_t        *ptr,
                     nmp_msg_data_t *nmp_msg_parsed_data_ptr,
                     uint8_t         debug_flag)
{
    int ret = 0;
    strcpy(space, "    ");
    
    switch (item_id)
    {
        case ITEM_GROUP_ID__N3_PDR:
        case ITEM_GROUP_ID__N6_PDR:
        case ITEM_GROUP_ID__N3_FAR:
        case ITEM_GROUP_ID__N6_FAR:
        case ITEM_GROUP_ID__USER_LOCATION_INFO:
        case ITEM_GROUP_ID__GLOBAL_RAN_NODE_ID:
        case ITEM_GROUP_ID__GUAMI_LIST:
        case ITEM_GROUP_ID__SUPPORTED_TA_LIST:
        case ITEM_GROUP_ID__SUPPORTED_TA_LIST_ITEM:
        case ITEM_GROUP_ID__PLMN_SUPPORT_LIST:
        case ITEM_GROUP_ID__PLMN_SUPPORT_LIST_ITEM:
            ret = get_item_group_values(space,
                                        ptr, 
                                        nmp_msg_parsed_data_ptr, 
                                        debug_flag);
            if(-1 == ret)
            {
                return -1;
            }
            return ret;

        default:
            printf("%s: Unknown item_id %u \n", __func__, item_id); 
            return -1;
    }

    return -1;
}



int
parse_nmp_msg(uint8_t        *msg_ptr,
              int             buf_len,
              nmp_msg_data_t *nmp_msg_parsed_data_ptr,
              uint8_t         debug_flag)
{
    int ret = 0;
    int offset = 0;
    int item_index = 0;
    uint8_t g__byte_debug_flag = 0; // make it 1 for byte parsing views..
    nmp_hdr_t *nmp_hdr_ptr = NULL;
    uint8_t *nmp_msg_ptr = NULL;
    uint16_t *item_id_ptr = NULL;
    uint16_t  item_id = 0;
    uint8_t item_type = 0;
    uint16_t parsed_item_count = 0;
    uint16_t item_group_bytelen = 0;
    uint8_t space[128];
    memset(space, 0x0, 128);

    memset(nmp_msg_parsed_data_ptr, 0x0, sizeof(nmp_msg_data_t));

    nmp_hdr_ptr = (nmp_hdr_t *)msg_ptr;

    nmp_msg_parsed_data_ptr->src_node_type = htons(nmp_hdr_ptr->src_node_type);
    nmp_msg_parsed_data_ptr->dst_node_type = htons(nmp_hdr_ptr->dst_node_type);
    nmp_msg_parsed_data_ptr->src_node_id = htons(nmp_hdr_ptr->src_node_id);
    nmp_msg_parsed_data_ptr->dst_node_id = htons(nmp_hdr_ptr->dst_node_id);
    nmp_msg_parsed_data_ptr->msg_type = htons(nmp_hdr_ptr->msg_type);
    nmp_msg_parsed_data_ptr->msg_item_len = htons(nmp_hdr_ptr->msg_item_len);
    nmp_msg_parsed_data_ptr->msg_identifier = htonl(nmp_hdr_ptr->msg_identifier);
    nmp_msg_parsed_data_ptr->msg_item_count = htons(nmp_hdr_ptr->msg_item_count);
    
    nmp_msg_ptr = (uint8_t *)(nmp_hdr_ptr + 1);
    offset = 0;

    if(debug_flag)
    {
        printf("\n");
        YELLOW_PRINT("!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=! NEW MESSAGE !=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=!=! \n");
        printf("\n");
        MAGENTA_PRINT("--------------------------------------------------------------------------------------\n");
        dump_src_to_dst_node_data(nmp_msg_parsed_data_ptr->src_node_type,
                nmp_msg_parsed_data_ptr->dst_node_type,
                nmp_msg_parsed_data_ptr->src_node_id,
                nmp_msg_parsed_data_ptr->dst_node_id);
        MAGENTA_PRINT("--------------------------------------------------------------------------------------\n");
        dump_msg_type(nmp_msg_parsed_data_ptr->msg_type);
        MAGENTA_PRINT("--------------------------------------------------------------------------------------\n");
        printf("%-16s : 0x%08x (%u) \n", "Msg Identifier", 
                nmp_msg_parsed_data_ptr->msg_identifier, 
                nmp_msg_parsed_data_ptr->msg_identifier);  
        MAGENTA_PRINT("--------------------------------------------------------------------------------------\n");
        printf("%-16s : %u  \n", "Item Count", nmp_msg_parsed_data_ptr->msg_item_count);
        printf("%-16s : %u bytes (20 bytes of NMP header is not included) \n", 
               "Item length", nmp_msg_parsed_data_ptr->msg_item_len);
        if(g__byte_debug_flag) dump_bytes((char *)space, "NMP Payload bytes", nmp_msg_ptr, nmp_msg_parsed_data_ptr->msg_item_len);
        MAGENTA_PRINT("--------------------------------------------------------------------------------------\n");
    }

    item_index = 1;
    while((offset < nmp_msg_parsed_data_ptr->msg_item_len) && 
            (parsed_item_count < nmp_msg_parsed_data_ptr->msg_item_count))
    {	
        item_id_ptr = (uint16_t *)(nmp_msg_ptr + offset);
        item_id = htons(*(item_id_ptr));

        if(-1 == get_item_type(item_id, &item_type))
        {
            if(debug_flag) printf("Failure in getting Item Type for Item ID (%u) \n", item_id);
            return -1;
        }


        if(ITEM_TYPE_IS_TYPE_1 == item_type)
        {
            memset(space, 0x0, 128);
            
            if(debug_flag)
            {
                GREEN_PRINT("===============================================\n");
                GREEN_PRINT("Item Index: %u, Parse this Type-1 Item         \n", item_index);
                GREEN_PRINT("===============================================\n");
            }
            
            if(debug_flag) dump_item_id((char *)space, item_id);
            // Initial 2 bytes consists of item_id (already parsed)
            // Parse item value (fixed length.. 2, 4, 8, or 16)
            ret = get_type1_item_value((char *)space,
                                       item_id, 
                                       nmp_msg_ptr + offset + 2, 
                                       nmp_msg_parsed_data_ptr,
                                       debug_flag);
            if(-1 == ret)
            {
                if(debug_flag) printf("Failure in getting Type-1 item value \n");
                return -1;
            }
            else
            {
                if(g__byte_debug_flag)
                {
                    dump_bytes((char *)space, "These type-1 item bytes are now parsed", 
                               nmp_msg_ptr + offset, (2 + ret));
                }
                offset += (2 + ret);
                item_index += 1;
            }
        }
        else if(ITEM_TYPE_IS_TYPE_2 == item_type)
        {
            memset(space, 0x0, 128);
            
            if(debug_flag)
            {
                GREEN_PRINT("===============================================================\n");
                GREEN_PRINT("Item Index: %u, Parse this Type-2 Item  \n", item_index);
                GREEN_PRINT("===============================================================\n");
            }
            
            if(g__byte_debug_flag)
            {
                item_group_bytelen = htons(*((uint16_t *)(nmp_msg_ptr + offset + 2)));
                dump_bytes((char *)space, "Proceed to parse these Type-2 item bytes", 
                           nmp_msg_ptr + offset, 4 + item_group_bytelen);
            }
            
            if(debug_flag) dump_item_id((char *)space, item_id);
            // Initial 2 bytes consists of item_id (already parsed)
            // Next 2 bytes consists of item_len (variable length type-2 item)...
            // Then follows actual item value bytes...
            ret = get_type2_item_value((char *)space,
                                       item_id,
                                       nmp_msg_ptr + offset + 2,
                                       nmp_msg_parsed_data_ptr,
                                       debug_flag);
            if(-1 == ret)
            {
                if(debug_flag) printf("Failure in getting Type-2 item value \n");
                return -1;
            }
            else
            {
                if(g__byte_debug_flag)
                {
                    dump_bytes((char *)space, "These type-2 item bytes are now parsed", 
                               nmp_msg_ptr + offset, (2 + ret));
                }
                offset += (2 + ret);
                item_index += 1;
            }
        }
        else
        {
            if(debug_flag)
            {
                GREEN_PRINT("===============================================================\n");
                GREEN_PRINT("Item Index: %u, Parse this Item-Group          \n", item_index);
                GREEN_PRINT("===============================================================\n");
            }

            if(g__byte_debug_flag)
            {
                item_group_bytelen = htons(*((uint16_t *)(nmp_msg_ptr + offset + 4)));
                dump_bytes((char *)space, "Proceed to parse these item-group bytes", 
                           nmp_msg_ptr + offset, 6 + item_group_bytelen);
            }
            
            strcpy((char *)space, "    ");
            if(debug_flag) printf("%s---------------------------------------------------------------\n", space);	
            if(debug_flag) dump_item_id((char *)space, item_id);
            ret = get_item_group_value((char *)space,
                                       item_id,
                                       nmp_msg_ptr + offset + 2,
                                       nmp_msg_parsed_data_ptr,
                                       debug_flag);
            if(-1 == ret)
            {
                if(debug_flag) printf("Failure in getting Item group value \n");
                return -1;
            }
            else
            {
                if(g__byte_debug_flag)
                {
                    dump_bytes((char *)space, "These item-group bytes are now parsed", 
                               nmp_msg_ptr + offset, (2 + ret));
                }
                offset += (2 + ret);
                item_index += 1;
            }
        }
        if(g__byte_debug_flag) GREEN_PRINT("---> Parsing offset is now %u \n", offset);

        parsed_item_count += 1;

        if(debug_flag)
        {
            if(parsed_item_count < nmp_msg_parsed_data_ptr->msg_item_count)
            {
                printf("\n\n");
            }
        }
    }

    if(offset != htons(nmp_hdr_ptr->msg_item_len))
    {
        printf("Length mismatch \n");
        return -1;
    }

    if(debug_flag) 
    {
        printf("\n");
        MAGENTA_PRINT("--------------------------------------------------------------------------------------\n");
        GREEN_PRINT("Message Parsing Complete..."); printf("\n");
        MAGENTA_PRINT("--------------------------------------------------------------------------------------\n");
        printf("\n\n\n");
    }

    return 0;
}

