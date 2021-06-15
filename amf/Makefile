all: amf

amf:
	gcc -g -Wall -I. -I./hdr/ -I./../common_hdr/       \
                src/amf_main.c              \
                src/n1_msg_handler.c        \
                src/n2_msg_handler.c        \
                src/ue_attach.c             \
                src/upf_session_manager.c       \
                src/upf_session_create.c        \
                src/upf_session_modify.c        \
                src/upf_session_common.c        \
                ../common_src/nmp_item.c        \
                ../common_src/nmp_msg_parser.c  \
                ../common_src/common_util.c          -o   amf

clean:
	rm -rf amf

