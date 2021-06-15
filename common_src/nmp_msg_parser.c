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


static void
get_node_type_string(uint16_t node_type, char *string)
{
    switch (node_type)
    {
        case NODE_TYPE__AMF:
            strcpy(string, "AMF");
            break;

        case NODE_TYPE__UE:
            strcpy(string, "UE");
            break;

        case NODE_TYPE__GNB:
            strcpy(string, "GNB");
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
        case MSG_TYPE__KEEPALIVE_REQUEST:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "KEEPALIVE_REQUEST"); printf("\n");
            break;

        case MSG_TYPE__KEEPALIVE_RESPONSE:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "KEEPALIVE_RESPONSE"); printf("\n");
            break;

        case MSG_TYPE__UE_ATTACH_REQUEST:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "UE_ATTACH_REQUEST"); printf("\n");
            break;

        case MSG_TYPE__UE_ATTACH_RESPONSE:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "UE_ATTACH_RESPONSE"); printf("\n");
            break;

        case MSG_TYPE__UE_DETACH_REQUEST:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "UE_DETACH_REQUEST"); printf("\n");
            break;

        case MSG_TYPE__UE_DETACH_RESPONSE:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "UE_DETACH_RESPONSE"); printf("\n");
            break;

        case MSG_TYPE__SESSION_CREATE_REQUEST:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "SESSION_CREATE_REQUEST"); printf("\n");
            break;

        case MSG_TYPE__SESSION_CREATE_RESPONSE:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "SESSION_CREATE_RESPONSE"); printf("\n");
            break;

        case MSG_TYPE__SESSION_MODIFY_REQUEST:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "SESSION_MODIFY_REQUEST"); printf("\n");
            break;

        case MSG_TYPE__SESSION_MODIFY_RESPONSE:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "SESSION_MODIFY_RESPONSE"); printf("\n");
            break;

        case MSG_TYPE__SESSION_DELETE_REQUEST:
            printf("%-16s : ", "Msg Type"); GREEN_PRINT("%s", "SESSION_DELETE_REQUEST"); printf("\n");
            break;

        case MSG_TYPE__SESSION_DELETE_RESPONSE:
            printf("%-16s : ", "Msg Type"); YELLOW_PRINT("%s", "SESSION_DELETE_RESPONSE"); printf("\n");
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
        case ITEM_ID__MSG_RESPONSE:
            printf("%s%-16s : %s \n", space, "Item ID", "MSG_RESPONSE");
            break;

        case ITEM_ID__UPLINK_QOS_PROFILE:
            printf("%s%-16s : %s \n", space, "Item ID", "UPLINK_QOS_PROFILE");
            break;

        case ITEM_ID__DNLINK_QOS_PROFILE:
            printf("%s%-16s : %s \n", space, "Item ID", "DNLINK_QOS_PROFILE");
            break;

        case ITEM_ID__PDR_ACTION:
            printf("%s%-16s : %s \n", space, "Item ID", "PDR_ACTION");
            break;

        case ITEM_ID__FAR_ACTION_FLAGS:
            printf("%s%-16s : %s \n", space, "Item ID", "FAR_ACTION_FLAGS");
            break;

        case ITEM_ID__FAR_DST_INTERFACE:
            printf("%s%-16s : %s \n", space, "Item ID", "FAR_DST_INTERFACE");
            break;

        ////////////////////////////////////////////////////////////////////

        case ITEM_ID__PDR_RULE_ID:
            printf("%s%-16s : %s \n", space, "Item ID", "PDR_RULE_ID");
            break;

        case ITEM_ID__PDR_PRECEDENCE:
            printf("%s%-16s : %s \n", space, "Item ID", "PDR_PRECEDENCE");
            break;

        case ITEM_ID__PDR_FAR_ID:
            printf("%s%-16s : %s \n", space, "Item ID", "PDR_FAR_ID");
            break;

        case ITEM_ID__FAR_RULE_ID:
            printf("%s%-16s : %s \n", space, "Item ID", "FAR_RULE_ID");
            break;


        ////////////////////////////////////////////////////////////////////

        case ITEM_ID__MCC_MNC:
            printf("%s%-16s : %s \n", space, "Item ID", "MCC_MNC");
            break;

        case ITEM_ID__TAC:
            printf("%s%-16s : %s \n", space, "Item ID", "TAC (Tracking Area Code)");
            break;

        case ITEM_ID__CELL_ID:
            printf("%s%-16s : %s \n", space, "Item ID", "Cell ID");
            break;

        case ITEM_ID__TMSI:
            printf("%s%-16s : %s \n", space, "Item ID", "Mobile-TMSI");
            break;

        case ITEM_ID__UE_IPV4_ADDR:
            printf("%s%-16s : %s \n", space, "Item ID", "UE_IPV4_ADDR");
            break;

        case ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR:
            printf("%s%-16s : %s \n", space, "Item ID", "PDR_PDI_MATCH_IPV4_ADDR");
            break;

        case ITEM_ID__PDR_PDI_MATCH_GTPU_TEID:
            printf("%s%-16s : %s \n", space, "Item ID", "PDR_PDI_MATCH_GTPU_TEID");
            break;

        ////////////////////////////////////////////////////////////////////

        case ITEM_ID__IMSI:
            printf("%s%-16s : %s \n", space, "Item ID", "IMSI");
            break;

        case ITEM_ID__GTPU_SELF_IPV4_ENDPOINT:
            printf("%s%-16s : %s \n", space, "Item ID", "GTPU_SELF_IPV4_ENDPOINT");
            break;

        case ITEM_ID__GTPU_PEER_IPV4_ENDPOINT:
            printf("%s%-16s : %s \n", space, "Item ID", "GTPU_PEER_IPV4_ENDPOINT");
            break;

        case ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE:
            printf("%s%-16s : %s \n", space, "Item ID", "FAR_OUTER_IPV4_HDR_CREATE");
            break;

        ////////////////////////////////////////////////////////////////////

        case ITEM_ID__UE_IDENTIFIER_SECRET:
            printf("%s%-16s : %s \n", space, "Item ID", "UE_IDENTIFIER_SECRET");
            break;

        case ITEM_ID__UE_IPV6_ADDR:
            printf("%s%-16s : %s \n", space, "Item ID", "UE_IPV6_ADDR");
            break;

        ////////////////////////////////////////////////////////////////////

        case ITEM_ID__GTPU_SELF_IPV6_ENDPOINT:
            printf("%s%-16s : %s \n", space, "Item ID", "GTPU_SELF_IPV6_ENDPOINT");
            break;

        case ITEM_ID__GTPU_PEER_IPV6_ENDPOINT:
            printf("%s%-16s : %s \n", space, "Item ID", "GTPU_PEER_IPV6_ENDPOINT");
            break;

        case ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE:
            printf("%s%-16s : %s \n", space, "Item ID", "FAR_OUTER_IPV6_HDR_CREATE");
            break;

        ////////////////////////////////////////////////////////////////////

        case ITEM_GROUP_ID__N3_PDR:
            printf("%s%-16s : %s \n", space, "Item Group ID", "N3_PDR");
            break;

        case ITEM_GROUP_ID__N6_PDR:
            printf("%s%-16s : %s \n", space, "Item Group ID", "N6_PDR");
            break;

        case ITEM_GROUP_ID__N3_FAR:
            printf("%s%-16s : %s \n", space, "Item Group ID", "N3_FAR");
            break;

        case ITEM_GROUP_ID__N6_FAR:
            printf("%s%-16s : %s \n", space, "Item Group ID", "N6_FAR");
            break;

        ////////////////////////////////////////////////////////////////////

        default:
            printf("%s%-16s : %s \n", space, "Item ID", "Unknown");
            break;
    }
}

