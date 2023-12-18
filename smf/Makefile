all: smf

smf:
	gcc -g -Wall -I. -I./hdr/ -I./../common_hdr/    \
                src/smf_main.c                          \
                src/n4_msg_handler.c                    \
                src/Nsmf_msg_handler.c                  \
                src/upf_session_manager.c               \
                src/upf_session_create.c                \
                src/upf_session_modify.c                \
                ../common_src/nmp_item.c                \
                ../common_src/nmp_item_group.c          \
                ../common_src/nmp_msg_parser.c          \
                ../common_src/cJSON.c                   \
                ../common_src/common_util.c      -o   smf

clean:
	rm -rf smf

