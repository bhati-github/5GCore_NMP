#ifndef NMP_H
#define NMP_H

// 20 bytes NMP header
struct nmp_hdr {
    uint16_t  src_node_type;
    uint16_t  dst_node_type;
    uint16_t  src_node_id;
    uint16_t  dst_node_id;
    uint16_t  msg_type;
    uint16_t  msg_item_count;
    uint16_t  msg_item_len;     
    uint16_t  rsvd;
    uint32_t  msg_identifier;  
} __attribute__((packed));

typedef struct nmp_hdr  nmp_hdr_t;

#endif