static int
get_item_type(uint16_t item_id,
              uint8_t  *item_type)
{
    switch (item_id)
    {
        // 1 byte items
        case ITEM_ID__MSG_RESPONSE:
        case ITEM_ID__UPLINK_QOS_PROFILE:
        case ITEM_ID__DNLINK_QOS_PROFILE:
        case ITEM_ID__PDR_ACTION:
        case ITEM_ID__FAR_ACTION_FLAGS:
        case ITEM_ID__FAR_DST_INTERFACE:
            *item_type = ITEM_TYPE_IS_TYPE_1;
            return 0;

        // 2 byte items
        case ITEM_ID__PDR_RULE_ID:
        case ITEM_ID__PDR_PRECEDENCE:
        case ITEM_ID__PDR_FAR_ID:
        case ITEM_ID__FAR_RULE_ID:
            *item_type = ITEM_TYPE_IS_TYPE_1;
            return 0;

        // 4 byte items
        case ITEM_ID__MCC_MNC:
        case ITEM_ID__TAC:
        case ITEM_ID__CELL_ID:
        case ITEM_ID__TMSI:
        case ITEM_ID__UE_IPV4_ADDR:
        case ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR:
        case ITEM_ID__PDR_PDI_MATCH_GTPU_TEID:
            *item_type = ITEM_TYPE_IS_TYPE_1;
            return 0;

        // 8 byte items
        case ITEM_ID__IMSI:
        case ITEM_ID__GTPU_SELF_IPV4_ENDPOINT:
        case ITEM_ID__GTPU_PEER_IPV4_ENDPOINT:
        case ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE:
            *item_type = ITEM_TYPE_IS_TYPE_1;
            return 0;

        // 16 byte items
        case ITEM_ID__UE_IDENTIFIER_SECRET:
        case ITEM_ID__UE_IPV6_ADDR:
            *item_type = ITEM_TYPE_IS_TYPE_1;
            return 0;

        // more than 16 bytes (variable length)
        case ITEM_ID__GTPU_SELF_IPV6_ENDPOINT:
        case ITEM_ID__GTPU_PEER_IPV6_ENDPOINT:
        case ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE:
            *item_type = ITEM_TYPE_IS_TYPE_2;
            return 0;

        // item groups
        case ITEM_GROUP_ID__N3_PDR:
        case ITEM_GROUP_ID__N6_PDR:
        case ITEM_GROUP_ID__N3_FAR:
        case ITEM_GROUP_ID__N6_FAR:
            *item_type = ITEM_TYPE_IS_TYPE_3;
            return 0;

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

    switch (item_id)
    {
        /////////////////////////////////////////////////////////////////////

        case ITEM_ID__MSG_RESPONSE:
            nmp_msg_parsed_data_ptr->msg_response = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));

                if(MSG_RESPONSE_IS_OK == nmp_msg_parsed_data_ptr->msg_response)
                {
                    printf("%s%-32s : Ok \n", space, "Message Response");
                }
                else if(MSG_RESPONSE_IS_NOT_OK == nmp_msg_parsed_data_ptr->msg_response)
                {
                    printf("%s%-32s :-> Not Ok \n", space, "Message Response");
                }
                else
                {
                    printf("%s%-32s :-> Unknown \n", space, "Message Response");
                }
            }
            return 1;

        case ITEM_ID__UPLINK_QOS_PROFILE:
            nmp_msg_parsed_data_ptr->ul_qos_profile = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));

                printf("%s%-32s :-> %u \n", space, "Uplink QoS Profile", nmp_msg_parsed_data_ptr->ul_qos_profile);
            }
            return 1;

        case ITEM_ID__DNLINK_QOS_PROFILE:
            nmp_msg_parsed_data_ptr->dl_qos_profile = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));

                printf("%s%-32s :-> %u \n", space, "Dnlink QoS Profile", nmp_msg_parsed_data_ptr->dl_qos_profile);
            }
            return 1;

        case ITEM_ID__PDR_ACTION:
            nmp_msg_parsed_data_ptr->pdr_action = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));

                printf("%s%-32s :-> %u \n", space, "PDR Action", nmp_msg_parsed_data_ptr->pdr_action);
            }
            return 1;

        case ITEM_ID__FAR_ACTION_FLAGS:
            nmp_msg_parsed_data_ptr->far_action_flags = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));

                printf("%s%-32s :-> 0x%02x \n", space, "FAR Action Flags", 
                        nmp_msg_parsed_data_ptr->far_action_flags);
            }
            return 1;

        case ITEM_ID__FAR_DST_INTERFACE:
            nmp_msg_parsed_data_ptr->far_dst_interface = *(ptr);

            if(debug_flag)
            {
                printf("%s%-16s : 1 byte (0x%02x) \n", space, "Item Value", *(ptr));

                get_far_dst_interface_string(nmp_msg_parsed_data_ptr->far_dst_interface, string);
                printf("%s%-32s :-> %s \n", space, "FAR Destination Interface", string);
            }
            return 1;

        /////////////////////////////////////////////////////////////////////

        case ITEM_ID__PDR_RULE_ID:
            nmp_msg_parsed_data_ptr->pdr_rule_id = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));

                printf("%s%-32s :-> %u \n", space, "PDR Rule ID", nmp_msg_parsed_data_ptr->pdr_rule_id);
            }
            return 2;

        case ITEM_ID__PDR_PRECEDENCE:
            nmp_msg_parsed_data_ptr->pdr_precedence = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));

                printf("%s%-32s :-> %u \n", space, "PDR Precedence", nmp_msg_parsed_data_ptr->pdr_precedence);
            }
            return 2;

        case ITEM_ID__PDR_FAR_ID:
            nmp_msg_parsed_data_ptr->pdr_far_id = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));

                printf("%s%-32s :-> %u \n", space, "PDR FAR Id", nmp_msg_parsed_data_ptr->pdr_far_id);
            }
            return 2;

        case ITEM_ID__FAR_RULE_ID:
            nmp_msg_parsed_data_ptr->far_rule_id = htons(*((uint16_t *)ptr));

            if(debug_flag)
            {
                printf("%s%-16s : 2 bytes (%02x%02x) \n", space, "Item Value", *(ptr), *(ptr + 1));

                printf("%s%-32s :-> %u \n", space, "FAR Rule Id", nmp_msg_parsed_data_ptr->far_rule_id);
            }
            return 2;

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
            return 4;

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
            return 8;

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
            return 16;

        /////////////////////////////////////////////////////////////////////

        default:
            return -1;
    }

    return -1;
}


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

    switch (item_id)
    {
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
                    printf("%02x", *(ptr + i));
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
                    printf("%02x", *(ptr + i));
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
                    printf("%02x", *(ptr + i));
                }
                printf(")\n");

                get_ipv6_addr_string(nmp_msg_parsed_data_ptr->far_outer_v6_hdr_create_addr.u8, string);
                printf("%s%-32s :-> %s \n", space, "FAR Outer Ipv6 Hdr Create (Addr)", string);
                printf("%s%-32s :-> 0x%x (%u) \n", space, "FAR Outer Ipv6 Hdr Create (TEID)",
                        nmp_msg_parsed_data_ptr->far_outer_v6_hdr_create_teid,
                        nmp_msg_parsed_data_ptr->far_outer_v6_hdr_create_teid);

            }
            return (2 + item_len);

        default:
            return -1;
    }

    return -1;
}







