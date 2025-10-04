#include <jni.h>
#include <stdio.h>

int main(void) {
    JavaVM *jvm = NULL;
    JNIEnv *env = NULL;

    JavaVMOption opt;
    opt.optionString = "-Djava.class.path=HelloWorld.jar";  // put your jar here

    JavaVMInitArgs args;
    args.version = JNI_VERSION_1_8;
    args.nOptions = 1;
    args.options = &opt;
    args.ignoreUnrecognized = JNI_FALSE;

    if (JNI_CreateJavaVM(&jvm, (void**)&env, &args) != 0 || !env) {
        fprintf(stderr, "Failed to create JVM\n");
        return 1;
    }

    // If your main class is in a package, use slashes, e.g. "com/acme/App"
    jclass cls = (*env)->FindClass(env, "HelloWorld");
    if (!cls) { (*env)->ExceptionDescribe(env); goto done; }

    jmethodID mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");
    if (!mid) { (*env)->ExceptionDescribe(env); goto done; }

    // Create empty String[] args to pass to main
    jclass strCls = (*env)->FindClass(env, "java/lang/String");
    if (!strCls) { (*env)->ExceptionDescribe(env); goto done; }
    jobjectArray emptyArgs = (*env)->NewObjectArray(env, 0, strCls, NULL);


    fprintf(stderr, "[trace] %10s:%-5d %32s() SRIDHAR calling java class file...\n", __FILE__, __LINE__, __func__);
    (*env)->CallStaticVoidMethod(env, cls, mid, emptyArgs);
    fprintf(stderr, "[trace] %10s:%-5d %32s() SRIDHAR done\n", __FILE__, __LINE__, __func__);

    if ((*env)->ExceptionCheck(env)) { (*env)->ExceptionDescribe(env); }

done:
    if (jvm) (*jvm)->DestroyJavaVM(jvm);
    return 0;
}
