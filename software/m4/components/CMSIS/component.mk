SOURCE_PATHS += $(BASE_PATH)/src
CFLAGS += -O3 -DVIRTIO_SLAVE_ONLY -D__LOG_TRACE_IO_ -DNO_ATOMIC_64_SUPPORT -DUSE_HAL_DRIVER -DSTM32MP157Fxx 