PROTOCOL_DIR = protocol
NRF_DIR = $(PROTOCOL_DIR)/nrf

  SRC += $(NRF_DIR)/matrix.c \
		 $(NRF_DIR)/matrix_basic.c \
		 $(NRF_DIR)/matrix_lpme.c \
		 $(NRF_DIR)/matrix_duplex.c \
		 $(NRF_DIR)/lpme.c \

 SRC += \
       $(NRF_DIR)/main_master.c \
       $(NRF_DIR)/$(NRF_VER_DIR)/cli.c \
       $(NRF_DIR)/microshell/core/microshell.c \
       $(NRF_DIR)/microshell/core/mscore.c \
       $(NRF_DIR)/microshell/util/mscmd.c \
       $(NRF_DIR)/microshell/util/msopt.c \
       $(NRF_DIR)/microshell/util/ntlibc.c \
       $(NRF_DIR)/$(NRF_VER_DIR)/configurator.c \
       $(NRF_DIR)/keycode_str_converter.c \
       $(NRF_DIR)/config_file_util.c \
       $(NRF_DIR)/bmp.c \
       $(NRF_DIR)/bmp_config.c \
       $(NRF_DIR)/bmp_extended_keycode_converter.c \
       $(NRF_DIR)/bmp_process_extended_keycode.c \
       $(NRF_DIR)/bmp_indicator_led.c \
       $(NRF_DIR)/bmp_encoder_actions.c \
       $(NRF_DIR)/bmp_macro.c \
       $(NRF_DIR)/bmp_macro_parser.c \
       $(NRF_DIR)/bmp_debounce.c \
       $(NRF_DIR)/encoder.c \
       $(NRF_DIR)/via.c \


VPATH += $(TMK_PATH)/$(NRF_DIR)/microshell/core
VPATH += $(TMK_PATH)/$(NRF_DIR)/microshell/util


VPATH += $(TMK_PATH)/$(PROTOCOL_DIR)
VPATH += $(TMK_PATH)/$(NRF_DIR)


ifeq ($(strip $(MIDI_ENABLE)), yes)
  include $(TMK_PATH)/protocol/midi.mk
endif

