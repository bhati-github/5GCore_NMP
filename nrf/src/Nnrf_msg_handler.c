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
#include "nmp_item.h"
#include "nmp_msg_parser.h"
#include "common_util.h"

#include "nrf.h"
#include "Nnrf_msg_handler.h"


static int
nrf_get_target_service_sockaddr(nmp_msg_data_t     *Nnrf_rcvd_nmp_msg_data_ptr,
                                struct sockaddr_in *target_sockaddr)
{
    if(g__nrf_config.Namf_addr.u.v4_addr == Nnrf_rcvd_nmp_msg_data_ptr->fiveg_service_addr)
    {
        target_sockaddr->sin_addr.s_addr = g__nrf_config.Namf_sockaddr.sin_addr.s_addr;
        target_sockaddr->sin_port = g__nrf_config.Namf_sockaddr.sin_port;
    }
    else if(g__nrf_config.Nsmf_addr.u.v4_addr == Nnrf_rcvd_nmp_msg_data_ptr->fiveg_service_addr)
    {
        target_sockaddr->sin_addr.s_addr = g__nrf_config.Nsmf_sockaddr.sin_addr.s_addr;
        target_sockaddr->sin_port = g__nrf_config.Nsmf_sockaddr.sin_port;
    }
    else
    {
        printf("No matching socket addr..\n");
        return -1;
    }

    return 0;
}



int
validate_rcvd_msg_on_Nnrf_interface(uint8_t *msg_ptr,
                                    int      msg_len,
                                    uint32_t request_identifier)
{
    if(msg_len < sizeof(nmp_hdr_t))
    {
        printf("Invalid size of NMP message.. \n");
        return -1;
    }

    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)msg_ptr;

    if(NODE_TYPE__NRF != htons(nmp_hdr_ptr->dst_node_type))
    {
        printf("Destination Node of NMP message is not NRF \n");
        return -1;
    }

    if(g__nrf_config.my_id != htons(nmp_hdr_ptr->dst_node_id))
    {
        printf("Destination node id is not equal to my id \n");
        return -1;
    }

    if(0 != request_identifier)
    {
        if(request_identifier != htonl(nmp_hdr_ptr->msg_identifier))
        {
            printf("Response identifier is not matching with Request identifier \n");
            return -1;
        }
    }

    return 0;
}

int
process_service_registration_request_msg(nmp_msg_data_t *Nnrf_rcvd_nmp_msg_data_ptr,
                                         uint8_t         debug_flag)
{
    int n = 0;
    int ret = 0;
    int offset = 0;
    char time_string[128];
    char src_string[128];
    char response[256];
    uint16_t response_code = MSG_RESPONSE_CODE__OK;
    uint16_t item_count = 0;
    struct sockaddr_in  target_service_sockaddr;
    nmp_msg_data_t  nmp_Nnrf_send_msg_data;

    // Prepare a response and send Ok to fiveg service.
    uint8_t *ptr = g__Nnrf_send_msg_buffer;
    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;

    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__NRF);
    nmp_hdr_ptr->dst_node_type  = htons(Nnrf_rcvd_nmp_msg_data_ptr->src_node_type);
    nmp_hdr_ptr->src_node_id    = htons(g__nrf_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(Nnrf_rcvd_nmp_msg_data_ptr->src_node_id);

    nmp_hdr_ptr->msg_type  = htons(MSG_TYPE__NRF_SERVICE_REGISTRATION_RESP);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;
    
    nmp_hdr_ptr->msg_identifier = htonl(Nnrf_rcvd_nmp_msg_data_ptr->msg_identifier);
    offset = sizeof(nmp_hdr_t);
    
    // Store information about fiveg service into NRF repository..
    if(NODE_TYPE__AMF == Nnrf_rcvd_nmp_msg_data_ptr->src_node_type)
    {
        printf("\n");
        get_current_time(time_string);
        printf("[%s] NRF <---------- AMF [ NRF_SERVICE_REGISTRATION_REQ ] \n", time_string);

        memcpy(g__nrf_config.amf_service_info,
               Nnrf_rcvd_nmp_msg_data_ptr->nrf_registration_req_data,
               Nnrf_rcvd_nmp_msg_data_ptr->nrf_registration_req_data_len);
        response_code = MSG_RESPONSE_CODE__OK;
        g__nrf_config.amf_service_info_is_registered = 1;
        strcpy(response, "Service Information is Registered Successfully");
    }
    else if(NODE_TYPE__SMF == Nnrf_rcvd_nmp_msg_data_ptr->src_node_type)
    {
        printf("\n");
        get_current_time(time_string);
        printf("[%s] NRF <---------- SMF [ NRF_SERVICE_REGISTRATION_REQ ] \n", time_string);
        memcpy(g__nrf_config.smf_service_info,
               Nnrf_rcvd_nmp_msg_data_ptr->nrf_registration_req_data,
               Nnrf_rcvd_nmp_msg_data_ptr->nrf_registration_req_data_len);
        response_code = MSG_RESPONSE_CODE__OK;    
        g__nrf_config.smf_service_info_is_registered = 1;
        strcpy(response, "Service Information is Registered Successfully");
    }
    else
    {
        printf("%s: No placeholder to keep fiveg service information \n", __func__);
        response_code = MSG_RESPONSE_CODE__NOT_OK;
        strcpy(response, "Service Information Registration Failed");
    }
        
    // Item: Response code
    ret = nmp_add_item__msg_response_code(ptr + offset, response_code);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;
   

    // Item: Response Description
    ret = nmp_add_item__msg_response_description(ptr + offset, 
                                                (uint8_t *)response, 
                                                 strlen((char *)response));
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;


    nmp_hdr_ptr->msg_item_len   = htons(offset - sizeof(nmp_hdr_t));
    nmp_hdr_ptr->msg_item_count = htons(item_count);

    if(-1 == parse_nmp_msg(ptr,
                           offset,
                           &(nmp_Nnrf_send_msg_data),
                           debug_flag))
    {
        printf("%s: Msg parse error. \n", __func__);
        return -1;
    }

    ////////////////////////////////////////////////
    // Write this msg on Nnrf socket 
    ////////////////////////////////////////////////
    if(-1 == nrf_get_target_service_sockaddr(Nnrf_rcvd_nmp_msg_data_ptr, 
                                            &target_service_sockaddr)) 
    {
        printf("%s: Unable to get taget service socket addr \n", __func__);
        return -1;
    }

    n = sendto(g__nrf_config.Nnrf_socket_id,
               (char *)ptr,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));
    if(n != offset)
    {
        printf("%s: sendto() failed \n", __func__);
        return -1;
    }

    get_node_type_string(Nnrf_rcvd_nmp_msg_data_ptr->src_node_type, src_string);

    get_current_time(time_string);
    printf("[%s] NRF ----------> %s [ NRF_SERVICE_REGISTRATION_RESP ] \n", time_string, src_string);
    return 0;
}



