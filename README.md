# Android核心

## Android源代码REPO工程搭建

[Android Source Code Repo](./repo.md)

## Recovery and OTA

[Recovery system and OTA](./ota_update.md)

## Android NDK

[Android NDK 使用](./ndk.md)

## Binder系统

[Android Binder系统分析](./binder)

## JNI原理

JAVA里面调用System.loadLibrary会导致C库里的JNI_OnLoad函数被调用

在JNI_OnLoad里将java里类方法hello和C库里的本地函数c_hello进行映射

	static const JNINativeMethod methods[] = {
		{"hello", "()V", (void *)c_hello},
	};

最终实现了JAVA里调用相应的类方法就调用到了C库里的函数

编译java代码

	javac JNIDemo.java

编译c库

	gcc -I/opt/java-7-openjdk-amd64/include/ -fPIC -shared -o libnative.so native.c

设置库文件链接地址

	export LD_LIBRARY_PATH=.

测试程序

	java JNIDemo

## Android中app调用底层驱动流程

![call flow](./jni_work_flow.png)

1. SystemServer调用loadLibrary调用c库
2. c库被掉用导致c库中的JNI_Onload被调用
3. 在JNI_OnLoad函数里注册本地方法(映射java类方法和c函数)
4. 本地方法中将复杂的硬件操作放在HAL层实现
5. SystemServer通过addService向service_manager注册服务
6. android应用程序通过getService获得服务接口
7. 应用程序通过获得的服务接口来调用本地方法从而操作底层硬件

## Android中JNI调用HAL

JNI向上提供本地函数,向下加载HAL层库文件并调用HAL层函数

android5.1 JNI代码路径为frameworks/base/services/core/jni

HAL负责访问驱动程序执行硬件操作

android5.1 HAL层代码路径为hardware/libhardware/modules

JNI和HAL层代码都是使用C/C++编写的,JNI调用HAL层库文件本质就是动态库编程

android中将这个动态库编程封装为hw_get_module

### JNI怎么怎么使用HAL

调用hw_get_module获得一个hw_module_t结构体

	hw_get_module("led", (hw_module_t const**)&module);

调用hw_module_t结构体里的open函数来获得一个hw_device_t结构体,每个module可以包含多个device

	module->methods->open(module, NULL, &device);

把hw_device_t转换为设备自定义的结构体(设备自定义结构体第一个成员是hw_device_t)

	led_device = (led_device_t *)device;

其中自定义结构体如下

	struct led_device_t {
		struct hw_device_t common; /* 第一个成员是hw_device_t方便转化为hw_device_t */

		int (*led_open)(void);
		int (*led_ctrl)(int which, int status);
	};

### HAL层怎么写

实现一个名为HMI的hw_module_t结构体

	struct hw_module_t HAL_MODULE_INFO_SYM = {
		.tag = HARDWARE_MODULE_TAG,
		.id = LED_HARDWARE_MODULE_ID,
		.methods = &led_module_methods,
	};

实现一个open函数,它根据名字返回一个设备自定义的结构体

	static struct hw_module_methods_t led_module_methods = {
		.open = led_device_open,
	};

	static int led_device_open(const struct hw_module_t* module, const char* id, struct hw_device_t** device)
	{
		*device = &led_dev;
		return 0;
	}

## APK签名

