all: gnb

gnb:
	gcc -g -Wall -I. -I./hdr/ -I./../common_hdr/    \
                src/gnb_main.c                  \
                src/ng_setup.c                  \
                src/ue_reg_and_pdu_setup.c      \
                src/n1_n2_msg_handler.c         \
                ../common_src/nmp_item.c        \
                ../common_src/nmp_item_group.c  \
                ../common_src/nmp_msg_parser.c  \
                ../common_src/cJSON.c           \
                ../common_src/common_util.c        -o   gnb

clean:
	rm -rf gnb

