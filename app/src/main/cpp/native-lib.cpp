#include <jni.h>
#include <string>
#include "md5.h"
#include <android/log.h>

static char* EXTRE_FRONT= "XFHL";
static char* EXTRE_BEHIND="123";
using namespace std;
static int is_verify=0;
static char* PACKAGE_NAME="com.sicpay.jniapplication";
static char* APP_signature="308201dd30820146020101300d06092a864886f70d010105050030373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b3009060355040613025553301e170d3138313230353035353131365a170d3438313132373035353131365a30373116301406035504030c0d416e64726f69642044656275673110300e060355040a0c07416e64726f6964310b300906035504061302555330819f300d06092a864886f70d010101050003818d0030818902818100845738239b94e5434632b52383789dfd0f2eeef372ccdae30567f59da1c3440b2208c8e01f5a25c637e8b127a909a41e27535a5d30346ac2e4056a56647f889f7bfb338196bb9a6660ef246c681430a66e9868a5a53b2c0ba24e4459371197e12aa7180b0937e9faa670c58cbc60b94e898a8716c40cf6f54c10a4661b6099690203010001300d06092a864886f70d010105050003818100022cf04047e9ef04a073473d1902343fe3fa5713c10dde2746af0a8029cef7e657b7a2237cc13892b72f73ba8830918a46068ce0bcc71c621ee46e292c0d9d466a67a4d1a52a56d23c6e0dc0726b30227e5e7c88a37a11147c4fb86f297b1a4af0588f5741003d585f7800a6d45dab7a07ca2efeb7bb951c71fb7936a7b5d2eb";

/**
 * 进行md5加密
 */
extern "C"
JNIEXPORT jstring JNICALL
Java_com_sicpay_jniapplication_SignatureUtils_signatureParam(JNIEnv *env, jclass clazz,jstring params_) {
    // TODO: implement signatureParam()
    if(is_verify==0){
        return env->NewStringUTF("错误的签名");
    }
    const char *params=env->GetStringUTFChars(params_,0);
    //md5签名规则，加上自己的前缀后缀
    string signature_str(params);
    signature_str.insert(0,EXTRE_FRONT);
    signature_str.insert(signature_str.length(),EXTRE_BEHIND);

    //md5加密
    MD5_CTX *ctx=new MD5_CTX();
    MD5Init(ctx);
    MD5Update(ctx,(unsigned char *)signature_str.c_str(),signature_str.length());
    unsigned char digest[16]={0};
    MD5Final(digest,ctx);

    //生成32位的字符串
    char md5_str[32];
    for(int i=0;i<16;i++){
        sprintf(md5_str,"%s%02x",md5_str,digest[i]);
    }

    return env->NewStringUTF(md5_str);
}

/**
 * 对包名|签名进行校验
 */
extern "C"
JNIEXPORT void JNICALL
Java_com_sicpay_jniapplication_SignatureUtils_signatureVerify(JNIEnv *env, jclass clazz,
                                                              jobject context) {
    //1 获取包名
    jclass j_clz=env->GetObjectClass(context);
    jmethodID j_mid=env->GetMethodID(j_clz,"getPackageName","()Ljava/lang/String;");
    jstring j_package_name=(jstring)env->CallObjectMethod(context,j_mid);
    //2对比包名
    const char* c_package_name=env->GetStringUTFChars(j_package_name,NULL);
    if(strcmp(c_package_name,PACKAGE_NAME)!=0){
        return;
    }
    __android_log_print(ANDROID_LOG_ERROR,"JNI_TAG","包名一致:%s",c_package_name);
    //3 获取签名
    //3.1 获取PackageManager
     j_mid = env->GetMethodID(j_clz,"getPackageManager","()Landroid/content/pm/PackageManager;");
    jobject package_manager = env->CallObjectMethod(context,j_mid);
    //3.2 获取PackageInfo
    j_clz = env->GetObjectClass(package_manager);
    j_mid = env->GetMethodID(j_clz,"getPackageInfo","(Ljava/lang/String;I)Landroid/content/pm/PackageInfo;");
    jobject packageInfo=env->CallObjectMethod(package_manager,j_mid,j_package_name,0x00000040);
    //3.3 获取signatures数组
    j_clz=env->GetObjectClass(packageInfo);
    jfieldID  j_fid=env->GetFieldID(j_clz,"signatures","[Landroid/content/pm/Signature;");
    jobjectArray  signatures=(jobjectArray) env->GetObjectField(packageInfo,j_fid);
    //3.4获取signatures[0]
    jobject signatures_first=env->GetObjectArrayElement(signatures,0);
    //3.5 signatures_first.toCharsString
    j_clz=env->GetObjectClass(signatures_first);
    j_mid=env->GetMethodID(j_clz,"toCharsString","()Ljava/lang/String;");
    jstring signature_str=(jstring)env->CallObjectMethod(signatures_first,j_mid);
    const char *c_signature_str=env->GetStringUTFChars(signature_str,NULL);
    //4比较签名
    if(strcmp(c_signature_str,APP_signature)!=0){
        return;
    }
    is_verify=1;

}