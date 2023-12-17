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
    uint16_t  msg_response_code;
    uint16_t  gnb_index;
    uint32_t  fiveg_service_addr;
    uint16_t  fiveg_service_port;

    uint16_t  mcc;
    uint16_t  mnc;
    uint32_t  tac;
    uint32_t  cell_id;
    uint32_t  tmsi;
    data_64bit_t  imsi;
    uint8_t   ue_identifier_secret[16];
    uint8_t   user_loc_info_nr_cell_identity[8];
    uint8_t   nas_pdu[128];
    uint8_t   ran_node_name[256];
    uint8_t   amf_name[256];

    uint8_t   default_paging_drx;
    uint8_t   relative_amf_capacity;
    uint8_t   nssai_sst;
    uint8_t   ul_qos_profile;
    uint8_t   dl_qos_profile;
    uint8_t   pdr_action;
    uint8_t   far_action_flags;
    uint8_t   far_dst_interface;

    uint16_t  ran_ue_ngap_id;
    uint16_t  amf_ue_ngap_id;
    uint16_t  guami_mcc;
    uint16_t  guami_mnc;
    uint16_t  guami_amf_region_id;
    uint16_t  guami_amf_set_id;
    uint16_t  guami_amf_pointer;
    uint16_t  rrc_establish_cause;
    uint16_t  user_loc_info_mcc;
    uint16_t  user_loc_info_mnc;
    uint32_t  user_loc_info_tac;
    uint32_t  nssai_sd;
    uint32_t  gnodeb_id;
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

    uint8_t  nrf_registration_req_data[2048];
    uint16_t nrf_registration_req_data_len;
   
    uint8_t  json_data[2048]; 
    uint8_t  msg_response_description[2048];

} __attribute__((packed));

typedef struct nmp_msg_data  nmp_msg_data_t;

void
get_node_type_string(uint16_t node_type, 
                     char    *string);

int
parse_nmp_msg(uint8_t         *msg_ptr,
              int              buf_len,
              nmp_msg_data_t  *nmp_msg_parsed_data_ptr,
              uint8_t          debug_flag);

#endif
