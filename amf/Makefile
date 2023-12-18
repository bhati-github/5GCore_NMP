all: amf

amf:
	gcc -g -Wall -I. -I./hdr/ -I./../common_hdr/    \
                src/amf_main.c                          \
                src/n1_n2_msg_handler.c                 \
                src/Namf_msg_handler.c                  \
                ../common_src/nmp_item.c                \
                ../common_src/nmp_item_group.c          \
                ../common_src/nmp_msg_parser.c          \
                ../common_src/cJSON.c                   \
                ../common_src/common_util.c      -o   amf

clean:
	rm -rf amf

