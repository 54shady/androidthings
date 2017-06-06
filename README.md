# JNI

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
