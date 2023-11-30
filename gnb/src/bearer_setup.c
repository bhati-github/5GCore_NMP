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

#include "gnb.h"
#include "n1_n2_msg_handler.h"
#include "bearer_setup.h"

int
process_bearer_setup_request_msg(nmp_msg_data_t *nmp_n1_rcvd_msg_data_ptr,
                                 uint8_t         debug_flag)
{
    char string[128];

    printf("%s: Handover: Process UE packets with these parameters... \n", __func__);

    get_ipv4_addr_string(nmp_n1_rcvd_msg_data_ptr->ue_ip_addr.u.v4_addr, string); 
    printf("UE IPv4 Addr = %s \n", string);
    printf("UE IMSI      = %02x%02x%02x%02x%02x%02x%02x%02x \n", 
            nmp_n1_rcvd_msg_data_ptr->imsi.u8[0],
            nmp_n1_rcvd_msg_data_ptr->imsi.u8[1],
            nmp_n1_rcvd_msg_data_ptr->imsi.u8[2],
            nmp_n1_rcvd_msg_data_ptr->imsi.u8[3],
            nmp_n1_rcvd_msg_data_ptr->imsi.u8[4],
            nmp_n1_rcvd_msg_data_ptr->imsi.u8[5],
            nmp_n1_rcvd_msg_data_ptr->imsi.u8[6],
            nmp_n1_rcvd_msg_data_ptr->imsi.u8[7]);

    get_ipv4_addr_string(nmp_n1_rcvd_msg_data_ptr->self_v4_endpoint.v4_addr, string);
    printf("Self GTP-U endpoint (Addr) = %s \n", string);

    printf("Self GTP-U endpoint (TEID) = 0x%x (%u) \n", 
            nmp_n1_rcvd_msg_data_ptr->self_v4_endpoint.teid, 
            nmp_n1_rcvd_msg_data_ptr->self_v4_endpoint.teid);


    get_ipv4_addr_string(nmp_n1_rcvd_msg_data_ptr->peer_v4_endpoint.v4_addr, string);
    printf("Peer GTP-U endpoint (Addr) = %s \n", string);

    printf("Peer GTP-U endpoint (TEID) = 0x%x (%u) \n", 
            nmp_n1_rcvd_msg_data_ptr->peer_v4_endpoint.teid, 
            nmp_n1_rcvd_msg_data_ptr->peer_v4_endpoint.teid);

    return 0;
}

