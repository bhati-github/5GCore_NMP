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


///////////////////////////////////////////
// 1 byte Items
///////////////////////////////////////////
int
nmp_add_item__msg_response(uint8_t *ptr,
                           uint8_t  response)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__MSG_RESPONSE);
    *(ptr + 2) = response; 
    return (2 + 1);
}

int
nmp_add_item__pdr__action(uint8_t *ptr,
                          uint8_t  action)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__PDR_ACTION);
    *(ptr + 2) = action;
    return (2 + 1);
}

int
nmp_add_item__far__action_flags(uint8_t *ptr,
                                uint8_t  action_flags)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__FAR_ACTION_FLAGS);
    *(ptr + 2) = action_flags;
    return (2 + 1);
}

int
nmp_add_item__far__dst_interface(uint8_t *ptr,
                                 uint8_t  dst_interface)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__FAR_DST_INTERFACE);
    *(ptr + 2) = dst_interface;
    return (2 + 1);
}

int
nmp_add_item__uplink_qos_profile(uint8_t *ptr,
                                 uint8_t  uplink_qos_profile)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__UPLINK_QOS_PROFILE);
    *(ptr + 2) = uplink_qos_profile;
    return (2 + 1);
}

int
nmp_add_item__dnlink_qos_profile(uint8_t *ptr,
                                 uint8_t  dnlink_qos_profile)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__DNLINK_QOS_PROFILE);
    *(ptr + 2) = dnlink_qos_profile;
    return (2 + 1);
}

int
nmp_add_item__default_paging_drx(uint8_t *ptr,
                                 uint8_t  default_paging_drx)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__DEFAULT_PAGING_DRX);
    *(ptr + 2) = default_paging_drx;
    return (2 + 1);
}

int
nmp_add_item__relative_amf_capacity(uint8_t *ptr,
                                    uint8_t  relative_amf_capacity)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__RELATIVE_AMF_CAPACITY);
    *(ptr + 2) = relative_amf_capacity;
    return (2 + 1);
}


///////////////////////////////////////////
// 2 byte Items
///////////////////////////////////////////
int
nmp_add_item__ran_ue_ngap_id(uint8_t *ptr,
                             uint16_t ran_ue_ngap_id)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__RAN_UE_NGAP_ID);
    *((uint16_t *)(ptr + 2)) = htons(ran_ue_ngap_id);
    return (2 + 2);
}

int
nmp_add_item__amf_ue_ngap_id(uint8_t *ptr,
                             uint16_t amf_ue_ngap_id)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__AMF_UE_NGAP_ID);
    *((uint16_t *)(ptr + 2)) = htons(amf_ue_ngap_id);
    return (2 + 2);
}

int
nmp_add_item__pdr__rule_id(uint8_t *ptr,
                           uint16_t rule_id)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__PDR_RULE_ID);
    *((uint16_t *)(ptr + 2)) = htons(rule_id);
    return (2 + 2);
}

int
nmp_add_item__pdr__precedence(uint8_t *ptr,
                              uint16_t precedence)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__PDR_PRECEDENCE);
    *((uint16_t *)(ptr + 2)) = htons(precedence);
    return (2 + 2);
}

int
nmp_add_item__pdr__far_id(uint8_t  *ptr,
                          uint16_t  far_id)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__PDR_FAR_ID);
    *((uint16_t *)(ptr + 2)) = htons(far_id);
    return (2 + 2);
}

int
nmp_add_item__far__rule_id(uint8_t *ptr,
                           uint16_t rule_id)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__FAR_RULE_ID);
    *((uint16_t *)(ptr + 2)) = htons(rule_id);
    return (2 + 2);
}

int
nmp_add_item__rrc_establish_cause(uint8_t *ptr,
                                  uint16_t rrc_establish_cause)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__RRC_ESTABLISH_CAUSE);
    *((uint16_t *)(ptr + 2)) = htons(rrc_establish_cause);
    return (2 + 2);
}


