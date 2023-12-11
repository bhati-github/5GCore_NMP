all: upf

upf:
	gcc -g -Wall -I. -I./hdr/ -I./../common_hdr/  \
                src/nrf_main.c                        \
                src/Nnrf_msg_handler.c                \
                ../common_src/nmp_item.c              \
                ../common_src/nmp_item_group.c        \
                ../common_src/nmp_msg_parser.c        \
                ../common_src/common_util.c   -o   nrf

clean:
	rm -rf nrf

