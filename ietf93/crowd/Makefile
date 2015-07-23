# name of your application
APPLICATION = crowd

# If no BOARD is found in the environment, use this default:
BOARD ?= iot-lab_M3

# This has to be the absolute path to the RIOT base directory:
RIOTBASE ?= $(CURDIR)/../../../RIOT

# define the modules used for Peta
USEMODULE += ng_netif_default
USEMODULE += auto_init_ng_netif
USEMODULE += ng_ipv6_default
USEMODULE += ng_icmpv6_echo
USEMODULE += ng_udp
USEMODULE += od
USEMODULE += ng_pktdump

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += ps

# Comment this out to disable code in RIOT that does safety checking
# which is not needed in a production environment but helps in the
# development process:
CFLAGS += -DDEVELHELP

# Change this to 0 show compiler invocation lines by default:
QUIET ?= 1

# include application specific include path
INCLUDES += -I$(CURDIR)/include

include $(RIOTBASE)/Makefile.include
