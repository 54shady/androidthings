LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_CFLAGS += -fPIE
LOCAL_LDFLAGS += -fPIE -pie

LOCAL_MODULE := iperf3
LOCAL_SRC_FILES := \
	cjson.c \
	cjson.h \
	flowlabel.h \
	iperf.h \
	iperf_api.c \
	iperf_api.h \
	iperf_error.c \
	iperf_client_api.c \
	iperf_locale.c \
	iperf_locale.h \
	iperf_server_api.c \
	iperf_tcp.c \
	iperf_tcp.h \
	iperf_udp.c \
	iperf_udp.h \
	iperf_sctp.c \
	iperf_sctp.h \
	iperf_util.c \
	iperf_util.h \
	main.c \
	net.c \
	net.h \
	portable_endian.h \
	queue.h \
	tcp_info.c \
	tcp_window_size.c \
	tcp_window_size.h \
	timer.c \
	timer.h \
	units.c \
	units.h \
	version.h

include $(BUILD_EXECUTABLE)
