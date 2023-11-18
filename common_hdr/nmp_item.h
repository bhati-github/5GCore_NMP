#ifndef NMP_ITEM_H
#define NMP_ITEM_H

#define ITEM_TYPE_IS_TYPE_1  1
#define ITEM_TYPE_IS_TYPE_2  2
#define ITEM_TYPE_IS_TYPE_3  3

enum item_id_t {
    ////////////////////////////////////////////////////////
    // Item id's carrying 1 byte value
    ////////////////////////////////////////////////////////
    ITEM_ID__MSG_RESPONSE,
    ITEM_ID__UPLINK_QOS_PROFILE,
    ITEM_ID__DNLINK_QOS_PROFILE,
    ITEM_ID__PDR_ACTION,
    ITEM_ID__FAR_ACTION_FLAGS,
    ITEM_ID__FAR_DST_INTERFACE,

    ////////////////////////////////////////////////////////
    // Item id's carrying 2 byte value
    ////////////////////////////////////////////////////////
    ITEM_ID__RAN_UE_NGAP_ID,
    ITEM_ID__AMF_UE_NGAP_ID,
    ITEM_ID__PDR_RULE_ID,
    ITEM_ID__PDR_PRECEDENCE,
    ITEM_ID__PDR_FAR_ID,
    ITEM_ID__FAR_RULE_ID,
    ITEM_ID__RRC_ESTABLISH_CAUSE,

    ////////////////////////////////////////////////////////
    // Item id's carrying more than 2 and upto 4 byte value
    ////////////////////////////////////////////////////////
    ITEM_ID__MCC_MNC,
    ITEM_ID__TAC,
    ITEM_ID__CELL_ID,
    ITEM_ID__TMSI,
    ITEM_ID__UE_IPV4_ADDR,
    ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR,
    ITEM_ID__PDR_PDI_MATCH_GTPU_TEID,

    ////////////////////////////////////////////////////////
    // Item id's carrying more than 4 and upto 8 byte value
    ////////////////////////////////////////////////////////
    ITEM_ID__IMSI,
    ITEM_ID__GTPU_SELF_IPV4_ENDPOINT,
    ITEM_ID__GTPU_PEER_IPV4_ENDPOINT,
    ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE,
    ITEM_ID__USER_LOCATION_INFO_TAC,

    ////////////////////////////////////////////////////////
    // Item id's carrying more than 8 and upto 16 byte value
    ////////////////////////////////////////////////////////
    ITEM_ID__UE_IDENTIFIER_SECRET,
    ITEM_ID__UE_IPV6_ADDR,
    ITEM_ID__USER_LOCATION_INFO_NR_CGI,

    ////////////////////////////////////////////////////////
    // Also known as Type-2 items.
    // Item id's carrying more than 16 byte value (variable length)
    // 2 bytes(item_id)  +
    // 2 bytes(item_len) +
    // actual item bytes
    ////////////////////////////////////////////////////////
    ITEM_ID__GTPU_SELF_IPV6_ENDPOINT,
    ITEM_ID__GTPU_PEER_IPV6_ENDPOINT,
    ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE,
    ITEM_ID__NAS_PDU,

    ////////////////////////////////////////////////////////
    // Item id's carrying group of individual items
    // 2 bytes(group_item_id) +
    // 2 bytes(item_count)    +
    // 2 bytes(item_len)      +
    // actual item bytes
    ////////////////////////////////////////////////////////
    ITEM_GROUP_ID__N3_PDR,
    ITEM_GROUP_ID__N6_PDR,
    ITEM_GROUP_ID__N3_FAR,
    ITEM_GROUP_ID__N6_FAR,
    ITEM_GROUP_ID__USER_LOCATION_INFO,

};



////////////////////////////////////////////////////////
// Add items which carry only 1 byte value
////////////////////////////////////////////////////////
int
nmp_add_item__msg_response(uint8_t *ptr,
                           uint8_t  response);

int
nmp_add_item__pdr__action(uint8_t *ptr,
                          uint8_t  action);

int
nmp_add_item__far__action_flags(uint8_t *ptr,
                                uint8_t  action_flags);

int
nmp_add_item__far__dst_interface(uint8_t *ptr,
                                 uint8_t  dst_interface);

int
nmp_add_item__uplink_qos_profile(uint8_t *ptr,
                                 uint8_t  uplink_qos_profile);

int
nmp_add_item__dnlink_qos_profile(uint8_t *ptr,
                                 uint8_t  dnlink_qos_profile);


////////////////////////////////////////////////////////
// Add items which carry 2 byte values
////////////////////////////////////////////////////////
int
nmp_add_item__ran_ue_ngap_id(uint8_t *ptr,
                             uint16_t ran_ue_ngap_id);

