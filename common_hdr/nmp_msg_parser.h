#ifndef NMP_MSG_PARSER_H
#define NMP_MSG_PARSER_H

typedef struct {
    uint32_t   v4_addr;
    uint32_t   teid;
} v4_teid_endpoint_t;

typedef struct {
    ipv6_addr_t  v6_addr;
    uint32_t     teid;
} v6_teid_endpoint_t;


struct nmp_msg_data {
    uint16_t  src_node_type;
    uint16_t  dst_node_type;
    uint16_t  src_node_id;
    uint16_t  dst_node_id;
    uint16_t  msg_type;
    uint16_t  msg_item_len;
    uint32_t  msg_identifier;
    uint16_t  msg_item_count;
    uint16_t  enb_index;
    uint8_t   msg_response;

    /////////////////////
    /////////////////////
    uint16_t  mcc;
    uint16_t  mnc;
    uint32_t  tac;
    uint32_t  cell_id;
    uint32_t  tmsi;
    data_64bit_t  imsi;
    uint8_t   ue_identifier_secret[16];

    uint8_t   ul_qos_profile;
    uint8_t   dl_qos_profile;
    uint8_t   pdr_action;
    uint8_t   far_action_flags;
    uint8_t   far_dst_interface;

    uint16_t  pdr_rule_id;
    uint16_t  pdr_precedence;
    uint32_t  pdr_pdi_match_ipv4_addr;
    uint32_t  pdr_pdi_match_gtpu_teid;
    uint16_t  pdr_far_id;
    uint16_t  far_rule_id;
    uint32_t  far_outer_v4_hdr_create_addr;
    uint32_t  far_outer_v4_hdr_create_teid;
    ipv6_addr_t far_outer_v6_hdr_create_addr;
    uint32_t    far_outer_v6_hdr_create_teid;

    ip_addr_t  ue_ip_addr;

    v4_teid_endpoint_t  self_v4_endpoint;
    v4_teid_endpoint_t  peer_v4_endpoint;

    v6_teid_endpoint_t  self_v6_endpoint;
    v6_teid_endpoint_t  peer_v6_endpoint;
    /////////////////////

} __attribute__((packed));
typedef struct nmp_msg_data  nmp_msg_data_t;


int
parse_nmp_msg(uint8_t         *msg_ptr,
              int              buf_len,
              nmp_msg_data_t  *nmp_msg_parsed_data_ptr,
              uint8_t          debug_flag);

#endif
