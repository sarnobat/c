#include <jni.h>
#include <stdio.h>

int main(int argc, char **argv) {
    JavaVM *jvm;
    JNIEnv *env;
    JavaVMInitArgs vm_args;
    JavaVMOption options[1];

    // Tell JVM to launch a JAR
    options[0].optionString = "-jar HelloWorld.jar";

    vm_args.version = JNI_VERSION_1_8;
    vm_args.nOptions = 1;
    vm_args.options = options;
    vm_args.ignoreUnrecognized = JNI_FALSE;

    if (JNI_CreateJavaVM(&jvm, (void**)&env, &vm_args) < 0) {
        fprintf(stderr, "Failed to create JVM\n");
        return 1;
    }

    // When you pass -jar, JVM executes it and then exits, so you don’t need to FindClass
    (*jvm)->DestroyJavaVM(jvm);
    return 0;
}
