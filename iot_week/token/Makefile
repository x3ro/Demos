# name of your application
export APPLICATION = iotweek_token

# If no BOARD is found in the environment, use this default:
export BOARD ?= avsextrem

# This has to be the absolute path to the RIOT base directory:
export RIOTBASE ?= $(CURDIR)/../../../RIOT

# Change this to 0 show compiler invocation lines by default:
export QUIET ?= 1

USEMODULE += shell
USEMODULE += shell_commands
USEMODULE += posix
USEMODULE += pnet
USEMODULE += ps
USEMODULE += vtimer
USEMODULE += defaulttransceiver
USEMODULE += sixlowpan
USEMODULE += config
USEMODULE += destiny

include $(RIOTBASE)/Makefile.include
