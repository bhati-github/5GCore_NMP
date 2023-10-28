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
#include "nmp_msg_parser.h"
#include "common_util.h"

#include "amf.h"
#include "n1_msg_handler.h"
#include "ue_attach.h"
#include "upf_session.h"

int
validate_rcvd_msg_on_n1_interface(uint8_t *msg_ptr,
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

    if(request_identifier != htonl(nmp_hdr_ptr->msg_identifier))
    {
        printf("Response identifier is not matching with Request identifier \n");
        return -1;
    }

    return 0;
}


int
get_gnb_index_from_v4_addr(uint32_t  gnb_n1_addr,
                           uint16_t *gnb_index)
{
    int i = 0;
    for(i = 0; i < g__amf_config.gnb_count; i++)
    {
        if(gnb_n1_addr == g__amf_config.gnb_data[i].gnb_n1_addr.u.v4_addr)
        {
            *gnb_index = i;
            return 0;
        }
    }

    return -1;
}


int
process_rcvd_n1_msg(nmp_msg_data_t *nmp_n1_rcvd_msg_data_ptr,
                    uint32_t        gnb_n1_addr,
                    uint8_t         debug_flag)
{
    char string[128];
    uint16_t gnb_index = 0;
    uint32_t ue_ipv4_addr = 0;
    uint32_t gnb_v4_addr = 0;
    uint32_t upf_v4_addr = 0;
    uint32_t ul_teid = 0;
    uint32_t dl_teid = 0;

    if(MSG_TYPE__UE_ATTACH_REQUEST == nmp_n1_rcvd_msg_data_ptr->msg_type)
    {
        if(-1 == get_gnb_index_from_v4_addr(gnb_n1_addr, &gnb_index))
        {
            get_ipv4_addr_string(gnb_n1_addr, string);
            printf("Unable to find a registered enodeb with ipv4 address %s \n", string);
            return -1;
        }
        nmp_n1_rcvd_msg_data_ptr->gnb_index = gnb_index;

        if(debug_flag) printf("%s: gnb_index = %u \n", __func__, gnb_index);

        // Allocate user ip
        ue_ipv4_addr = g__ue_ipv4_addr_base++;

        gnb_v4_addr = g__amf_config.gnb_data[gnb_index].gnb_n3_addr.u.v4_addr;
        upf_v4_addr = g__amf_config.upf_n3_addr.u.v4_addr;

        printf("%s: Send session create message to UPF \n", __func__);

        ///////////////////////////////////////		
        // Send msg to UPF to create a session
        ///////////////////////////////////////		
        if(-1 == send_session_create_msg_to_upf(ue_ipv4_addr,
                                                nmp_n1_rcvd_msg_data_ptr->imsi, 
                                                gnb_v4_addr, 
                                                upf_v4_addr,
                                                &(ul_teid),
                                                &(dl_teid),
                                                debug_flag))
        {
            return send_ue_attach_response_not_ok(nmp_n1_rcvd_msg_data_ptr, debug_flag);
        }

        return send_ue_attach_response_ok(nmp_n1_rcvd_msg_data_ptr,
                                          ue_ipv4_addr,
                                          gnb_v4_addr,
                                          upf_v4_addr,
                                          ul_teid,
                                          dl_teid,
                                          debug_flag);
    }
    else
    {
        printf("Unknown message... \n");
        return -1;
    }

    return 0;	
}


