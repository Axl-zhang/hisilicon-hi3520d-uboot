include $(TOPDIR)/config.mk

LIB     := $(obj)libhifmcv100.a

COBJS-$(CONFIG_HIFMC_SPI_NOR_SUPPORT)     += hifmc100.o  hifmc100_spi_ids.o

COBJS   := $(COBJS-y)
SRCS    := $(COBJS:.o=.c)
OBJS    := $(addprefix $(obj),$(COBJS))

all:    $(LIB)

$(LIB): $(obj).depend $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS)

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
