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
#include <stdint.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <string.h>
#include <sys/queue.h>
#include <stdarg.h>
#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <ctype.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <linux/if_ether.h>
#include <linux/if_arp.h>
#include <pthread.h>

#include "app.h"
#include "common_util.h"

////////////////////////////////////////////////////
////////////////////////////////////////////////////
////////////////////////////////////////////////////

void
get_ipv4_addr_string(uint32_t ipv4_addr, char *dst)
{
    uint8_t str[8];
    data_32bit_t ip;
    ip.u32 = htonl(ipv4_addr);

    // Max 15 characters expected inside ipv4 address
    memset(dst, 0x0, 15);
    
    sprintf((char *)str, "%u", ip.u8[0]);
    strcat((char *)dst, (char *)str);

    strcat((char *)dst, ".");
    
    sprintf((char *)str, "%u", ip.u8[1]);
    strcat((char *)dst, (char *)str);
    
    strcat((char *)dst, ".");

    sprintf((char *)str, "%u", ip.u8[2]);
    strcat((char *)dst, (char *)str);
    
    strcat((char *)dst, ".");
    
    sprintf((char *)str, "%u", ip.u8[3]);
    strcat((char *)dst, (char *)str);
}

void
get_ipv6_addr_string(uint8_t *src, char *dst)
{
    int i = 0, j = 0;
    uint16_t words[16];
    char string[32][32];
    char tmp_string[32];
    char output_string[32];
    uint8_t skip_was_set = 0;

    //////////////////////////////////////
    // Part 1.
    // Make 16-bit words from source bytes
    //////////////////////////////////////
    j = 0;
    for(i = 0; i < 15; i += 2)
    {
        words[j] = (src[i] << 8) | src[i + 1];
        j++;
    }


    //////////////////////////////////////
    // Part 2.
    // Try to remove leading zero's from
    // each word.
    //////////////////////////////////////
    for(i = 0; i < 8; i ++)
    {
        sprintf(string[i], "%04x", words[i]);

        while(1)
        {
            if('0' == string[i][0])
            {
                strcpy(tmp_string, &string[i][1]);
                strcpy(string[i], tmp_string);
            }
            else
            {
                break;
            }
        }
    }

    //////////////////////////////////////
    // Part 3.
    // Finally, make output string from
    // compacted words. Skip empty words
    // in between.
    //////////////////////////////////////
    memset(output_string, 0x0, 32);
    for(i = 0; i < 8; i++)
    {
        if(strlen(string[i]))
        {
            if(skip_was_set)
            {
                strcat(output_string, ":");
                skip_was_set = 0;
            }
            strcat(output_string, string[i]);

            if(7 != i)
            {
                strcat(output_string, ":");
            }
        }
        else
        {
            skip_was_set = 1;
        }
    }

    strcpy((char *)dst, output_string);
}


void
get_imsi_string(uint8_t *src, char *dst)
{
    int i;
    uint8_t str[8];
    uint8_t *data = src;
    uint8_t bcd;

    // IMSI len is 8 bytes
    memset(dst, 0x0, 8);

    for(i = 0; i < 8; i++)
    {
        bcd = *data & 0xf;
        if (bcd != 0xf)
        {
            sprintf((char *)str, "%u", bcd);
            strcat((char *)dst, (char *)str);
        }
        bcd = *data >> 4;
        if (bcd != 0xf)
        {
            sprintf((char *)str, "%u", bcd);
            strcat((char *)dst, (char *)str);
        }
        data++;
    }
}



int
get_byte_from_char(char c, uint8_t *byte)
{
    switch (c)
    {
        case '0':
            *byte = 0; break;
        case '1':
            *byte = 1; break;
        case '2':
            *byte = 2; break;
        case '3':
            *byte = 3; break;
        case '4':
            *byte = 4; break;
        case '5':
            *byte = 5; break;
        case '6':
            *byte = 6; break;
        case '7':
            *byte = 7; break;
        case '8':
            *byte = 8; break;
        case '9':
            *byte = 9; break;
        case 'a':
            *byte = 0xa; break;
        case 'b':
            *byte = 0xb; break;
        case 'c':
            *byte = 0xc; break;
        case 'd':
            *byte = 0xd; break;
        case 'e':
            *byte = 0xe; break;
        case 'f':
            *byte = 0xf; break;
        default:
            return -1;
    }

    return 0;
}


int
convert_user_imsi_to_pkt_data_format(char *str, int str_len, data_64bit_t *user_imsi)
{
    int i = 0;
    uint8_t data = 0;
    uint8_t byte_low = 0, byte_high = 0;

    for(i = 0; i < str_len; i++)
    {
        if(-1 == get_byte_from_char(str[2 * i], &byte_low))
        {
            return -1;
        }
        if(-1 == get_byte_from_char(str[(2 * i) + 1], &byte_high))
        {
            return -1;
        }

        data = byte_low | (byte_high << 4);
        user_imsi->u8[i] = data;
    }

    return 0;
}

