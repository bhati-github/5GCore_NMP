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

#include "smf.h"
#include "upf_session.h"


static void
get_session_index_from_ue_ipv4_addr(uint32_t  ue_ipv4_addr,
                                    uint32_t *session_index)
{
    *session_index = (ue_ipv4_addr & 0xffff);
}


int
get_pdr_and_far_for_session_create(uint32_t  ue_ipv4_addr,
                                   uint32_t  upf_n3_iface_v4_addr,
                                   uint32_t *session_idx,
                                   uint32_t *ul_teid)
{
    uint32_t session_index = 0;

    get_session_index_from_ue_ipv4_addr(ue_ipv4_addr, &session_index);

    session_info_t *session_ptr = &(g__smf_config.upf_session_data[session_index]);

    // N3 PDR Information (detection of uplink packets)
    session_ptr->n3_pdr.rule_id    = N3_PDR_BASE;
    session_ptr->n3_pdr.precedence = 255;
    session_ptr->n3_pdr.pdi.src_interface = INTERFACE_TYPE_IS_N3;
    session_ptr->n3_pdr.pdi.f_teid.teid   = N3_UPLINK_TEID_BASE + session_index;
    session_ptr->n3_pdr.pdi.f_teid.ip_addr.ip_version = IP_VER_IS_V4;
    session_ptr->n3_pdr.pdi.f_teid.ip_addr.u.v4_addr  = upf_n3_iface_v4_addr;
    session_ptr->n3_pdr.action = 0x01;
    session_ptr->n3_pdr.far_id = N6_FAR_BASE;

    // N6 PDR information (detection of downlink packets)
    session_ptr->n6_pdr.rule_id    = N6_PDR_BASE;
    session_ptr->n6_pdr.precedence = 255;
    session_ptr->n6_pdr.pdi.src_interface = INTERFACE_TYPE_IS_N6;
    session_ptr->n6_pdr.pdi.ue_ip_addr.ip_version = IP_VER_IS_V4;
    session_ptr->n6_pdr.pdi.ue_ip_addr.u.v4_addr  = ue_ipv4_addr;
    session_ptr->n6_pdr.far_id = N3_FAR_BASE;

    // N6 FAR information 
    session_ptr->n6_far.far_id = session_ptr->n3_pdr.far_id;
    session_ptr->n6_far.action_flags = 0x02; // forward
    session_ptr->n6_far.forward_param.dst_interface = INTERFACE_TYPE_IS_N6;

    // N3 FAR information 
    session_ptr->n3_far.far_id = session_ptr->n6_pdr.far_id;
    session_ptr->n3_far.action_flags = 0x02; // gtp-encap and forward
    session_ptr->n3_far.forward_param.f_teid.teid = 0x0; // will be updated later in session modify message
    session_ptr->n3_far.forward_param.f_teid.ip_addr.ip_version = IP_VER_IS_V4;
    session_ptr->n3_far.forward_param.f_teid.ip_addr.u.v4_addr = 0x0; // will be updated later in session modify message

    *session_idx = session_index;
    *ul_teid = session_ptr->n3_pdr.pdi.f_teid.teid;

    return 0;
}


int
get_pdr_and_far_for_session_modify(uint32_t  ue_ipv4_addr,
                                   uint32_t  latest_gnb_n3_iface_v4_addr,
                                   uint32_t  teid_rcvd_from_gnb,
                                   uint32_t *session_idx)
{
    uint32_t session_index = 0;

    get_session_index_from_ue_ipv4_addr(ue_ipv4_addr, &session_index);

    session_info_t *session_ptr = &(g__smf_config.upf_session_data[session_index]);

    // N3 FAR information
    session_ptr->n3_far.forward_param.f_teid.teid = teid_rcvd_from_gnb;
    session_ptr->n3_far.forward_param.f_teid.ip_addr.u.v4_addr = latest_gnb_n3_iface_v4_addr;

    *session_idx = session_index;

    return 0;
}