///////////////////////////////////////////
// 4 byte Items
///////////////////////////////////////////
int
nmp_add_item__mcc_mnc(uint8_t  *ptr,
                      uint16_t  mcc,
                      uint16_t  mnc)
{
    *((uint16_t *)(ptr))     = htons(ITEM_ID__MCC_MNC); // Also known as PLMN Identity
    *((uint16_t *)(ptr + 2)) = htons(mcc);
    *((uint16_t *)(ptr + 4)) = htons(mnc);
    return (2 + 4);
}

int
nmp_add_item__gnodeb_id(uint8_t  *ptr,
                        uint32_t  gnodeb_id)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__GNODEB_ID);
    *((uint32_t *)(ptr + 2)) = htonl(gnodeb_id);
    return (2 + 4);
}

int
nmp_add_item__tac(uint8_t  *ptr,
                  uint32_t  tac)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__TAC);
    *((uint32_t *)(ptr + 2)) = htonl(tac);
    return (2 + 4);
}

int
nmp_add_item__cell_id(uint8_t  *ptr,
                      uint32_t  cell_id)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__CELL_ID);
    *((uint32_t *)(ptr + 2)) = htonl(cell_id);
    return (2 + 4);
}

int
nmp_add_item__tmsi(uint8_t  *ptr,
                   uint32_t  tmsi)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__TMSI);
    *((uint32_t *)(ptr + 2)) = htonl(tmsi);
    return (2 + 4);
}

int
nmp_add_item__ue_ipv4_addr(uint8_t  *ptr,
                           uint32_t  ue_ipv4_addr)
{
    *((uint16_t *)(ptr))     = htons(ITEM_ID__UE_IPV4_ADDR);
    *((uint32_t *)(ptr + 2)) = htonl(ue_ipv4_addr);
    return (2 + 4);
}

int
nmp_add_item__pdr__pdi_match_dst_v4_addr(uint8_t *ptr,
                                         uint32_t ipv4_addr)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR);
    *((uint32_t *)(ptr + 2)) = htonl(ipv4_addr);
    return (2 + 4);
}

int
nmp_add_item__pdr__pdi_match_gtpu_teid(uint8_t *ptr,
                                       uint32_t gtpu_teid)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__PDR_PDI_MATCH_GTPU_TEID);
    *((uint32_t *)(ptr + 2)) = htonl(gtpu_teid);
    return (2 + 4);
}



///////////////////////////////////////////
// More than 4 and upto 8 byte Items
///////////////////////////////////////////
int
nmp_add_item__imsi(uint8_t      *ptr,
                   data_64bit_t  imsi)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__IMSI);
    memcpy(ptr + 2, imsi.u8, 8);
    return (2 + 8);
}

int
nmp_add_item__gtpu_self_ipv4_endpoint(uint8_t  *ptr,
                                      uint32_t  tunnel_ipv4_addr,
                                      uint32_t  teid)
{
    *((uint16_t *)(ptr))     = htons(ITEM_ID__GTPU_SELF_IPV4_ENDPOINT);
    *((uint32_t *)(ptr + 2)) = htonl(tunnel_ipv4_addr);
    *((uint32_t *)(ptr + 6)) = htonl(teid);
    return (2 + 8);
}

int
nmp_add_item__gtpu_peer_ipv4_endpoint(uint8_t  *ptr,
                                      uint32_t  tunnel_ipv4_addr,
                                      uint32_t  teid)
{
    *((uint16_t *)(ptr))     = htons(ITEM_ID__GTPU_PEER_IPV4_ENDPOINT);
    *((uint32_t *)(ptr + 2)) = htonl(tunnel_ipv4_addr);
    *((uint32_t *)(ptr + 6)) = htonl(teid);
    return (2 + 8);
}

int
nmp_add_item__far__outer_hdr_create(uint8_t *ptr,
                                    uint32_t v4_addr,
                                    uint32_t teid)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE);
    *((uint32_t *)(ptr + 2)) = htonl(v4_addr);
    *((uint32_t *)(ptr + 6)) = htonl(teid);
    return (2 + 8);
}

