# Run cvd on rk3588

[cuttlefish on arm64](https://medium.com/@BushMinusZero/cuttlefish-on-arm64-in-aws-b1f60d937614)

[google cuttlefish doc](https://android.googlesource.com/device/google/cuttlefish/)

[build cuttlefish via docker](https://github.com/google/android-cuttlefish/blob/main/README.md#docker)

install bazel on arm

	wget https://releases.bazel.build/7.2.0/release/bazel-7.2.0-linux-arm64
	mv bazelisk-linux-arm64 /usr/local/bin/bazel
	chmod 0755 /usr/local/bin/bazel
	echo "USE_BAZEL_VERSION=7.2.0" > /root/.bazeliskrc

setup go env on arm

	go env -w GOPROXY="https://goproxy.cn,direct"

download source code cuttlefish and build deb on arm

	git clone https://github.com/google/android-cuttlefish
	cd android-cuttlefish
	tools/buildutils/build_packages.sh

install deb packake

	apt install -y ./cuttlefish-base_0.9.30_arm64.deb

config kernel, enabl kvm, and virtio vsock

	CONFIG_VIRTUALIZATION=y
	CONFIG_KVM=y
	CONFIG_VSOCKETS=y
	CONFIG_VHOST_NET=y
	CONFIG_VHOST_VSOCK=y

download prebuild ci image for test

using [aosp-main-throttled]branch(https://ci.android.com/builds/branches/aosp-main-throttled/grid?legacy=1)

- https://ci.android.com/builds/submitted/11976522/aosp_cf_arm64_only_phone-trunk_staging-userdebug/latest/cvd-host_package.tar.gz
- https://ci.android.com/builds/submitted/11976522/aosp_cf_arm64_only_phone-trunk_staging-userdebug/latest/aosp_cf_arm64_only_phone-img-11976522.zip

extra above tarball to arm board /root/aosp(run cvd)

	cd /root/aosp
	HOME=$PWD ./bin/launch_cvd

connect via adb and scrcpy

	adb connect <arm_board_ip>:6520
	scrcpy -s <arm_board_ip>:6520
