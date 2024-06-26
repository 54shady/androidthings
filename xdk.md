# XDK for android

## JDK

download

	https://mirrors.huaweicloud.com/java/jdk/13+33/jdk-13_linux-x64_bin.tar.gz

setup environment variable

	export JAVA_HOME=/path/to/jdk/jdk-13
	export JRE_HOME=${JAVA_HOME}/jre
	export CLASSPATH=.:${JAVA_HOME}/lib:${JRE_HOME}/lib
	export PATH=${JAVA_HOME}/bin:$PATH

## SDK

download

	wget http://dl.google.com/android/android-sdk_r24.4.1-linux.tgz

using sdk via command

	android list sdk --all
	android update sdk --no-ui

setup environment variable

	ANDROID_HOME=/path/to/sdk/android-sdk-linux
	PATH="$PATH:$ANDROID_HOME/tools:$ANDROID_HOME/platform-tools"

## NDK

download

	https://developer.android.com/ndk/downloads/index.html

setup environment variable

	export NDKROOT=/path/to/ndk/android-ndk-r26d
	export PATH=$NDKROOT:$PATH

### NDK usage

[Android NDK 使用](./ndk.md)

## Android Studio

install android studio from tarball(android-studio-2023.3.1.19-linux.tar.gz)

	https://developer.android.com/studio

launch android studio

	cd /path/to/android-studio/bin
	./studio.sh