[参考文章 APK使用系统签名](https://www.jianshu.com/p/63d699cffa1a)

- 找到平台签名文件"platform.pk8"和"platform.x509.pem",文件位置android/build/target/product/security/
- 签名工具"signapk.jar"在android/prebuilts/sdk/tools/lib
- 签名证书"platform.pk8" "platform.x509.pem",签名工具"signapk.jar"放置在同一个文件夹
- 执行如下命令
	java -jar signapk.jar platform.x509.pem platform.pk8 Demo.apk signedDemo.apk

或者直接在编译环境执行

	java -jar out/host/linux-x86/framework/signapk.jar build/target/product/security/platform.x509.pem build/target/product/security/platform.pk8 input.apk output.apk

## userdata image

查看分区中userdata大小,这个应该可以通过分区表来决定,在编译的时候获取到

	cat /proc/partitions

	major minor  #blocks  name
	254        0     520912 zram0
	179        0   15388672 mmcblk0
	179        1       4096 mmcblk0p1
	179        2       4096 mmcblk0p2
	179        3      65536 mmcblk0p3
	179        4      65536 mmcblk0p4
	179        5      20480 mmcblk0p5
	179        6     614400 mmcblk0p6
	179        7       4096 mmcblk0p7
	179        8    1089536 mmcblk0p8
	179        9      16384 mmcblk0p9
	179       10    1048576 mmcblk0p10
	179       11      65536 mmcblk0p11
	179       12       4096 mmcblk0p12
	179       13       4096 mmcblk0p13
	179       14      16384 mmcblk0p14
	179       15   12357632 mmcblk0p15
	179       32       4096 mmcblk0rpmb

其中userdata分区对应关系如下

	lrwxrwxrwx root     root              2013-01-23 17:59 userdata -> /dev/block/mmcblk0p15

mmcblk0即emmc的容量(单位kb)

	12357632 * 1024 = 12654215168

修改device/rockchip/rk3288/BoardConfig.mk里添加如下

	BOARD_USERDATAIMAGE_PARTITION_SIZE=12654215168

编译userdata

	make userdataimage

## BootChart

### 制作bootchart

[参考文章 Android bootchart的使用](http://blog.csdn.net/qqxiaoqiang1573/article/details/56839031)

设备上操作

	echo 120 > /data/bootchart/start
	tar -czf bootchart.tgz header proc_stat.log proc_ps.log proc_diskstats.log kernel_pacct

PC机上操作

	bootchart bootchart.tgz

### 比较两次开机bootchart

将前后两次bootchart.tgz拷贝到boot_bootchart_dir和exp_bootchart_dir目录下

	ls base_bootchart_dir exp_bootchart_dir
	base_bootchart_dir:
	bootchart.tgz

	exp_bootchart_dir:
	bootchart.tgz

执行[compare-bootcharts.py](./compare-bootcharts.py)脚本,得到对比结果,其中添加一个名为user_specify_process的进程

	$ ./compare-bootcharts.py base_bootchart_dir exp_bootchart_dir

## KeyEvent and KeyLayout

内核中KeyLayout文件(device/rockchip/common/rk29-keypad.kl)内容如下

	key 59    MENU
	key 102   HOME
	key 114   VOLUME_DOWN
	key 115   VOLUME_UP
	key 116   POWER
	key 143   WAKEUP
	key 158   BACK
	key 212   CAMERA
	key 217   SEARCH

获取按键事件值(0x74对应的116)POWER

	getevent  /dev/input/event3

	0001 0074 00000001
	0000 0000 00000000
	0001 0074 00000000
	0000 0000 00000000

内核中POWER对应的android的键值KEYCODE_POWER

	input  keyevent KEYCODE_POWER        //26

## 命令行操作执行OTA升级

将生成的ota包放到/cache/update.zip后执行如下命令

	echo -e "--update_package=CACHE:update.zip" > /cache/recovery/command
	reboot recovery

### OTA差分包制作

发布pre版本的固件,生成pre版本的ota完整包

	make otapackage

保存pre版本的基础素材

	cp out/target/product/rk3288/obj/PACKAGING/target_files_intermediates/rk3288-target_files-user.gentoo.20171009.110038.zip target_files-pre.zip

修改代码...

发布cur版本的固件,生成cur版本的ota完整包

	make otapackage

保存cur版本的基础素材

	cp out/target/product/rk3288/obj/PACKAGING/target_files_intermediates/rk3288-target_files-user.gentoo.20171009.112455.zip target_files-cur.zip

生成pre和cur的差异升级包

./build/tools/releasetools/ota_from_target_files -v -i rk3288-target_files-pre.zip -p out/host/linux-x86 -k build/target/product/security/testkey rk3288-target_files-cur.zip out/target/product/rk3288/rk3288_OTA_DIFF.zip

将rk3288_OTA_DIFF.zip放到/cache/update_diff.zip后执行如下命令

	echo -e "--update_package=CACHE:update_diff.zip" > /cache/recovery/command
	reboot recovery

## 使用am命令启动apk

首先用下面这个命令查出APK名字

	dumpsys window w | grep \/ | grep name=

比如现在打开摄像头apk后

	dumpsys window w | grep \/ | grep name=

输出结果如下

	mSurface=Surface(name=com.android.camera2/com.android.camera.CameraActivity)

以后就可以用adb来启动这个apk

	am start -n com.android.camera2/com.android.camera.CameraActivity

## 命令行恢复出厂设置

命令行恢复出厂设置

	am broadcast -a android.intent.action.MASTER_CLEAR

## android doc

android文档位于docs/source.android.com

在源码目下编译android docs(输出路径out/target/common/docs/online-sac)

	make online-sac-docs

## crash debug

android系统crash定位,假设有类似下面日志

logcat日志

```c
--------- beginning of crash
libc  : Fatal signal 11 (SIGSEGV), code 1, fault addr 0xc in tid 235 (mediaserver)
DEBUG : *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
DEBUG : Revision: '0'
DEBUG : ABI: 'arm'
DEBUG : pid: 235, tid: 235, name: mediaserver  >>> /system/bin/mediaserver <<<
DEBUG : signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0xc
DEBUG :     r0 00000000  r1 00000000  r2 00001554  r3 00000000
DEBUG :     r4 00000000  r5 b7b6b978  r6 be8ede58  r7 b6a28e40
DEBUG :     r8 00000001  r9 be8ee54c  sl 00000000  fp b7b6b978
DEBUG :     ip b33cc5d0  sp be8ede38  lr b33bb043  pc b69fe92c  cpsr 400f0030
DEBUG :
DEBUG : backtrace:
DEBUG :     #00 pc 0003992c  /system/lib/libc.so (fclose+3)
DEBUG :     #01 pc 0003503f  /system/lib/hw/camera.rk30board.so (_ZN21camera_board_profiles10LoadSensorEPS_+342)
DEBUG :     #02 pc 00018ad3  /system/lib/hw/camera.rk30board.so
DEBUG :     #03 pc 0005451f  /system/lib/libcameraservice.so (_ZN7android12CameraModule18getNumberOfCamerasEv+18)
DEBUG :     #04 pc 0005469b  /system/lib/libcameraservice.so (_ZN7android12CameraModule4initEv+66)
DEBUG :     #05 pc 0004d359  /system/lib/libcameraservice.so (_ZN7android13CameraService10onFirstRefEv+128)
DEBUG :     #06 pc 00002afd  /system/bin/mediaserver
DEBUG :     #07 pc 0000235d  /system/bin/mediaserver
DEBUG :     #08 pc 00016631  /system/lib/libc.so (__libc_init+44)
DEBUG :     #09 pc 000025dc  /system/bin/mediaserver
```
- 从日志中发现是mediaserver往下调用到libc库里的flcose系统调用出错了
- backtrace的前面编号的顺序和调用顺序相反
- 可以看出是在函数LoadSensor中调用fclose出错

错误代码位于下面函数中(多调用了一次)

	int camera_board_profiles::LoadSensor(camera_board_profiles* profiles)

android tombstone日志信息
其中能更清楚的看到错误是发生在调用了OpenAndRegistALLSensor后发生的
```c
*** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
Revision: '0'
ABI: 'arm'
pid: 1058, tid: 1058, name: mediaserver  >>> /system/bin/mediaserver <<<
signal 11 (SIGSEGV), code 1 (SEGV_MAPERR), fault addr 0xc
    r0 00000000  r1 00000000  r2 00001554  r3 00000000
    r4 00000000  r5 b8d71448  r6 befb6e58  r7 b6a27e40
    r8 00000001  r9 befb754c  sl 00000000  fp b8d717a0
    ip b33cb5d0  sp befb6e38  lr b33ba043  pc b69fd92c  cpsr 400f0030
    d0  72433a3a62446220  d1  696c614365746128
    d2  736e6f6328624465  d3  292a726168632078
    d4  4c4d583a3a326c6d  d5  2a746e656d656c45
    d6  43202c746e69202c  d7  3a3a624462696c61
    d8  0000000000000000  d9  0000000000000000
    d10 0000000000000000  d11 0000000000000000
    d12 0000000000000000  d13 0000000000000000
    d14 0000000000000000  d15 0000000000000000
    d16 0000000a0000000a  d17 4024000000000000
    d18 3ff0000000000000  d19 3ff0000000000000
    d20 4024000000000000  d21 3ff0000000000000
    d22 3fa54fca7c9c2079  d23 4004000000000000
    d24 4000000000000000  d25 3e21eddbf0e6d3c7
    d26 4024000000000000  d27 3ff0000000000000
    d28 4004000000000000  d29 3ff0000000000000
    d30 3fef838b93d4c393  d31 3fe94ee19f69b820
    scr 88000012

backtrace:
    #00 pc 0003992c  /system/lib/libc.so (fclose+3)
    #01 pc 0003503f  /system/lib/hw/camera.rk30board.so (_ZN21camera_board_profiles10LoadSensorEPS_+342)
    #02 pc 00018ad3  /system/lib/hw/camera.rk30board.so
    #03 pc 0005451f  /system/lib/libcameraservice.so (_ZN7android12CameraModule18getNumberOfCamerasEv+18)
    #04 pc 0005469b  /system/lib/libcameraservice.so (_ZN7android12CameraModule4initEv+66)
    #05 pc 0004d359  /system/lib/libcameraservice.so (_ZN7android13CameraService10onFirstRefEv+128)
    #06 pc 00002afd  /system/bin/mediaserver
    #07 pc 0000235d  /system/bin/mediaserver
    #08 pc 00016631  /system/lib/libc.so (__libc_init+44)
    #09 pc 000025dc  /system/bin/mediaserver

stack:
         befb6df8  b8d8b2d8  [heap]
         befb6dfc  00000001
         befb6e00  b6a2cd4c
         befb6e04  b6a0dd11  /system/lib/libc.so (__sfp+312)
         befb6e08  00000004
         befb6e0c  f7549977
         befb6e10  00000001
         befb6e14  00000001
         befb6e18  b8d71448  [heap]
         befb6e1c  b6a27e40
         befb6e20  00000001
         befb6e24  befb754c  [stack]
         befb6e28  00000000
         befb6e2c  b33b9501  /system/lib/hw/camera.rk30board.so (_ZN21camera_board_profiles22OpenAndRegistALLSensorEPS_+88)
         befb6e30  b8d71448  [heap]
         befb6e34  befb6948  [stack]
    #00  befb6e38  00000000
         befb6e3c  00000000
         befb6e40  b8d71448  [heap]
         befb6e44  b33ba043  /system/lib/hw/camera.rk30board.so (_ZN21camera_board_profiles10LoadSensorEPS_+346)
    #01  befb6e48  00000000
         befb6e4c  00000000
         befb6e50  00000000
         befb6e54  b6a27e40
         befb6e58  7461642f
         befb6e5c  61632f61
         befb6e60  6172656d
         befb6e64  64656d2f
         befb6e68  705f6169
         befb6e6c  69666f72
         befb6e70  2e73656c
         befb6e74  006c6d78
         befb6e78  00000000
         befb6e7c  00000000
         befb6e80  00000000
         befb6e84  00000000
         ........  ........
    #02  befb6eb8  000002b3
         befb6ebc  00000001
         befb6ec0  00000000
         befb6ec4  00000000
         befb6ec8  00000000
         befb6ecc  00000000
         befb6ed0  00000000
         befb6ed4  00000000
         befb6ed8  00000000
         befb6edc  00000000
         befb6ee0  00000000
         befb6ee4  00000000
         befb6ee8  00000000
         befb6eec  b6a27e40
         befb6ef0  00000000
         befb6ef4  00000000
         ........  ........
    #03  befb7708  00000000
         befb770c  b6e1c69f  /system/lib/libcameraservice.so (_ZN7android12CameraModule4initEv+70)
    #04  befb7710  00000002
         befb7714  b8d8ce50  [heap]
         befb7718  b6e46dd1  /system/lib/libcameraservice.so
         befb771c  b6e1535d  /system/lib/libcameraservice.so (_ZN7android13CameraService10onFirstRefEv+132)
    #05  befb7720  befb7740  [stack]
         befb7724  b6af4885  /system/lib/libbinder.so (_ZN7android8BpBinder8transactEjRKNS_6ParcelEPS1_j)
         befb7728  befb7774  [stack]
         befb772c  b6afca6d  /system/lib/libbinder.so (_ZN7android6ParcelD1Ev+8)
         befb7730  befb7740  [stack]
         befb7734  b6afa6f9  /system/lib/libbinder.so
         befb7738  00000000
         befb773c  b6afa6f9  /system/lib/libbinder.so
         befb7740  00000000
         befb7744  b8d71728  [heap]
         befb7748  00000088
         befb774c  000000ae
         befb7750  b8d71740  [heap]
         befb7754  00000018
         befb7758  b33cc2c0  /system/lib/hw/camera.rk30board.so
         befb775c  b69f3e2d  /system/lib/libc.so (dlmalloc_real+3600)
         ........  ........
    #06  befb77c8  00000000
         befb77cc  b8d6bcd0  [heap]
         befb77d0  b8d70f40  [heap]
         befb77d4  b8d8ce54  [heap]
         befb77d8  befb7898  [stack]
         befb77dc  00000000
         befb77e0  00000001
         befb77e4  befb79f4  [stack]
         befb77e8  befb783c  [stack]
         befb77ec  befb7850  [stack]
         befb77f0  00000000
         befb77f4  b6f1f361  /system/bin/mediaserver
    #07  befb77f8  befb77e0  [stack]
         befb77fc  00000000
         befb7800  00000000
         befb7804  00000000
         befb7808  b6ed109c  [anon:linker_alloc]
         befb780c  00000000
         befb7810  00000000
         befb7814  b6a27e40
         befb7818  befb79f4  [stack]
         befb781c  befb79fc  [stack]
         befb7820  befb7a4c  [stack]
         befb7824  b6f1aa28  /system/bin/linker
         befb7828  00000000
         befb782c  00000000
         befb7830  00000000
         befb7834  00000000
         ........  ........
    #08  befb79c0  befb79d8  [stack]
         befb79c4  00000000
         befb79c8  00000000
         befb79cc  00000000
         befb79d0  00000000
         befb79d4  b6f1f5e0  /system/bin/mediaserver
    #09  befb79d8  b6f22cc8  /system/bin/mediaserver
         befb79dc  b6f22cd0  /system/bin/mediaserver
         befb79e0  b6f22cd8  /system/bin/mediaserver
         befb79e4  befb79f0  [stack]
         befb79e8  00000000
         befb79ec  b6f00843  /system/bin/linker (__dl___linker_init+1462)
         befb79f0  00000001
         befb79f4  befb7afb  [stack]
         befb79f8  00000000
         befb79fc  befb7b13  [stack]
         befb7a00  befb7b50  [stack]
         befb7a04  befb7b63  [stack]
         befb7a08  befb7b78  [stack]
         befb7a0c  befb7b93  [stack]
         befb7a10  befb7ba6  [stack]
         befb7a14  befb7bbf  [stack]
```
代码片段如下
```c
int camera_board_profiles::LoadSensor(camera_board_profiles* profiles)
{
	FILE* fp = fopen(dst_file, "r");
	if(!fp) {
		ALOGE(" is not exist, register all\n");
		goto err_end;
	}

	count = ReadDevNameFromXML(fp, profiles->mXmlDevInfo);
	fclose(fp); /* 这里调用了一次fclose */

	...这里省去无用代码...

err_end:
	OpenAndRegistALLSensor(profiles);
	fclose(fp); /* 假设代码还会执行到这里的话,就再次调用了fclose */
	return err;
}
```
