JNI原理

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
