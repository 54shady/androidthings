# Android NDK 使用

## 下载NDK源码android-ndk-r17b

解压到目录

	/path_to_ndk/android-ndk-r17b

## standalone toolchain

	NDK=/path_to_ndk/android-ndk-r17b
	$NDK/build/tools/make-standalone-toolchain.sh --platform=android-23 --arch=arm --install-dir=/toolchain/gcc

### 测试交叉编译环境

可以在bashrc中设置下面两个环境变量

	export NDK=/path_to_ndk/android-ndk-r17b
	export NDK_CROSS=/toolchain/gcc/bin
	PATH=$PATH:$NDK:$NDK_CROSS

写一个测试程序编译执行

	arm-linux-androideabi-gcc test.c -ondk_test -pie -fPIE

## Iperf3 for android(使用standalone compiler)

### 下载iperf3[源码下载地址](https://iperf.fr/iperf-download.php#source)

	iperf-3.1.3

源码配置和编译

	./configure --host=arm-linux CC=arm-linux-androideabi-gcc CXX=arm-linux-androideabi-g++ CFLAGS="-static --pie -fPIE" CXXFLAGS="-static" LDFLAGS="-pie -fPIE"
	make

在src目录下会生成iperf3程序,使用前需要创建/tmp目录

	mkdir /tmp

服务端(192.168.1.103)测试命令

	iperf3 -s

客户端(192.168.1.102)测试命令

	iperf3 -c 192.168.1.103 -t 100 -w 512k -f M

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
