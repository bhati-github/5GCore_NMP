all: upf

upf:
	gcc -g -Wall -I. -I./hdr/ -I./../common_hdr/    \
                src/upf_main.c                  \
                src/n2_msg_handler.c            \
                src/session_create.c            \
                src/session_modify.c            \
                ../common_src/nmp_item.c        \
                ../common_src/nmp_msg_parser.c  \
                ../common_src/common_util.c             -o   upf

clean:
	rm -rf upf

