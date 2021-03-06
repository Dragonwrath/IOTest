/* DO NOT EDIT THIS FILE - it is machine generated */
#include <jni.h>
/* Header for class com_junwtech_iotest_util_JniUtils */

#ifndef _Included_com_junwtech_iotest_util_JniUtils
#define _Included_com_junwtech_iotest_util_JniUtils
#ifdef __cplusplus
extern "C" {
#endif
/*
 * Class:     com_junwtech_iotest_util_JniUtils
 * Method:    getStringFormC
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_junwtech_iotest_util_JniUtils_getStringFormC
  (JNIEnv *, jclass);

/*
 * Class:     com_junwtech_iotest_util_JniUtils
 * Method:    getAmlLoginNetByte
 * Signature: (Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_junwtech_iotest_util_JniUtils_getAmlLoginNetByte
  (JNIEnv *, jclass, jstring);

/*
 * Class:     com_junwtech_iotest_util_JniUtils
 * Method:    encoder
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)[B
 */
JNIEXPORT jbyteArray JNICALL Java_com_junwtech_iotest_util_JniUtils_encoder
  (JNIEnv *, jclass, jstring, jstring, jstring);

/*
 * Class:     com_junwtech_iotest_util_JniUtils
 * Method:    decoder
 * Signature: ([BI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_junwtech_iotest_util_JniUtils_decoder
  (JNIEnv *, jclass, jbyteArray, jint);

#ifdef __cplusplus
}
#endif
#endif
