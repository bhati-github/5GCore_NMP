#ifndef NMP_ITEM_H
#define NMP_ITEM_H

#define ITEM_TYPE_IS_TYPE_1  1
#define ITEM_TYPE_IS_TYPE_2  2
#define ITEM_TYPE_IS_TYPE_3  3

enum item_id_t {
    //////////////////////////////////////////////
    // Item id's carrying 1 byte value
    //////////////////////////////////////////////
    ITEM_ID__MSG_RESPONSE,
    ITEM_ID__UPLINK_QOS_PROFILE,
    ITEM_ID__DNLINK_QOS_PROFILE,
    ITEM_ID__PDR_ACTION,
    ITEM_ID__FAR_ACTION_FLAGS,
    ITEM_ID__FAR_DST_INTERFACE,

    //////////////////////////////////////////////
    // Item id's carrying 2 byte value
    //////////////////////////////////////////////
    ITEM_ID__RAN_UE_NGAP_ID,
    ITEM_ID__AMF_UE_NGAP_ID,
    ITEM_ID__PDR_RULE_ID,
    ITEM_ID__PDR_PRECEDENCE,
    ITEM_ID__PDR_FAR_ID,
    ITEM_ID__FAR_RULE_ID,

    //////////////////////////////////////////////
    // Item id's carrying 4 byte value
    //////////////////////////////////////////////
    ITEM_ID__MCC_MNC,
    ITEM_ID__TAC,
    ITEM_ID__CELL_ID,
    ITEM_ID__TMSI,
    ITEM_ID__UE_IPV4_ADDR,
    ITEM_ID__PDR_PDI_MATCH_IPV4_ADDR,
    ITEM_ID__PDR_PDI_MATCH_GTPU_TEID,

    //////////////////////////////////////////////
    // Item id's carrying 8 byte value
    //////////////////////////////////////////////
    ITEM_ID__IMSI,
    ITEM_ID__GTPU_SELF_IPV4_ENDPOINT,
    ITEM_ID__GTPU_PEER_IPV4_ENDPOINT,
    ITEM_ID__FAR_OUTER_IPV4_HDR_CREATE,

    //////////////////////////////////////////////
    // Item id's carrying 16 byte value
    //////////////////////////////////////////////
    ITEM_ID__UE_IDENTIFIER_SECRET,
    ITEM_ID__UE_IPV6_ADDR,

    //////////////////////////////////////////////
    // Item id's carrying more than 16 byte value
    // 2 bytes(item_id)  +
    // 2 bytes(item_len) +
    // actual item bytes
    //////////////////////////////////////////////
    ITEM_ID__GTPU_SELF_IPV6_ENDPOINT,
    ITEM_ID__GTPU_PEER_IPV6_ENDPOINT,
    ITEM_ID__FAR_OUTER_IPV6_HDR_CREATE,

    //////////////////////////////////////////////
    // Item id's carrying group of individual items
    // 2 bytes(group_item_id) +
    // 2 bytes(item_count)    +
    // 2 bytes(item_len)      +
    // actual item bytes
    //////////////////////////////////////////////
    ITEM_GROUP_ID__N3_PDR,
    ITEM_GROUP_ID__N6_PDR,
    ITEM_GROUP_ID__N3_FAR,
    ITEM_GROUP_ID__N6_FAR,

};



///////////////////////////////////////////
// 1 byte Items
///////////////////////////////////////////
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


///////////////////////////////////////////
// 2 byte Items
///////////////////////////////////////////
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


///////////////////////////////////////////
// 4 byte Items
///////////////////////////////////////////
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

///////////////////////////////////////////
// 8 byte Items
///////////////////////////////////////////
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


///////////////////////////////////////////
// 16 byte Items
///////////////////////////////////////////
int
nmp_add_item__ue_identifier_secret(uint8_t *ptr,
                                   uint8_t *secret);

int
nmp_add_item__ue_ipv6_addr(uint8_t *ptr,
                           uint8_t *ipv6_addr);



#endif
