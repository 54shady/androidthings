# Android KeyLayout

## KeyLayout 文件

KeyLayout文件是一个将Linux上报键值映射为android键值的映射文件

## KeyLayout 优先级

可以有多个KeyLayout文件,但是有如下优先级

	/system/usr/keylayout/Vendor_XXXX_Product_XXXX_Version_XXXX.kl
	/system/usr/keylayout/Vendor_XXXX_Product_XXXX.kl
	/system/usr/keylayout/DEVICE_NAME.kl
	/data/system/devices/keylayout/Vendor_XXXX_Product_XXXX_Version_XXXX.kl
	/data/system/devices/keylayout/Vendor_XXXX_Product_XXXX.kl
	/data/system/devices/keylayout/DEVICE_NAME.kl
	/system/usr/keylayout/Generic.kl
	/data/system/devices/keylayout/Generic.kl

## KeyLayout 文件内容解析

例如KeyLayout文件中有如下内容

	key 59    MENU
	key 102   HOME
	key 114   VOLUME_DOWN
	key 115   VOLUME_UP
	key 116   POWER
	key 143   WAKEUP
	key 158   BACK
	key 212   CAMERA
	key 217   SEARCH

第二列的数字是include/uapi/linux/input.h中定义的,kernel上报的键值就是这个

第三列表示linux上报后映射为android的键

获取vendor号和input name

	cat /proc/bus/input/devices
	getevent -p

## Android 中keycode定义

在文件frameworks/native/include/input/InputEventLabels.h如下

    DEFINE_KEYCODE(VOLUME_DOWN),

而真正的键值是定义在frameworks/native/include/android/keycodes.h中

    AKEYCODE_VOLUME_DOWN     = 25,