static int
get_item_group_values(uint8_t        *ptr,
                      uint16_t        item_count,
                      uint16_t        item_len,
                      nmp_msg_data_t *nmp_msg_parsed_data_ptr,
                      uint8_t         debug_flag)
{
    int i = 0;
    int ret = 0;
    int offset = 0;
    uint16_t item_id = 0;
    uint8_t item_type = 0;
    char *space = "    ";

    if(debug_flag)
    {
        printf("%-16s : %u \n", "Item Count", item_count);
        printf("%-16s : %u \n", "Item Length", item_len);
        printf("--------------------------------------------\n");	
    }

    offset = 0;
    for(i = 0; i < item_count; i++)
    {
        item_id = htons(*((uint16_t *)(ptr + offset)));

        if(debug_flag) dump_item_id(space, item_id);

        if(-1 == get_item_type(item_id, &item_type))
        {
            return -1;
        }

        if(ITEM_TYPE_IS_TYPE_1 == item_type)
        {
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
        else
        {
            printf("Item Group contains type3 item... invalid case \n");
            return -1;
        }

        if(debug_flag)
        {
            if(i < (item_count - 1)) printf("\n");
        }
    }
    if(debug_flag) printf("--------------------------------------------\n");

    if(offset != item_len)
    {
        printf("Length mismatch found in parsing...(offset = %u, item_len = %u) \n", offset, item_len);
        return -1;
    }

    return offset;	
}

		 		

static int
get_item_group_value(uint16_t        item_id,
                     uint8_t        *ptr,
                     nmp_msg_data_t *nmp_msg_parsed_data_ptr,
                     uint8_t         debug_flag)
{
    int ret = 0;
    int offset = 0;
    uint16_t item_count = htons(*((uint16_t *)ptr));
    uint16_t item_len   = htons(*((uint16_t *)(ptr + 2)));

    offset = 4;
    switch (item_id)
    {
        case ITEM_GROUP_ID__N3_PDR:
        case ITEM_GROUP_ID__N6_PDR:
        case ITEM_GROUP_ID__N3_FAR:
        case ITEM_GROUP_ID__N6_FAR:
            ret = get_item_group_values(ptr + offset, 
                    item_count, 
                    item_len, 
                    nmp_msg_parsed_data_ptr, 
                    debug_flag);
            if(-1 == ret)
            {
                return -1;
            }
            return (4 + ret);

        default:
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
    nmp_hdr_t *nmp_hdr_ptr = NULL;
    uint8_t *nmp_msg_ptr = NULL;
    uint16_t *item_id_ptr = NULL;
    uint16_t  item_id = 0;
    uint8_t item_type = 0;
    uint16_t parsed_item_count = 0;
    char *space = "";

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

    if(debug_flag)
    {
        printf("\n\n");
        printf("--------------------------------------------------------------------\n");
        dump_src_to_dst_node_data(nmp_msg_parsed_data_ptr->src_node_type,
                nmp_msg_parsed_data_ptr->dst_node_type,
                nmp_msg_parsed_data_ptr->src_node_id,
                nmp_msg_parsed_data_ptr->dst_node_id);
        printf("--------------------------------------------------------------------\n");
        dump_msg_type(nmp_msg_parsed_data_ptr->msg_type);
        printf("--------------------------------------------------------------------\n");
        printf("%-16s : 0x%08x (%u) \n", "Msg Identifier", 
                nmp_msg_parsed_data_ptr->msg_identifier, 
                nmp_msg_parsed_data_ptr->msg_identifier);  
        printf("--------------------------------------------------------------------\n");
        printf("%-16s : %u  \n", "Item Count", nmp_msg_parsed_data_ptr->msg_item_count);
        printf("%-16s : %u bytes \n", "Item length", nmp_msg_parsed_data_ptr->msg_item_len);
        printf("--------------------------------------------------------------------\n");
    }

    nmp_msg_ptr = (uint8_t *)(nmp_hdr_ptr + 1);

    offset = 0;

    while((offset < nmp_msg_parsed_data_ptr->msg_item_len) && 
            (parsed_item_count < nmp_msg_parsed_data_ptr->msg_item_count))
    {	
        item_id_ptr = (uint16_t *)(nmp_msg_ptr + offset);
        item_id = htons(*(item_id_ptr));

        if(-1 == get_item_type(item_id, &item_type))
        {
            return -1;
        }

        if(debug_flag) 
        {
            if(ITEM_TYPE_IS_TYPE_3 == item_type) printf("--------------------------------------------\n");	
            dump_item_id(space, item_id);
        }

        if(ITEM_TYPE_IS_TYPE_1 == item_type)
        {
            if(debug_flag) printf("%-16s : Type 1 (Fixed Length) \n", "Item Type");

            ret = get_type1_item_value(space,
                    item_id, 
                    nmp_msg_ptr + offset + 2, 
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
            if(debug_flag) printf("%-16s : Type 2 (Variable length) \n", "Item Type");

            ret = get_type2_item_value(space,
                    item_id,
                    nmp_msg_ptr + offset + 2,
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
            if(debug_flag) printf("%-16s : Type 3 (Item Group) \n", "Item Type");

            ret = get_item_group_value(item_id,
                    nmp_msg_ptr + offset + 2,
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
        printf("--------------------------------------------------------------------\n\n\n");
    }

    return 0;
}