int
nmp_add_item__user_location_info_tai(uint8_t *ptr,
                                     uint16_t mcc,
                                     uint16_t mnc,
                                     uint32_t tac)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__USER_LOCATION_INFO_TAC);
    *((uint16_t *)(ptr + 2)) = htons(mcc);
    *((uint16_t *)(ptr + 4)) = htons(mnc);
    *((uint32_t *)(ptr + 6)) = htonl(tac);
    return (2 + 8);
}

int
nmp_add_item__slice_support_item(uint8_t *ptr,
                                 uint8_t  sst,
                                 uint32_t sd)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__NSSAI);
    *(ptr + 2) = sst;
    *((uint32_t *)(ptr + 3)) = htonl(sd);
    // Fill rest of the 3 bytes as zeroes...
    *(ptr + 7) = 0x0;
    *(ptr + 8) = 0x0;
    *(ptr + 9) = 0x0;
    return (2 + 8);
}


///////////////////////////////////////////
// More than 8 and upto 16 byte Items
///////////////////////////////////////////
int
nmp_add_item__ue_identifier_secret(uint8_t *ptr,
                                   uint8_t *secret)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__UE_IDENTIFIER_SECRET);
    memcpy(ptr + 2, secret, 16);
    return (2 + 16);
}

int
nmp_add_item__ue_ipv6_addr(uint8_t *ptr,
                           uint8_t *ipv6_addr)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__UE_IPV6_ADDR);
    memcpy(ptr + 2, ipv6_addr, 16);
    return (2 + 16);
}

int
nmp_add_item__user_location_info_nr_cgi(uint8_t *ptr,
                                        uint16_t mcc,
                                        uint16_t mnc,
                                        data_64bit_t nr_cell_identity)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__USER_LOCATION_INFO_NR_CGI);
    *((uint16_t *)(ptr + 2)) = htons(mcc);
    *((uint16_t *)(ptr + 4)) = htons(mnc);
    *((uint32_t *)(ptr + 6)) = 0x0;
    memcpy(ptr + 10, nr_cell_identity.u8, 8);
    return (2 + 16);
}



//////////////////////////////////////////////////
// Data more than 16 bytes (variable lenth items)
//////////////////////////////////////////////////
int
nmp_add_item__nas_pdu(uint8_t *ptr,
                      uint8_t *nas_pdu_ptr,
                      uint16_t nas_pdu_len)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__NAS_PDU);
    *((uint16_t *)(ptr + 2)) = htons(nas_pdu_len);
    memcpy(ptr + 2 + 2, nas_pdu_ptr, nas_pdu_len);
    // 2 bytes of item-id + 2 bytes of item-len + actual bytes of item value
    return (2 + 2 + nas_pdu_len); 
}

int
nmp_add_item__ran_node_name(uint8_t *ptr,
                            uint8_t *ran_node_name,
                            uint16_t ran_node_name_len)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__RAN_NODE_NAME);
    *((uint16_t *)(ptr + 2)) = htons(ran_node_name_len);
    memcpy(ptr + 2 + 2, ran_node_name, ran_node_name_len);
    // 2 bytes of item-id + 2 bytes of item-len + actual bytes of item value
    return (2 + 2 + ran_node_name_len);
}

int
nmp_add_item__amf_name(uint8_t *ptr,
                       uint8_t *amf_name,
                       uint16_t amf_name_len)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__AMF_NAME);
    *((uint16_t *)(ptr + 2)) = htons(amf_name_len);
    memcpy(ptr + 2 + 2, amf_name, amf_name_len);
    // 2 bytes of item-id + 2 bytes of item-len + actual bytes of item value
    return (2 + 2 + amf_name_len);
}

int
nmp_add_item__guami(uint8_t  *ptr,
                    uint8_t  *guami_item_ptr,
                    uint16_t  guami_item_len)
{
    *((uint16_t *)(ptr)) = htons(ITEM_ID__GUAMI);
    *((uint16_t *)(ptr + 2)) = htons(guami_item_len);
    memcpy(ptr + 2 + 2, guami_item_ptr, guami_item_len);
    // 2 bytes of item-id + 2 bytes of item-len + actual bytes of item value
    return (2 + 2 + guami_item_len);
}

 
