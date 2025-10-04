#include <jni.h>
#include <stdio.h>

int main(int argc, char **argv) {
    JavaVM *jvm;       // Pointer to the JVM
    JNIEnv *env;       // Pointer to native interface
    JavaVMInitArgs vm_args;
    JavaVMOption options[1];

    // Option: set classpath to current dir
    options[0].optionString = "-Djava.class.path=.";

    vm_args.version = JNI_VERSION_1_8; // pick JDK version (1.8, 11, etc.)
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = JNI_FALSE;

    // Load and initialize JVM
    jint res = JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args);
    if (res < 0) {
        fprintf(stderr, "Can't create JVM\n");
        return 1;
    }

    // Find the HelloWorld class
    jclass cls = (*env)->FindClass(env, "HelloWorld");
    if (cls == NULL) {
        fprintf(stderr, "Can't find HelloWorld class\n");
        (*jvm)->DestroyJavaVM(jvm);
        return 1;
    }

    // Find main method
    jmethodID mid = (*env)->GetStaticMethodID(env, cls, "main", "([Ljava/lang/String;)V");
    if (mid == NULL) {
        fprintf(stderr, "Can't find HelloWorld.main\n");
        (*jvm)->DestroyJavaVM(jvm);
        return 1;
    }

    // Call main()
    (*env)->CallStaticVoidMethod(env, cls, mid, NULL);

    // Destroy the JVM
    (*jvm)->DestroyJavaVM(jvm);
    return 0;
}