int
nmp_add_item__amf_ue_ngap_id(uint8_t *ptr,
                             uint16_t amf_ue_ngap_id);

int
nmp_add_item__pdr__rule_id(uint8_t *ptr,
                           uint16_t rule_id);

int
nmp_add_item__pdr__precedence(uint8_t *ptr,
                              uint16_t precedence);

int
nmp_add_item__pdr__far_id(uint8_t  *ptr,
                          uint16_t  far_id);

int
nmp_add_item__far__rule_id(uint8_t *ptr,
                           uint16_t rule_id);

int
nmp_add_item__rrc_establish_cause(uint8_t *ptr,
                                  uint16_t rrc_establish_cause);

///////////////////////////////////////////////////////////
// Add items which carry more than 2 and upto 4 byte value
///////////////////////////////////////////////////////////
int
nmp_add_item__mcc_mnc(uint8_t  *ptr,
                      uint16_t  mcc,
                      uint16_t  mnc);

int
nmp_add_item__tac(uint8_t  *ptr,
                  uint32_t  tac);

int
nmp_add_item__cell_id(uint8_t  *ptr,
                      uint32_t  cell_id);

int
nmp_add_item__tmsi(uint8_t  *ptr,
                   uint32_t  tmsi);

int
nmp_add_item__ue_ipv4_addr(uint8_t  *ptr,
                           uint32_t  ue_ipv4_addr);

int
nmp_add_item__pdr__pdi_match_dst_v4_addr(uint8_t *ptr,
                                         uint32_t ipv4_addr);

int
nmp_add_item__pdr__pdi_match_gtpu_teid(uint8_t *ptr,
                                       uint32_t gtpu_teid);

///////////////////////////////////////////////////////////
// Add items which carry more than 4 and upto 8 byte value
///////////////////////////////////////////////////////////
int
nmp_add_item__imsi(uint8_t      *ptr,
                   data_64bit_t  imsi);

int
nmp_add_item__gtpu_self_ipv4_endpoint(uint8_t  *ptr,
                                      uint32_t  tunnel_ipv4_addr,
                                      uint32_t  teid);

int
nmp_add_item__gtpu_peer_ipv4_endpoint(uint8_t  *ptr,
                                      uint32_t  tunnel_ipv4_addr,
                                      uint32_t  teid);

int
nmp_add_item__far__outer_hdr_create(uint8_t *ptr,
                                    uint32_t v4_addr,
                                    uint32_t teid);

int
nmp_add_item__user_location_info_tai(uint8_t *ptr,
                                     uint16_t mcc,
                                     uint16_t mnc,
                                     uint32_t tac);

////////////////////////////////////////////////////////////
// Add items which carry more than 8 and upto 16 byte value
////////////////////////////////////////////////////////////
int
nmp_add_item__ue_identifier_secret(uint8_t *ptr,
                                   uint8_t *secret);

int
nmp_add_item__ue_ipv6_addr(uint8_t *ptr,
                           uint8_t *ipv6_addr);

int
nmp_add_item__user_location_info_nr_cgi(uint8_t *ptr,
                                        uint16_t mcc,
                                        uint16_t mnc,
                                        data_64bit_t nr_cell_identity);

/////////////////////////////////////////////////////////
// More than 16 byte Items (variable length byte stream)
/////////////////////////////////////////////////////////
int
nmp_add_item__nas_pdu(uint8_t *ptr,
                      uint8_t *nas_pdu_ptr,
                      uint16_t nas_pdu_len);


////////////////////////////
// Item Groups
////////////////////////////
int
nmp_add_item_group__user_location_info(uint8_t *ptr,
                                       uint16_t mcc,
                                       uint16_t mnc,
                                       data_64bit_t nr_cell_identity,
                                       uint32_t tac);

int
nmp_add_item_group__n3_pdr(uint8_t *ptr,
                           uint16_t rule_id,
                           uint16_t precedence,
                           uint32_t teid_v4_addr,
                           uint32_t teid,
                           uint8_t  pdr_action,
                           uint16_t pdr_far_id);

int
nmp_add_item_group__n6_pdr(uint8_t *ptr,
                           uint16_t rule_id,
                           uint16_t precedence,
                           uint32_t ue_v4_addr,
                           uint8_t  pdr_action,
                           uint16_t pdr_far_id);

int
nmp_add_item_group__n3_far(uint8_t *ptr,
                           uint16_t far_id,
                           uint8_t  action_flags,
                           uint8_t  dst_interface,
                           uint32_t teid_v4_addr,
                           uint32_t teid);

int
nmp_add_item_group__n6_far(uint8_t *ptr,
                           uint16_t far_id,
                           uint8_t  action_flags,
                           uint8_t  dst_interface);

#endif