int
process_service_discovery_request_msg(nmp_msg_data_t *Nnrf_rcvd_nmp_msg_data_ptr,
                                      uint8_t         debug_flag)
{
    int n = 0;
    int ret = 0;
    int offset = 0;
    char src_string[128];
    uint16_t item_count = 0;
    struct sockaddr_in  target_service_sockaddr;
    nmp_msg_data_t  nmp_Nnrf_send_msg_data;

    // Prepare a response and send Ok to fiveg service.
    uint8_t *ptr = g__Nnrf_send_msg_buffer;
    nmp_hdr_t *nmp_hdr_ptr = (nmp_hdr_t *)ptr;

    nmp_hdr_ptr->src_node_type  = htons(NODE_TYPE__NRF);
    nmp_hdr_ptr->dst_node_type  = htons(Nnrf_rcvd_nmp_msg_data_ptr->src_node_type);
    nmp_hdr_ptr->src_node_id    = htons(g__nrf_config.my_id);
    nmp_hdr_ptr->dst_node_id    = htons(Nnrf_rcvd_nmp_msg_data_ptr->src_node_id);

    nmp_hdr_ptr->msg_type  = htons(MSG_TYPE__NRF_SERVICE_DISCOVERY_RESP);
    nmp_hdr_ptr->msg_item_len   = 0;
    nmp_hdr_ptr->msg_item_count = 0;

    nmp_hdr_ptr->msg_identifier = htonl(Nnrf_rcvd_nmp_msg_data_ptr->msg_identifier);
    offset = sizeof(nmp_hdr_t);


    // Add AMF service Information as NMP item
    if(g__nrf_config.amf_service_info_is_registered)
    {
        ret = nmp_add_item__service_info_as_json_data(ptr + offset,
                                                     (uint8_t *)g__nrf_config.amf_service_info,
                                                     strlen((char *)g__nrf_config.amf_service_info));
        if(-1 == ret)
        {
            return -1;
        }
        offset += ret;
        item_count += 1;
    }

    // Add SMF service Information as NMP item
    if(g__nrf_config.smf_service_info_is_registered)
    {
        ret = nmp_add_item__service_info_as_json_data(ptr + offset,
                                                     (uint8_t *)g__nrf_config.smf_service_info,
                                                     strlen((char *)g__nrf_config.smf_service_info));
        if(-1 == ret)
        {
            return -1;
        }
        offset += ret;
        item_count += 1;
    }

    nmp_hdr_ptr->msg_item_len   = htons(offset - sizeof(nmp_hdr_t));
    nmp_hdr_ptr->msg_item_count = htons(item_count);

    if(-1 == parse_nmp_msg(ptr,
                           offset,
                           &(nmp_Nnrf_send_msg_data),
                           debug_flag))
    {
        printf("%s: Msg parse error. \n", __func__);
        return -1;
    }

    ////////////////////////////////////////////////
    // Write this msg on Nnrf socket
    ////////////////////////////////////////////////
    if(-1 == nrf_get_target_service_sockaddr(Nnrf_rcvd_nmp_msg_data_ptr,
                                            &target_service_sockaddr))
    {
        printf("%s: Unable to get taget service socket addr \n", __func__);
        return -1;
    }

    n = sendto(g__nrf_config.Nnrf_socket_id,
               (char *)ptr,
               offset,
               MSG_WAITALL,
               (struct sockaddr *)&(target_service_sockaddr),
               sizeof(struct sockaddr_in));
    if(n != offset)
    {
        printf("%s: sendto() failed \n", __func__);
        return -1;
    }

    get_node_type_string(Nnrf_rcvd_nmp_msg_data_ptr->src_node_type, src_string);
    printf("%s: Msg sent successfully to source node %s \n", __func__, src_string);
    return 0;
}


