#include <jni.h>
#include <stdio.h>
#include <stdlib.h>

#if 0
typedef struct {
    char *name;          /* Java里调用的函数名 */
    char *signature;    /* JNI字段描述符, 用来表示Java里调用的函数的参数和返回值类型 */
    void *fnPtr;          /* C语言实现的本地函数 */
} JNINativeMethod;
#endif

void c_hello()
{
	printf("Hello, world!\n");
}

static const JNINativeMethod methods[] = {
	{"hello", "()V", (void *)c_hello},
};

/* System.loadLibrary will triger the JNI_OnLoad */
JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM *jvm, void *reserved)
{
	JNIEnv *env;
	jclass cls;

	if ((*jvm)->GetEnv(jvm, (void **)&env, JNI_VERSION_1_4)) {
		return JNI_ERR; /* JNI version not supported */
	}

	/* find java class */
	cls = (*env)->FindClass(env, "JNIDemo");
	if (cls == NULL) {
		return JNI_ERR;
	}

	/* 2. map java hello method to c c_hello function */
	if ((*env)->RegisterNatives(env, cls, methods, 1) < 0)
		return JNI_ERR;

	return JNI_VERSION_1_4;
}
