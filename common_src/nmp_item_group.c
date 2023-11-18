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


int
nmp_add_item_group__user_location_info(uint8_t *ptr,
                                       uint16_t mcc,
                                       uint16_t mnc,
                                       data_64bit_t nr_cell_identity,
                                       uint32_t tac)
{ 
    int ret = 0;
    uint16_t item_count = 0;
    uint16_t offset = 0;

    *((uint16_t *)(ptr)) = htons(ITEM_GROUP_ID__USER_LOCATION_INFO);

    // 2 bytes of group_item_id + 2 bytes of item_count + 2 bytes of item_len
    offset = 6;

    ret = nmp_add_item__user_location_info_nr_cgi(ptr+ offset, mcc, mnc, nr_cell_identity);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__user_location_info_tai(ptr + offset, mcc, mnc, tac);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    *((uint16_t *)(ptr + 2)) = htons(item_count);
    *((uint16_t *)(ptr + 4)) = htons(offset - 6);

    return offset;
}

int
nmp_add_item_group__n3_pdr(uint8_t *ptr,
                           uint16_t rule_id,
                           uint16_t precedence,
                           uint32_t teid_v4_addr,
                           uint32_t teid,
                           uint8_t  pdr_action,
                           uint16_t pdr_far_id)
{
    int ret = 0;
    uint16_t item_count = 0;
    uint16_t offset = 0;

    *((uint16_t *)(ptr)) = htons(ITEM_GROUP_ID__N3_PDR);

    // 2 bytes of item_id + 2 bytes of item_count + 2 bytes of item_len
    offset = 6;

    ret = nmp_add_item__pdr__rule_id(ptr+ offset, rule_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__pdr__precedence(ptr + offset, precedence);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__pdr__pdi_match_dst_v4_addr(ptr + offset, teid_v4_addr);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__pdr__pdi_match_gtpu_teid(ptr + offset, teid);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__pdr__action(ptr + offset, pdr_action);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__pdr__far_id(ptr + offset, pdr_far_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    *((uint16_t *)(ptr + 2)) = htons(item_count);
    *((uint16_t *)(ptr + 4)) = htons(offset - 6);

    return offset;
}

int
nmp_add_item_group__n6_pdr(uint8_t *ptr,
                           uint16_t rule_id,
                           uint16_t precedence,
                           uint32_t ue_v4_addr,
                           uint8_t  pdr_action,
                           uint16_t pdr_far_id)
{
    int ret = 0;
    uint16_t item_count = 0;
    uint16_t offset = 0;

    *((uint16_t *)(ptr)) = htons(ITEM_GROUP_ID__N6_PDR);

    // 2 bytes of item_id + 2 bytes of item_count + 2 bytes of item_len
    offset = 6;

    ret = nmp_add_item__pdr__rule_id(ptr+ offset, rule_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__pdr__precedence(ptr + offset, precedence);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__pdr__pdi_match_dst_v4_addr(ptr + offset, ue_v4_addr);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__pdr__action(ptr + offset, pdr_action);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__pdr__far_id(ptr + offset, pdr_far_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    *((uint16_t *)(ptr + 2)) = htons(item_count);
    *((uint16_t *)(ptr + 4)) = htons(offset - 6);

    return offset;
}


int
nmp_add_item_group__n3_far(uint8_t *ptr,
                           uint16_t far_id,
                           uint8_t  action_flags,
                           uint8_t  dst_interface,
                           uint32_t teid_v4_addr,
                           uint32_t teid)
{
    int ret = 0;
    uint16_t item_count = 0;
    uint16_t offset = 0;

    *((uint16_t *)(ptr)) = htons(ITEM_GROUP_ID__N3_FAR);

    // 2 bytes of item_id + 2 bytes of item_count + 2 bytes of item_len
    offset = 6;

    ret = nmp_add_item__far__rule_id(ptr+ offset, far_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__far__action_flags(ptr+ offset, action_flags);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__far__dst_interface(ptr+ offset, dst_interface);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__far__outer_hdr_create(ptr+ offset, teid_v4_addr, teid);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    *((uint16_t *)(ptr + 2)) = htons(item_count);
    *((uint16_t *)(ptr + 4)) = htons(offset - 6);

    return offset;
}

int
nmp_add_item_group__n6_far(uint8_t *ptr,
                           uint16_t far_id,
                           uint8_t  action_flags,
                           uint8_t  dst_interface)
{
    int ret = 0;
    uint16_t item_count = 0;
    uint16_t offset = 0;

    *((uint16_t *)(ptr)) = htons(ITEM_GROUP_ID__N6_FAR);

    // 2 bytes of item_id + 2 bytes of item_count + 2 bytes of item_len
    offset = 6;

    ret = nmp_add_item__far__rule_id(ptr+ offset, far_id);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__far__action_flags(ptr+ offset, action_flags);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;

    ret = nmp_add_item__far__dst_interface(ptr+ offset, dst_interface);
    if(-1 == ret)
    {
        return -1;
    }
    offset += ret;
    item_count += 1;


    *((uint16_t *)(ptr + 2)) = htons(item_count);
    *((uint16_t *)(ptr + 4)) = htons(offset - 6);

    return offset;
}