int
process_rcvd_Nnrf_msg(nmp_msg_data_t *Nnrf_rcvd_nmp_msg_data_ptr,
                      uint8_t         debug_flag)
{
    if(MSG_TYPE__NRF_SERVICE_REGISTRATION_REQ == Nnrf_rcvd_nmp_msg_data_ptr->msg_type)
    {
        return process_service_registration_request_msg(Nnrf_rcvd_nmp_msg_data_ptr, debug_flag);	
    }
    else if(MSG_TYPE__NRF_SERVICE_DISCOVERY_REQ == Nnrf_rcvd_nmp_msg_data_ptr->msg_type)
    {
        return process_service_discovery_request_msg(Nnrf_rcvd_nmp_msg_data_ptr, debug_flag);
    }
    else
    {
        printf("%s: NRF has received an unknown message on Nnrf interface \n", __func__);
        return -1;
    }

    return 0;
}


int
listen_for_Nnrf_messages()
{
    int n = 0;
    int len = 0;
    char string[128];
    uint32_t fiveg_service_addr = 0;
    uint16_t fiveg_service_port = 0;
    struct sockaddr_in  fiveg_service_sockaddr;
    nmp_msg_data_t Nnrf_rcvd_nmp_msg_data;

    while(1)
    {
        //////////////////////////////////////////////////////////////////////////////////////
        // Wait for incoming messages on Nnrf interface.
        // NRF can recieve service registration request messages from 5G core functions 
        // like AMF, SMF, UPF etc. NRF will register and store that service information 
        // in its database/repository.
        // 
        // Any service like AMF, SMF, UPF, etc. can send service_discovery 
        // message to NRF and NRF will reposnd back with list of available 
        // services (registered with NRF) in 5G core network.
        //////////////////////////////////////////////////////////////////////////////////////
        len = sizeof(struct sockaddr_in);
        memset(&fiveg_service_sockaddr, 0x0, sizeof(struct sockaddr_in));
        n = recvfrom(g__nrf_config.Nnrf_socket_id,
                     (char *)g__Nnrf_rcvd_msg_buffer,
                     MSG_BUFFER_LEN,
                     MSG_WAITALL,
                     (struct sockaddr *)&(fiveg_service_sockaddr),
                     (socklen_t *)&len);

        fiveg_service_addr = htonl(fiveg_service_sockaddr.sin_addr.s_addr);
        fiveg_service_port = htons(fiveg_service_sockaddr.sin_port);

        if(g__nrf_config.debug_switch)
        {
            get_ipv4_addr_string(fiveg_service_addr, string);
            printf("-----------> Rcvd a message (%u bytes) from Fiveg service at (%s:%u) \n",
                    n, string, fiveg_service_port);
        }

        if(-1 == validate_rcvd_msg_on_Nnrf_interface(g__Nnrf_rcvd_msg_buffer, n, 0))
        {
            printf("%s: Rcvd Nnrf message validation error.. \n", __func__);
            continue;
        }

        if(-1 == parse_nmp_msg(g__Nnrf_rcvd_msg_buffer,
                               n,
                               &(Nnrf_rcvd_nmp_msg_data),
                               g__nrf_config.debug_switch))
        {
            printf("%s: Rcvd Nnrf message parse error.. \n", __func__);
            continue;
        }

        Nnrf_rcvd_nmp_msg_data.fiveg_service_addr = fiveg_service_addr;
        Nnrf_rcvd_nmp_msg_data.fiveg_service_port = fiveg_service_port;

        if(-1 == process_rcvd_Nnrf_msg(&(Nnrf_rcvd_nmp_msg_data),
                                       g__nrf_config.debug_switch))
        {
            printf("Unable to process rcvd Nnrf message \n\n");
        }
    }

    return 0;
}


