# Android NDK 使用

## 使用NDK编译libusb里的测试代码

下载最新NDK,解压后设置解压路径

	android-ndk-r17b
	NDK=/path/to/ndk/android-ndk-r17b

下载libusb代码

	libusb-1.0.20

不修改代码的话会有如下错误

	error: only position independent executables (PIE) are supported

需要修改(libusb-1.0.20/android/jni/examples.mk)

```Makefile
include $(CLEAR_VARS)

LOCAL_CFLAGS += -pie -fPIE
LOCAL_LDFLAGS += -pie -fPIE

LOCAL_SRC_FILES := \
$(LIBUSB_ROOT_REL)/examples/listdevs.c
```

在目录libusb-1.0.20/android/jni下执行下面命令来编译

	$NDK/ndk-build APP_ALLOW_MISSING_DEPS=true
