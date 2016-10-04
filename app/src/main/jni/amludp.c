//
// Created by smxcyp on 2016/6/27.
//
#include <jni.h>
#include <string.h>
#include <stdio.h>


#include "amludp.h"
#include "com_junwtech_iotest_util_JniUtils.h"


/*
* Class:     Java_com_junwei_amludp_ndk_JniUtils
 * com_junwtech_iotest_util_JniUtils
* Method:    getStringFormC
* Signature: ()Ljava/lang/String;
*/
JNIEXPORT jstring JNICALL Java_com_junwtech_iotest_util_JniUtils_getStringFormC
        (JNIEnv *env, jobject obj){


   return (*env)->NewStringUTF(env,"这里是来自c的string");

}

JNIEXPORT jbyteArray JNICALL Java_com_junwtech_iotest_util_JniUtils_getAmlLoginNetByte
        (JNIEnv * env, jclass obj_class, jstring mStrData) {

   const char *c_str = NULL;
   jboolean isCopy;    // 返回JNI_TRUE表示原字符串的拷贝，返回JNI_FALSE表示返回原字符串的指针
   c_str = (*env)->GetStringUTFChars(env, mStrData, &isCopy);
   LOGD("isCopy:%d\n", isCopy);
   if (c_str != NULL) {
      LOGD("C_str: %s \n", c_str);
   }

   jbyteArray firstMacArray = (*env)->NewByteArray(env,6);
   jbyte *bytes = (*env)->GetByteArrayElements(env,firstMacArray,0);

   for ( int i=0; i<6; i++)
   {
      bytes[i]=i;
   }

   //(*env)->SetByteArrayRegion(env,firstMacArray,0,10,bytes);
   (*env)->ReleaseByteArrayElements(env,firstMacArray,bytes, 0);

   LOGD("jbyteArray\n");

   return firstMacArray;

}

JNIEXPORT jbyteArray JNICALL Java_com_junwtech_iotest_util_JniUtils_encoder
        (JNIEnv * env, jclass obj_class, jstring fixHeader, jstring ascii, jstring gb2312) {

    const char *fixHeaderStr = NULL, *asciiStr = NULL, *gb2312Str = NULL;
    static char buf[AML_FRAME_Buff];
    int ret=0;
    jboolean isCopy;    // 返回JNI_TRUE表示原字符串的拷贝，返回JNI_FALSE表示返回原字符串的指针

    memset(buf,0,sizeof(buf));

    fixHeaderStr = (*env)->GetStringUTFChars(env, fixHeader, &isCopy);
    if(fixHeaderStr == NULL){
        LOGE("encoder fixHeaderStr is NULL!\n");
       goto Error_encoder_GoTo;
    }

    asciiStr = (*env)->GetStringUTFChars(env, ascii, &isCopy);
    if(asciiStr == NULL){
        LOGE("encoder asciiStr is NULL!\n");
       goto Error_encoder_GoTo;
    }

    gb2312Str = (*env)->GetStringUTFChars(env, gb2312, &isCopy);
    if(gb2312Str == NULL){
        LOGE("encoder gb2312Str is NULL!\n");
       goto Error_encoder_GoTo;
    }

    ret=AmlEncoder(fixHeaderStr,asciiStr,gb2312Str,buf);
   if(ret <= 0){
       LOGE("AMLencoder ret<=0\n");
       goto Error_encoder_GoTo;
    }else {
        jbyteArray firstMacArrayRet = (*env)->NewByteArray(env, ret);
        jbyte *bytesRet = (*env)->GetByteArrayElements(env, firstMacArrayRet, 0);
        memcpy(bytesRet,buf,ret);
        (*env)->ReleaseByteArrayElements(env,firstMacArrayRet, bytesRet, 0);
        return firstMacArrayRet;
    }

    Error_encoder_GoTo:{
        jbyteArray firstMacArray = (*env)->NewByteArray(env, 2);
        jbyte *bytes = (*env)->GetByteArrayElements(env, firstMacArray, 0);
        bytes[0] = 0;
        bytes[1] = 0;
        (*env)->ReleaseByteArrayElements(env, firstMacArray, bytes, 0);

        return firstMacArray;
  }

}



/*
 * Class:     com_junwtech_iotest_util_JniUtils
 * Method:    decoder
 * Signature: ([BI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_junwtech_iotest_util_JniUtils_decoder
        (JNIEnv * env, jclass obj_class, jbyteArray netArry, jint jlen){

    static char pbuf[AML_FRAME_Buff];
    static char retStrbuf[AML_FRAME_Buff];
    int len=jlen,ret;
    //int i=0;
    jbyte * bytes = (*env)->GetByteArrayElements(env, netArry, NULL);

    memset(retStrbuf,0,sizeof(retStrbuf));
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,bytes,len);

    (*env)->ReleaseByteArrayElements(env, netArry, bytes, 0);

#ifdef AML_DEBUG_INFO
    LOGI("decoder len=%d \n",len);
    /* for(i = 0 ; i < len ; i ++){
        LOGI("0x%02x ",pbuf[i]);
    }*/
#endif //AML_DEBUG_INFO

    ret=AmlDecoder(pbuf,len,retStrbuf);

    if(ret > 0){
        return (*env)->NewStringUTF(env,retStrbuf);
    }

    return (*env)->NewStringUTF(env,"error!");
}


int AmlDecoder(const char *netBuf, const int len,char *retStrBuf)
{
    FixedHeader_type *fixedHeader=(FixedHeader_type *)(netBuf);
    int ret=0;

#ifdef AML_DEBUG_INFO
    LOGI("AmlDecoder broadcast:%d messageType:%d relayFlag:%d len:%d\n",fixedHeader->header.broadcast,
         fixedHeader->header.messageType,fixedHeader->header.relayFlag,fixedHeader->len);
#endif //AML_DEBUG_INFO

    switch (fixedHeader->header.messageType){
        case AML_MessageType_Smartconfig:
            ret=AmlSmartconfigToStr(netBuf,len,retStrBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_Login:
        case AML_MessageType_Logout:
            ret=AmlLoginAndLogoutToStr(netBuf,len,retStrBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_GetInfo:
            ret=AmlGetInformationToStr(netBuf,len,retStrBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_SendText:
            ret=AmlSendTextToStr(netBuf,len,retStrBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_SendAcronym:
            ret=AmlSendAcronymToStr(netBuf,len,retStrBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_SendVoice:

            break;
        case AML_MessageType_SendGroup:
            ret=AmlSendGroupToStr(netBuf,len,retStrBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_SendSync:
            ret=AmlSendSyncToStr(netBuf,len,retStrBuf);
            if(ret>0){
                return ret;
            }
            break;
        default:
            break;
    }


    return 0;
}

int AmlLoginAndLogoutToStr(const char *netBuf, const int len,char *retStrBuf)
{   Aml_LoginAndLogout_type *loginAndLogout=(Aml_LoginAndLogout_type *)(netBuf);
    static int Group,Enctype,userId,netNum,image,id,year,month,day,hour,minute,second;
    static int count,ilen,ret=0,broadcast,messageType,relayFlag;
    unsigned short messageId;
    unsigned long  globalId;
    float  latitude,longitude;
    int slen=sizeof(Aml_LoginAndLogout_type);

    if((loginAndLogout->fixedHeader.len != (len -2)) || (len < slen)){
        LOGE("AmlLoginAndLogoutToStr Data length error!\n");
        return ret;
    }

    broadcast=loginAndLogout->fixedHeader.header.broadcast;
    messageType=loginAndLogout->fixedHeader.header.messageType;
    relayFlag=loginAndLogout->fixedHeader.header.relayFlag;
    messageId=loginAndLogout->messageId;
    year=loginAndLogout->time.year + AML_DATE_BASE_YEAR;
    month=loginAndLogout->time.month;
    day=loginAndLogout->time.day;
    hour=loginAndLogout->time.hour;
    minute=loginAndLogout->time.minute;
    second=loginAndLogout->time.second;
    latitude=loginAndLogout->latitude;
    longitude=loginAndLogout->longitude;
    globalId=loginAndLogout->globalId;
    netNum=loginAndLogout->netNum;
    id=loginAndLogout->apid;
    Group=loginAndLogout->userInfo.Group_Super;
    Enctype=loginAndLogout->userInfo.Enctype;
    userId=loginAndLogout->userInfo.userId;
    image=loginAndLogout->image;

    sprintf(retStrBuf,"%d,%d,%d,%u,%0d-%02d-%02d %02d:%02d:%02d,%f,%f,%d,%d,%d,%d,%d,%d,%d,",broadcast,messageType,relayFlag,messageId,
            year,month,day,hour,minute,second,
            latitude,longitude,(int)globalId,netNum,id,Group,Enctype,userId,image);

    ilen=strlen(retStrBuf);
    count=sizeof(Aml_LoginAndLogout_type);
    strcpy(retStrBuf + ilen,netBuf + count);
    ret=strlen(retStrBuf);

#ifdef AML_DEBUG_INFO
    LOGI("AmlLoginAndLogoutToStr broadcast=%d\n",loginAndLogout->fixedHeader.header.broadcast);
    LOGI("AmlLoginAndLogoutToStr messageType=%d\n",loginAndLogout->fixedHeader.header.messageType);
    LOGI("AmlLoginAndLogoutToStr relayFlag=%d\n",loginAndLogout->fixedHeader.header.relayFlag);
    LOGI("AmlLoginAndLogoutToStr len=%d\n",loginAndLogout->fixedHeader.len);

    LOGI("AmlLoginAndLogoutToStr messageId=%d\n",loginAndLogout->messageId);
    LOGI("AmlLoginAndLogoutToStr time.year=%d\n",loginAndLogout->time.year);
    LOGI("AmlLoginAndLogoutToStr time.month=%d\n",loginAndLogout->time.month);
    LOGI("AmlLoginAndLogoutToStr time.day=%d\n",loginAndLogout->time.day);
    LOGI("AmlLoginAndLogoutToStr time.hour=%d\n",loginAndLogout->time.hour);
    LOGI("AmlLoginAndLogoutToStr time.minute=%d\n",loginAndLogout->time.minute);
    LOGI("AmlLoginAndLogoutToStr time.second=%d\n",loginAndLogout->time.second);

    LOGI("AmlLoginAndLogoutToStr latitude=%f\n",loginAndLogout->latitude);
    LOGI("AmlLoginAndLogoutToStr longitude=%f\n",loginAndLogout->longitude);
    LOGI("AmlLoginAndLogoutToStr globalId=%d\n",(int)loginAndLogout->globalId);
    LOGI("AmlLoginAndLogoutToStr netNum=%d\n",loginAndLogout->netNum);
    LOGI("AmlLoginAndLogoutToStr apid=%d\n",loginAndLogout->apid);
    LOGI("AmlLoginAndLogoutToStr userInfo.Group_Super=%d\n",loginAndLogout->userInfo.Group_Super);
    LOGI("AmlLoginAndLogoutToStr userInfo.Enctype=%d\n",loginAndLogout->userInfo.Enctype);
    LOGI("AmlLoginAndLogoutToStr userInfo.userId=%d\n",loginAndLogout->userInfo.userId);

    LOGI("AmlLoginAndLogoutToStr image=%d\n",loginAndLogout->image);
#endif //AML_DEBUG_INFO

    return ret;
}

int AmlSmartconfigToStr(const char *netBuf, const int len,char *retStrBuf)
{    AML_Smartconfig_type *Smartconfig=(AML_Smartconfig_type *)(netBuf);
    static int ret=0,msgformat;

    if(netBuf[0] != 0){
        LOGE("AmlSmartconfigToStr broadcast !=0 \n");
        return 0;
    }

    msgformat  =Smartconfig->msgformat;

    LOGI("AmlSmartconfigToStr len=%d msgformat=%d reserve=%d\n",Smartconfig->fixedHeader.len,Smartconfig->msgformat,Smartconfig->reserve);
    switch (msgformat){
        case AML_Smartconfig_Response:
            ret=AmlSmartconfigToStrResponse(netBuf,len,retStrBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_Smartconfig_UserStateRes:
            ret=AmlSmartconfigToStrUserStateRes(netBuf,len,retStrBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_Smartconfig_UserPermissionRes:
            ret=AmlSmartconfigToStrUserPermissionRes(netBuf,len,retStrBuf);
            if(ret>0){
                return ret;
            }
            break;
        default:
            break;
    }

    return ret;
}

int AmlSmartconfigToStrResponse(const char *netBuf, const int len,char *retStrBuf)
{    AML_Smartconfig_type *Smartconfig=(AML_Smartconfig_type *)(netBuf);
    static int ret=0,broadcast,messageType,relayFlag,msgformat,reserve;

    if(Smartconfig->fixedHeader.len != (len -2)) {
        LOGE("AmlSmartconfigToStrResponse Data length error!\n");
        return ret;
    }

    broadcast  =Smartconfig->fixedHeader.header.broadcast;
    messageType=Smartconfig->fixedHeader.header.messageType;
    relayFlag  =Smartconfig->fixedHeader.header.relayFlag;

    msgformat  =Smartconfig->msgformat;
    reserve    =Smartconfig->reserve;

    sprintf(retStrBuf,"%d,%d,%d,%d,%d",broadcast,messageType,relayFlag,msgformat,reserve);
    ret=strlen(retStrBuf);

#ifdef AML_DEBUG_INFO
    LOGI("Response broadcast=%d\n",Smartconfig->fixedHeader.header.broadcast);
    LOGI("Response messageType=%d\n",Smartconfig->fixedHeader.header.messageType);
    LOGI("Response relayFlag=%d\n",Smartconfig->fixedHeader.header.relayFlag);
    LOGI("Response len=%d\n",Smartconfig->fixedHeader.len);

    LOGI("Response msgformat=%d\n",Smartconfig->msgformat);
    LOGI("Response reserve=%d\n",Smartconfig->reserve);
#endif //AML_DEBUG_INFO

    return ret;
}

int AmlSmartconfigToStrUserStateRes(const char *netBuf, const int len,char *retStrBuf)
{    AML_Smartconfig_type *Smartconfig=(AML_Smartconfig_type *)(netBuf);
    AML_UserState_type  *UserState=NULL;
    static int ret=0,count,i,ilen,broadcast,messageType,relayFlag,msgformat;

    ilen=sizeof(AML_Smartconfig_type)-4;
    if(Smartconfig->fixedHeader.len != (len -2)){
        LOGE("AmlSmartconfigToStrUserStateRes Data length error!\n");
        return ret;
    }

    if((len-ilen) % 16 != 0) {
        LOGE("AmlSmartconfigToStrUserStateRes payload length error!\n");
        return ret;
    }

    count =(len-ilen) / 16;

#ifdef AML_DEBUG_INFO
    LOGI("UserStateRes broadcast=%d\n",Smartconfig->fixedHeader.header.broadcast);
    LOGI("UserStateRes messageType=%d\n",Smartconfig->fixedHeader.header.messageType);
    LOGI("UserStateRes relayFlag=%d\n",Smartconfig->fixedHeader.header.relayFlag);
    LOGI("UserStateRes len=%d\n",Smartconfig->fixedHeader.len);
    LOGI("UserStateRes msgformat=%d\n",Smartconfig->msgformat);
    LOGI("UserStateRes count=%d\n",count);
#endif //AML_DEBUG_INFO

    broadcast  =Smartconfig->fixedHeader.header.broadcast;
    messageType=Smartconfig->fixedHeader.header.messageType;
    relayFlag  =Smartconfig->fixedHeader.header.relayFlag;
    msgformat  =Smartconfig->msgformat;

    sprintf(retStrBuf,"%d,%d,%d,%d,",broadcast,messageType,relayFlag,msgformat);
    ret=strlen(retStrBuf);

    for(i=0; i<count;i++){
        UserState=(AML_UserState_type *)(netBuf + ilen+(i * 16));
        sprintf(retStrBuf+ret,"%d|%d+%u+%f+%f;",UserState->userInfo.userId,UserState->userInfo.Group_Super,(unsigned int)UserState->globalId,UserState->latitude,UserState->longitude);

#ifdef AML_DEBUG_INFO
        LOGI("UserStateRes user=%d wan=%d globalId=%d latitude=%f longitude=%f\n",UserState->userInfo.userId,UserState->userInfo.Group_Super,
             (int)UserState->globalId,(double)UserState->latitude,(double)UserState->longitude);
#endif //AML_DEBUG_INFO

    }

    return ret;
}

int AmlSmartconfigToStrUserPermissionRes(const char *netBuf, const int len,char *retStrBuf)
{    AML_Smartconfig_type *Smartconfig=(AML_Smartconfig_type *)(netBuf);
    static int ret=0,broadcast,messageType,relayFlag,msgformat,reserve;

    if(Smartconfig->fixedHeader.len  != (len -2) || len < 4) {
        LOGE("AmlSmartconfigToStrUserPermissionRes Data length error!\n");
        return ret;
    }

    broadcast  =Smartconfig->fixedHeader.header.broadcast;
    messageType=Smartconfig->fixedHeader.header.messageType;
    relayFlag  =Smartconfig->fixedHeader.header.relayFlag;

    msgformat  =Smartconfig->msgformat;
    reserve    =Smartconfig->reserve;

    sprintf(retStrBuf,"%d,%d,%d,%d,%d",broadcast,messageType,relayFlag,msgformat,reserve);
    ret=strlen(retStrBuf);

#ifdef AML_DEBUG_INFO
    LOGI("UserPermissionRes broadcast=%d\n",Smartconfig->fixedHeader.header.broadcast);
    LOGI("UserPermissionRes messageType=%d\n",Smartconfig->fixedHeader.header.messageType);
    LOGI("UserPermissionRes relayFlag=%d\n",Smartconfig->fixedHeader.header.relayFlag);
    LOGI("UserPermissionRes len=%d\n",Smartconfig->fixedHeader.len);

    LOGI("UserPermissionRes msgformat=%d\n",Smartconfig->msgformat);

#endif //AML_DEBUG_INFO

    return ret;
}

int AmlGetInformationToStr(const char *netBuf, const int len,char *retStrBuf)
{   Aml_GetInformation_type *getInformation=(Aml_GetInformation_type *)(netBuf);
    AML_GetInfoRes_type  *getInfoRes=NULL;
    static int Enctype,dstApid,srcApid,userId,netNum,year,month,day,hour,minute,second,msgformat;
    static int count,ilen,ret=0,broadcast,messageType,relayFlag,cmd,messageId,errorCode;
    static unsigned long  dstGlobalId,srcGlobalId;

    int slen=sizeof(Aml_GetInformation_type);

    if(getInformation->fixedHeader.len != (len -2) || len < slen){
        LOGE("AmlGetInformationToStr Data length error! len=%d\n",len);
        return ret;
    }

    broadcast=getInformation->fixedHeader.header.broadcast;
    messageType=getInformation->fixedHeader.header.messageType;
    relayFlag=getInformation->fixedHeader.header.relayFlag;
    msgformat=getInformation->msgformat;
    netNum=getInformation->netNum;
    year=getInformation->time.year + AML_DATE_BASE_YEAR;
    month=getInformation->time.month;
    day=getInformation->time.day;
    hour=getInformation->time.hour;
    minute=getInformation->time.minute;
    second=getInformation->time.second;

    dstApid=getInformation->dstApid;
    dstGlobalId=getInformation->dstGlobalId;
    srcApid=getInformation->srcApid;
    srcGlobalId=getInformation->srcGlobalId;

#ifdef AML_DEBUG_INFO
    LOGI("AmlGetInformationToStr broadcast=%d\n",broadcast);
    LOGI("AmlGetInformationToStr messageType=%d\n",messageType);
    LOGI("AmlGetInformationToStr relayFlag=%d\n",relayFlag);
    LOGI("AmlGetInformationToStr len=%d\n",len);

    LOGI("AmlGetInformationToStr msgformat=%d\n",msgformat);
    LOGI("AmlGetInformationToStr netNum=%d\n",netNum);

    LOGI("AmlGetInformationToStr time.year=%d\n",year);
    LOGI("AmlGetInformationToStr time.month=%d\n",month);
    LOGI("AmlGetInformationToStr time.day=%d\n",day);
    LOGI("AmlGetInformationToStr time.hour=%d\n",hour);
    LOGI("AmlGetInformationToStr time.minute=%d\n",minute);
    LOGI("AmlGetInformationToStr time.second=%d\n",second);

    LOGI("AmlGetInformationToStr dstApid=%d\n",dstApid);
    LOGI("AmlGetInformationToStr dstGlobalId=%u\n",(int)dstGlobalId);
    LOGI("AmlGetInformationToStr srcApid=%d\n",srcApid);
    LOGI("AmlGetInformationToStr srcGlobalId=%u\n",(int)srcGlobalId);
#endif //AML_DEBUG_INFO

    switch (msgformat){
        case AML_GetInfo_Request:
            count= sizeof(Aml_GetInformation_type);
            cmd=netBuf[count];
            sprintf(retStrBuf,"%d,%d,%d,%d,%d,%0d-%02d-%02d %02d:%02d:%02d,%d,%d,%d,%d,%d",broadcast,messageType,relayFlag,msgformat,
                    netNum,year,month,day,hour,minute,second,dstApid,(int)dstGlobalId,srcApid,(int)srcGlobalId,cmd);
            ret=strlen(retStrBuf);
#ifdef AML_DEBUG_INFO
            LOGI("AmlGetInformationToStr Request retStrBuf=%s\n",retStrBuf);
            LOGI("AmlGetInformationToStr cmd=%d\n",cmd);
#endif //AML_DEBUG_INFO
            break;
        case AML_GetInfo_Response:
            count= sizeof(Aml_GetInformation_type);
            getInfoRes=(AML_GetInfoRes_type *)(netBuf+count);
            messageId=getInfoRes->messageId;
            cmd=getInfoRes->cmd;
            Enctype=getInfoRes->user.Enctype;
            userId=getInfoRes->user.userId;
            ilen=count+sizeof(AML_GetInfoRes_type);
            errorCode=netBuf[ilen];

            sprintf(retStrBuf,"%d,%d,%d,%d,%d,%0d-%02d-%02d %02d:%02d:%02d,%d,%d,%d-%d,%d,%d,%d,%d,%d,",broadcast,messageType,relayFlag,msgformat,
                    netNum,year,month,day,hour,minute,second,dstApid,Enctype,(int)dstGlobalId,userId,srcApid,(int)srcGlobalId,messageId,cmd,errorCode);
            ilen=strlen(retStrBuf);
            count=sizeof(Aml_GetInformation_type)+sizeof(AML_GetInfoRes_type);
            strncpy(retStrBuf + ilen,netBuf + count,len-count);
            ret=strlen(retStrBuf);

#ifdef AML_DEBUG_INFO
            LOGI("AmlGetInformationToStr Response ret=%d retStrBuf=%s\n",ret,retStrBuf);
            LOGI("AmlGetInformationToStr messageId=%d\n",messageId);
            LOGI("AmlGetInformationToStr cmd=%d\n",cmd);
            LOGI("AmlGetInformationToStr errorCode=%d\n",errorCode);
            LOGI("AmlGetInformationToStr Enctype=%d\n",Enctype);
#endif //AML_DEBUG_INFO
            break;
        default:
            break;
    }

    return ret;
}

int AmlSendTextToStr(const char *netBuf, const int len,char *retStrBuf)
{   Aml_SendTextMsg_type *SendTextMsg=(Aml_SendTextMsg_type *)(netBuf);
    static int ret,ilen,count,Group,Enctype,netNum,dstApid,userId,srcApid,broadcast,messageType,relayFlag;
    static int year,month,day,hour,minute,second;
    unsigned short messageId;
    static unsigned long  dstGlobalId,srcGlobalId;

    int slen=sizeof(Aml_SendTextMsg_type);

    if(SendTextMsg->fixedHeader.len != (len -2) || len < slen){
        LOGE("AmlSendTextToStr Data length error! len=%d\n",len);
        return ret;
    }

    broadcast=SendTextMsg->fixedHeader.header.broadcast;
    messageType=SendTextMsg->fixedHeader.header.messageType;
    relayFlag=SendTextMsg->fixedHeader.header.relayFlag;
    messageId=SendTextMsg->messageId;
    year=SendTextMsg->time.year + AML_DATE_BASE_YEAR;
    month=SendTextMsg->time.month;
    day=SendTextMsg->time.day;
    hour=SendTextMsg->time.hour;
    minute=SendTextMsg->time.minute;
    second=SendTextMsg->time.second;

    dstApid=SendTextMsg->dstApid;
    Group=SendTextMsg->User.Group_Super;
    Enctype=SendTextMsg->User.Enctype;
    dstGlobalId=SendTextMsg->dstGlobalId;
    userId=SendTextMsg->User.userId;
    srcApid=SendTextMsg->srcApid;
    srcGlobalId=SendTextMsg->srcGlobalId;

    count= sizeof(Aml_SendTextMsg_type);
    netNum=netBuf[count];


    sprintf(retStrBuf,"%d,%d,%d,%d,%0d-%02d-%02d %02d:%02d:%02d,%d,%d,%d,%d-%d,%d,%d,%d,",broadcast,messageType,relayFlag,messageId,
            year,month,day,hour,minute,second,dstApid,Group,Enctype,(int)dstGlobalId,userId,srcApid,(int)srcGlobalId,netNum);
    ilen=strlen(retStrBuf);
    count=sizeof(Aml_SendTextMsg_type)+1;
    strcpy(retStrBuf + ilen,netBuf + count);
    ret=strlen(retStrBuf);

#ifdef AML_DEBUG_INFO
    LOGI("AmlSendTextToStr broadcast=%d\n",SendTextMsg->fixedHeader.header.broadcast);
    LOGI("AmlSendTextToStr messageType=%d\n",SendTextMsg->fixedHeader.header.messageType);
    LOGI("AmlSendTextToStr relayFlag=%d\n",SendTextMsg->fixedHeader.header.relayFlag);
    LOGI("AmlSendTextToStr len=%d\n",SendTextMsg->fixedHeader.len);

    LOGI("AmlSendTextToStr messageId=%d\n",SendTextMsg->messageId);

    LOGI("AmlSendTextToStr time.year=%d\n",SendTextMsg->time.year);
    LOGI("AmlSendTextToStr time.month=%d\n",SendTextMsg->time.month);
    LOGI("AmlSendTextToStr time.day=%d\n",SendTextMsg->time.day);
    LOGI("AmlSendTextToStr time.hour=%d\n",SendTextMsg->time.hour);
    LOGI("AmlSendTextToStr time.minute=%d\n",SendTextMsg->time.minute);
    LOGI("AmlSendTextToStr time.second=%d\n",SendTextMsg->time.second);

    LOGI("AmlSendTextToStr dstApid=%d\n",SendTextMsg->dstApid);
    LOGI("AmlSendTextToStr Group=%d\n",SendTextMsg->User.Group_Super);
    LOGI("AmlSendTextToStr Enctype=%d\n",SendTextMsg->User.Enctype);
    LOGI("AmlSendTextToStr dstGlobalId=%d\n",(int)SendTextMsg->dstGlobalId);
    LOGI("AmlSendTextToStr userId=%d\n",SendTextMsg->User.userId);
    LOGI("AmlSendTextToStr srcApid=%d\n",SendTextMsg->srcApid);
    LOGI("AmlSendTextToStr srcGlobalId=%d\n",(int)SendTextMsg->srcGlobalId);

    LOGI("AmlSendTextToStr netNum=%d\n",netNum);
#endif //AML_DEBUG_INFO

    return ret;
}

int AmlSendAcronymToStr(const char *netBuf, const int len,char *retStrBuf)
{   Aml_SendTextMsg_type *SendTextMsg=(Aml_SendTextMsg_type *)(netBuf);
    static int ret,ilen,count,Group,netNum,dstApid,userId,srcApid,broadcast,messageType,relayFlag;
    static int year,month,day,hour,minute,second,Acronym;
    unsigned short messageId;
    static unsigned long  dstGlobalId,srcGlobalId;

    int slen=sizeof(Aml_SendTextMsg_type);

    if(SendTextMsg->fixedHeader.len != (len -2) || len < slen){
        LOGE("AmlSendAcronymToStr Data length error! len=%d\n",len);
        return ret;
    }

    broadcast=SendTextMsg->fixedHeader.header.broadcast;
    messageType=SendTextMsg->fixedHeader.header.messageType;
    relayFlag=SendTextMsg->fixedHeader.header.relayFlag;
    messageId=SendTextMsg->messageId;
    year=SendTextMsg->time.year + AML_DATE_BASE_YEAR;
    month=SendTextMsg->time.month;
    day=SendTextMsg->time.day;
    hour=SendTextMsg->time.hour;
    minute=SendTextMsg->time.minute;
    second=SendTextMsg->time.second;

    dstApid=SendTextMsg->dstApid;
    Group=SendTextMsg->User.Group_Super;
    dstGlobalId=SendTextMsg->dstGlobalId;
    userId=SendTextMsg->User.userId;
    srcApid=SendTextMsg->srcApid;
    srcGlobalId=SendTextMsg->srcGlobalId;

    count= sizeof(Aml_SendTextMsg_type);
    netNum=netBuf[count];
    Acronym=netBuf[count+1];

    sprintf(retStrBuf,"%d,%d,%d,%d,%0d-%02d-%02d %02d:%02d:%02d,%d,%d,%d-%d,%d,%d,%d,%d",broadcast,messageType,relayFlag,messageId,
            year,month,day,hour,minute,second,dstApid,Group,(int)dstGlobalId,userId,srcApid,(int)srcGlobalId,netNum,Acronym);
    ilen=strlen(retStrBuf);
    count=sizeof(Aml_SendTextMsg_type)+2;
    strcpy(retStrBuf + ilen,netBuf + count);
    ret=strlen(retStrBuf);

#ifdef AML_DEBUG_INFO
    LOGI("AmlSendAcronymToStr broadcast=%d\n",SendTextMsg->fixedHeader.header.broadcast);
    LOGI("AmlSendAcronymToStr messageType=%d\n",SendTextMsg->fixedHeader.header.messageType);
    LOGI("AmlSendAcronymToStr relayFlag=%d\n",SendTextMsg->fixedHeader.header.relayFlag);
    LOGI("AmlSendAcronymToStr len=%d\n",SendTextMsg->fixedHeader.len);

    LOGI("AmlSendAcronymToStr messageId=%d\n",SendTextMsg->messageId);

    LOGI("AmlSendAcronymToStr time.year=%d\n",SendTextMsg->time.year);
    LOGI("AmlSendAcronymToStr time.month=%d\n",SendTextMsg->time.month);
    LOGI("AmlSendAcronymToStr time.day=%d\n",SendTextMsg->time.day);
    LOGI("AmlSendAcronymToStr time.hour=%d\n",SendTextMsg->time.hour);
    LOGI("AmlSendAcronymToStr time.minute=%d\n",SendTextMsg->time.minute);
    LOGI("AmlSendAcronymToStr time.second=%d\n",SendTextMsg->time.second);

    LOGI("AmlSendAcronymToStr dstApid=%d\n",SendTextMsg->dstApid);
    LOGI("AmlSendAcronymToStr Group=%d\n",SendTextMsg->User.Group_Super);
    LOGI("AmlSendAcronymToStr Enctype=%d\n",SendTextMsg->User.Enctype);
    LOGI("AmlSendAcronymToStr dstGlobalId=%d\n",(int)SendTextMsg->dstGlobalId);
    LOGI("AmlSendAcronymToStr userId=%d\n",SendTextMsg->User.userId);
    LOGI("AmlSendAcronymToStr srcApid=%d\n",SendTextMsg->srcApid);
    LOGI("AmlSendAcronymToStr srcGlobalId=%d\n",(int)SendTextMsg->srcGlobalId);

    LOGI("AmlSendAcronymToStr netNum=%d\n",netNum);
    LOGI("AmlSendAcronymToStr Acronym=%d\n",Acronym);
#endif //AML_DEBUG_INFO

    return ret;
}

int AmlSendSyncToStr(const char *netBuf, const int len,char *retStrBuf)
{   Aml_SyncMsg_type     *syncMsg=(Aml_SyncMsg_type *)(netBuf);
    Aml_SyncMsgId_type   *syncMsgId=NULL;
    Aml_SyncMsgTime_type *syncMsgTime=NULL;
    static int ret,count,netNum,msgformat,broadcast,messageType,relayFlag;
    static int yearS,monthS,dayS,hourS,minuteS,secondS,yearE,monthE,dayE,hourE,minuteE,secondE;
    unsigned short msgIdStart,msgIdEnd;
    unsigned long  globalId;
    int slen=sizeof(Aml_SyncMsg_type);

    if(syncMsg->fixedHeader.len != (len -2) || len <slen){
        LOGE("AmlSendSyncToStr Data length error! len=%d\n",len);
        return ret;
    }

    broadcast=syncMsg->fixedHeader.header.broadcast;
    messageType=syncMsg->fixedHeader.header.messageType;
    relayFlag=syncMsg->fixedHeader.header.relayFlag;

    msgformat=syncMsg->msgformat;
    netNum   =syncMsg->netNum;
    globalId =syncMsg->globalId;

    switch (msgformat){
        case AML_SendSync_ID:
            count= sizeof(Aml_SyncMsg_type);
            syncMsgId=(Aml_SyncMsgId_type *)(netBuf+count);

            msgIdStart=syncMsgId->msgIdStart;
            msgIdEnd  =syncMsgId->msgIdEnd;

            sprintf(retStrBuf,"%d,%d,%d,%d,%d,%u,%d,%d",broadcast,messageType,relayFlag,msgformat,
                    netNum,(unsigned int)globalId,msgIdStart,msgIdEnd);
            ret=strlen(retStrBuf);
#ifdef AML_DEBUG_INFO
            LOGI("AmlSendSyncToStr broadcast=%d\n",syncMsg->fixedHeader.header.broadcast);
            LOGI("AmlSendSyncToStr messageType=%d\n",syncMsg->fixedHeader.header.messageType);
            LOGI("AmlSendSyncToStr relayFlag=%d\n",syncMsg->fixedHeader.header.relayFlag);
            LOGI("AmlSendSyncToStr len=%d\n",syncMsg->fixedHeader.len);

            LOGI("AmlSendSyncToStr msgformat=%d\n",syncMsg->msgformat);
            LOGI("AmlSendSyncToStr netNum=%d\n",syncMsg->netNum);
            LOGI("AmlSendSyncToStr globalId=%d\n",(int)syncMsg->globalId);
            LOGI("AmlSendSyncToStr msgIdStart=%d\n",syncMsgId->msgIdStart);
            LOGI("AmlSendSyncToStr msgIdEnd=%d\n",syncMsgId->msgIdEnd);
#endif //AML_DEBUG_INFO
            break;
        case AML_SendSync_TimeQuantum:
        case AML_SendSync_LoginAndLogout:
            count= sizeof(Aml_SyncMsg_type);
            syncMsgTime=(Aml_SyncMsgTime_type *)(netBuf+count);

            yearS  =syncMsgTime->timeStart.year + AML_DATE_BASE_YEAR;
            monthS =syncMsgTime->timeStart.month;
            dayS   =syncMsgTime->timeStart.day;
            hourS  =syncMsgTime->timeStart.hour;
            minuteS=syncMsgTime->timeStart.minute;
            secondS=syncMsgTime->timeStart.second;

            yearE  =syncMsgTime->timeEnd.year + AML_DATE_BASE_YEAR;
            monthE =syncMsgTime->timeEnd.month;
            dayE   =syncMsgTime->timeEnd.day;
            hourE  =syncMsgTime->timeEnd.hour;
            minuteE=syncMsgTime->timeEnd.minute;
            secondE=syncMsgTime->timeEnd.second;

            sprintf(retStrBuf,"%d,%d,%d,%d,%d,%u,%0d-%02d-%02d %02d:%02d:%02d,%0d-%02d-%02d %02d:%02d:%02d",broadcast,messageType,relayFlag,msgformat,
                    netNum,(unsigned int)globalId,yearS,monthS,dayS,hourS,minuteS,secondS,yearE,monthE,dayE,hourE,minuteE,secondE);
            ret=strlen(retStrBuf);
#ifdef AML_DEBUG_INFO
            LOGI("AmlSendSyncToStr broadcast=%d\n",syncMsg->fixedHeader.header.broadcast);
            LOGI("AmlSendSyncToStr messageType=%d\n",syncMsg->fixedHeader.header.messageType);
            LOGI("AmlSendSyncToStr relayFlag=%d\n",syncMsg->fixedHeader.header.relayFlag);
            LOGI("AmlSendSyncToStr len=%d\n",syncMsg->fixedHeader.len);

            LOGI("AmlSendSyncToStr msgformat=%d\n",syncMsg->msgformat);
            LOGI("AmlSendSyncToStr netNum=%d\n",syncMsg->netNum);
            LOGI("AmlSendSyncToStr globalId=%d\n",(int)syncMsg->globalId);

            LOGI("AmlSendSyncToStr yearS=%d\n",syncMsgTime->timeStart.year);
            LOGI("AmlSendSyncToStr monthS=%d\n",syncMsgTime->timeStart.month);
            LOGI("AmlSendSyncToStr dayS=%d\n",syncMsgTime->timeStart.day);
            LOGI("AmlSendSyncToStr hourS=%d\n",syncMsgTime->timeStart.hour);
            LOGI("AmlSendSyncToStr minuteS=%d\n",syncMsgTime->timeStart.minute);
            LOGI("AmlSendSyncToStr secondS=%d\n",syncMsgTime->timeStart.second);

            LOGI("AmlSendSyncToStr yearE=%d\n",syncMsgTime->timeEnd.year);
            LOGI("AmlSendSyncToStr monthE=%d\n",syncMsgTime->timeEnd.month);
            LOGI("AmlSendSyncToStr dayE=%d\n",syncMsgTime->timeEnd.day);
            LOGI("AmlSendSyncToStr hourE=%d\n",syncMsgTime->timeEnd.hour);
            LOGI("AmlSendSyncToStr minuteE=%d\n",syncMsgTime->timeEnd.minute);
            LOGI("AmlSendSyncToStr secondE=%d\n",syncMsgTime->timeEnd.second);

#endif //AML_DEBUG_INFO
            break;
        default:
            break;
    }

    return ret;
}


int AmlSendGroupToStr(const char *netBuf, const int len,char *retStrBuf)
{   Aml_GroupMsg_type     *groupMsg=(Aml_GroupMsg_type *)(netBuf);
    char pbuf[AML_FRAME_Buff],name[AML_UserName_MAX_LEN*2];
    static int i,ret,ilen,pLen,nameLen,count,groupLen,netNum,broadcast,messageType,relayFlag;
    static int year,month,day,hour,minute,second,Enctype,Group,srcApid;
    static unsigned long  GlobalId;
    unsigned long *Gid=NULL;
    unsigned short msgId;
    int slen=sizeof(Aml_GroupMsg_type);

    if(groupMsg->fixedHeader.len != (len -2) || len < slen){
        LOGE("AmlSendGroupToStr Data length error! len=%d\n",len);
        return ret;
    }

    broadcast  =groupMsg->fixedHeader.header.broadcast;
    messageType=groupMsg->fixedHeader.header.messageType;
    relayFlag  =groupMsg->fixedHeader.header.relayFlag;
    msgId =groupMsg->messageId;

    year=groupMsg->time.year + AML_DATE_BASE_YEAR;
    month=groupMsg->time.month;
    day=groupMsg->time.day;
    hour=groupMsg->time.hour;
    minute=groupMsg->time.minute;
    second=groupMsg->time.second;

    srcApid =groupMsg->srcApid;
    Enctype =groupMsg->User.Enctype;
    GlobalId=groupMsg->GlobalId;
    Group   =groupMsg->User.userId;
    GlobalId=groupMsg->GlobalId;
    netNum  =groupMsg->netNum;

    nameLen=groupMsg->nameLen > AML_UserName_MAX_LEN ? AML_UserName_MAX_LEN : groupMsg->nameLen;
    memset(name,0,sizeof(name));
    strncpy(name,netBuf+ sizeof(Aml_GroupMsg_type),nameLen);

#ifdef AML_DEBUG_INFO
    LOGI("AmlSendGroupToStr nameLen=%d name=%s\n",nameLen,name);
#endif //AML_DEBUG_INFO

    sprintf(retStrBuf,"%d,%d,%d,%d,%0d-%02d-%02d %02d:%02d:%02d,%d,%d,%d-%d,%d,",broadcast,messageType,relayFlag,msgId,
            year,month,day,hour,minute,second,srcApid,Enctype,(int)GlobalId,Group,netNum);
    ilen=strlen(retStrBuf);

#ifdef AML_DEBUG_INFO
    LOGI("AmlSendGroupToStr retStrBuf=%s\n",retStrBuf);
#endif //AML_DEBUG_INFO

    count=sizeof(Aml_GroupMsg_type)+nameLen;
    groupLen=len-count;
    if(groupLen % 4 != 0){
        LOGI("AmlSendGroupToStr The data format error\n");
        ret=0;
    }

#ifdef AML_DEBUG_INFO
        LOGI("AmlSendGroupToStr groupLen=%d\n",groupLen);
#endif //AML_DEBUG_INFO

    for(i=0;i<(groupLen/4);i++){
        Gid=(unsigned long *)(netBuf+count+(i*4));

        memset(pbuf,0,sizeof(pbuf));
        sprintf(pbuf,"%d+",(int)*Gid);
        pLen=strlen(pbuf);

        strncpy(retStrBuf+ilen,pbuf,pLen);
        ilen=ilen+pLen;
    }

    retStrBuf[ilen-1]=',';
    strncpy(retStrBuf+ilen,name,nameLen);

#ifdef AML_DEBUG_INFO
    LOGI("AmlSendGroupToStr retStrBuf=%s\n",retStrBuf);
#endif //AML_DEBUG_INFO

    ret=strlen(retStrBuf);
    return ret;
}


//=================================send================================================================

int AmlEncoder(const char *fixHeader, const char *ascii, const char *gb2312, char *netBuf)
{
    static FixedHeader_type fixedHeader;
    int ret=0,broadcast,messageType,relayFlag;

    sscanf(fixHeader,"%d,%d,%d",&broadcast,&messageType,&relayFlag);
    fixedHeader.header.broadcast  =(unsigned char)broadcast;
    fixedHeader.header.messageType=(unsigned char)messageType;
    fixedHeader.header.relayFlag  =(unsigned char)relayFlag;
    fixedHeader.len=0;

#ifdef AML_DEBUG_INFO
    LOGI("AmlEncoder broadcast:%d messageType:%d relayFlag:%d\n",fixedHeader.header.broadcast,fixedHeader.header.messageType,fixedHeader.header.relayFlag);
    LOGI("AmlEncoder ascii:%s gb2312:%s\n",ascii,gb2312);
#endif //AML_DEBUG_INFO

    switch (fixedHeader.header.messageType){
        case AML_MessageType_Smartconfig:
            ret=AmlSmartconfigToByte(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_Login:
        case AML_MessageType_Logout:
            ret=AmlLoginAndLogoutToByte(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_GetInfo:
            ret=AmlGetInformationToByte(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_SendText:
            ret=AmlSendTextToByte(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_SendAcronym:
            ret=AmlSendAcronymToByte(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_SendVoice:
            ret=AmlSendVoiceToByte(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_SendGroup:
            ret=AmlSendGroupMsgToByte(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_MessageType_SendSync:
            ret=AmlSyncMsgToByte(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }            break;
        default:
            break;
    }

    return 0;
}

int AmlLoginAndLogoutToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{    Aml_LoginAndLogout_type *loginAndLogout=(Aml_LoginAndLogout_type *)(netBuf);
    char pbuf[AML_FRAME_Buff];
    static int ret,count,Group,Enctype,userId,netNum,image,id,len;
    unsigned short messageId;
    unsigned long  globalId;

    loginAndLogout->fixedHeader.header = fixedHeader.header;
    loginAndLogout->fixedHeader.len=0;

    LOGI("AmlLoginAndLogoutToByte ascii=%s\n",ascii);

    count=0;
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlLoginAndLogoutToByte 'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",(int *)&messageId);

    len=strlen(ascii);
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlLoginAndLogoutToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    timeStrToStruct(pbuf,&loginAndLogout->time);

    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlLoginAndLogoutToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%f",&loginAndLogout->latitude);

    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlLoginAndLogoutToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%f",&loginAndLogout->longitude);

    memset(pbuf,0,sizeof(pbuf));
    strcpy(pbuf,ascii+count);
    sscanf(pbuf,"%d,%d,%d,%d,%d,%d,%d",(int *)&globalId,&netNum,&id,&Group,&Enctype,&userId,&image);

    loginAndLogout->netNum=(unsigned char)netNum;
    loginAndLogout->apid=(unsigned char)id;
    loginAndLogout->userInfo.Group_Super=(unsigned char)Group;
    loginAndLogout->userInfo.Enctype=(unsigned char)Enctype;
    loginAndLogout->userInfo.userId=(unsigned char)userId;
    loginAndLogout->image=(unsigned char)image;
    loginAndLogout->messageId=messageId;
    loginAndLogout->globalId=globalId;

    ret = sizeof(Aml_LoginAndLogout_type);
    len = strlen(gb2312);
    memcpy(netBuf+ret,gb2312,len);
    //LOGD("ret=%d len=%d\n",ret,len);
    ret=ret+len;
    loginAndLogout->fixedHeader.len=(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
    LOGI("AmlLoginAndLogoutToByte broadcast=%d\n",loginAndLogout->fixedHeader.header.broadcast);
    LOGI("AmlLoginAndLogoutToByte messageType=%d\n",loginAndLogout->fixedHeader.header.messageType);
    LOGI("AmlLoginAndLogoutToByte relayFlag=%d\n",loginAndLogout->fixedHeader.header.relayFlag);
    LOGI("AmlLoginAndLogoutToByte len=%d\n",loginAndLogout->fixedHeader.len);

    LOGI("AmlLoginAndLogoutToByte messageId=%d\n",loginAndLogout->messageId);
    LOGI("AmlLoginAndLogoutToByte year=%d\n",loginAndLogout->time.year);
    LOGI("AmlLoginAndLogoutToByte month=%d\n",loginAndLogout->time.month);
    LOGI("AmlLoginAndLogoutToByte day=%d\n",loginAndLogout->time.day);
    LOGI("AmlLoginAndLogoutToByte hour=%d\n",loginAndLogout->time.hour);
    LOGI("AmlLoginAndLogoutToByte minute=%d\n",loginAndLogout->time.minute);
    LOGI("AmlLoginAndLogoutToByte second=%d\n",loginAndLogout->time.second);

    LOGI("AmlLoginAndLogoutToByte latitude=%f\n",loginAndLogout->latitude);
    LOGI("AmlLoginAndLogoutToByte longitude=%f\n",loginAndLogout->longitude);
    LOGI("AmlLoginAndLogoutToByte globalId=%d\n",(int)loginAndLogout->globalId);
    LOGI("AmlLoginAndLogoutToByte netNum=%d\n",loginAndLogout->netNum);
    LOGI("AmlLoginAndLogoutToByte apid=%d\n",loginAndLogout->apid);
    LOGI("AmlLoginAndLogoutToByte Group_Super=%d\n",loginAndLogout->userInfo.Group_Super);
    LOGI("AmlLoginAndLogoutToByte Enctype=%d\n",loginAndLogout->userInfo.Enctype);
    LOGI("AmlLoginAndLogoutToByte userId=%d\n",loginAndLogout->userInfo.userId);

    LOGI("LoginAndLogout.image=%d\n",loginAndLogout->image);
#endif //AML_DEBUG_INFO

    return ret;
}

int timeStrToStruct(const char *Strtime, Time_type *time){
    int year,month,day,hour,minute,second;
    sscanf(Strtime,"%d-%d-%d %d:%d:%d",&year,&month,&day,&hour,&minute,&second);
    time->year  = (unsigned char)(year - AML_DATE_BASE_YEAR);
    time->month = (unsigned char)month;
    time->day   = (unsigned char)day;
    time->hour  = (unsigned char)hour;
    time->minute= (unsigned char)minute;
    time->second= (unsigned char)second;
    return 0;
}

int AmlSmartconfigToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{   AML_Smartconfig_type * Smartconfig=(AML_Smartconfig_type *)(netBuf);
    char pbuf[AML_FRAME_Buff];
    static int ret,msgformat;

    if(Smartconfig->fixedHeader.header.broadcast != 0){
        LOGE("AmlSmartconfigToByte  broadcast != 0(Local)!\n");
        return 0;
    }

    if(strlen(ascii) <= 0){
        LOGE("AmlSmartconfigToByte  'ascii' is NULL!\n");
        return 0;
    }

    ret = strcspn(ascii,",");
    if(ret <= 0){
        LOGE("AmlSmartconfigToByte  'ascii' is NULL!\n");
        return 0;
    }

    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii,ret);
    sscanf(pbuf,"%d",&msgformat);

    //LOGD("AmlSmartconfigToByte msgformat=%d\n",msgformat);

    switch (msgformat){
        case AML_Smartconfig_Request:
            ret=AmlSmartconfigToByteRequest(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_Smartconfig_UserStateReq:
            ret=AmlSmartconfigToByteUserStateReq(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_Smartconfig_UserPermissionReq:
            ret=AmlSmartconfigToByteUserPermissionReq(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        default:
            break;
    }

    return ret;
}

int AmlSmartconfigToByteRequest(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{   AML_Smartconfig_type * Smartconfig=(AML_Smartconfig_type *)(netBuf);
    static int ret,count,msgformat;
    unsigned long  globalId;

#ifdef AML_DEBUG_INFO
    //LOGI("AmlSmartconfigToByteRequest ascii:%s gb2312:%s\n",ascii,gb2312);
#endif //AML_DEBUG_INFO

    Smartconfig->fixedHeader.header = fixedHeader.header;
    Smartconfig->fixedHeader.len=0;

    sscanf(ascii,"%d,%u",&msgformat,(int *)&globalId);

    //LOGD("AmlSmartconfigToByteRequest msgformat=%d\n",msgformat);
    Smartconfig->msgformat=(unsigned char)msgformat;
    Smartconfig->reserve  =0;
    Smartconfig->globalId =globalId;

    count=sizeof(AML_Smartconfig_type);
    strcpy(netBuf + count,gb2312);

    ret=count + strlen(gb2312);
    Smartconfig->fixedHeader.len =(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
    LOGI("Request broadcast=%d\n",Smartconfig->fixedHeader.header.broadcast);
    LOGI("Request messageType=%d\n",Smartconfig->fixedHeader.header.messageType);
    LOGI("Request relayFlag=%d\n",Smartconfig->fixedHeader.header.relayFlag);
    LOGI("Request len=%d\n",Smartconfig->fixedHeader.len);

    LOGI("Request msgformat=%d\n",Smartconfig->msgformat);
#endif //AML_DEBUG_INFO

    return ret;
}

int AmlSmartconfigToByteUserStateReq(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{   AML_Smartconfig_type * Smartconfig=(AML_Smartconfig_type *)(netBuf);
    static int ret,count,msgformat;

    Smartconfig->fixedHeader.header = fixedHeader.header;
    Smartconfig->fixedHeader.len=0;

    sscanf(ascii,"%d",&msgformat);

    LOGD("UserStateReq msgformat=%d\n",msgformat);
    Smartconfig->msgformat=(unsigned char)msgformat;
    Smartconfig->reserve  =0;

    count=sizeof(AML_Smartconfig_type)-4;

    ret=count;
    Smartconfig->fixedHeader.len =(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
    LOGI("UserStateReq broadcast=%d\n",Smartconfig->fixedHeader.header.broadcast);
    LOGI("UserStateReq messageType=%d\n",Smartconfig->fixedHeader.header.messageType);
    LOGI("UserStateReq relayFlag=%d\n",Smartconfig->fixedHeader.header.relayFlag);
    LOGI("UserStateReq len=%d\n",Smartconfig->fixedHeader.len);

    LOGI("UserStateReq msgformat=%d\n",Smartconfig->msgformat);
#endif //AML_DEBUG_INFO

    return ret;
}

int UserPermToByte(const char *pbuf,char *rebuf)
{   AML_UserPerm_type * UserPerm=NULL;
    int i,ret,len,ilen,slen,count=0,wan;
    unsigned long  gId;
    char buf[200];

#ifdef AML_DEBUG_INFO
    LOGI("UserPermToByte pbuf:%s\n",pbuf);
#endif //AML_DEBUG_INFO

     slen= sizeof(AML_UserPerm_type);
     len=strlen(pbuf);
     ilen=0;

     ret = strcspn(pbuf,"+");
     if(ret <= 0){
         LOGE("UserPermToByte  'pbuf' is NULL!\n");
         return 0;
     }
     memset(buf,0,sizeof(buf));
     memcpy(buf,pbuf,ret);
     sscanf(buf,"%d|%d",(int *)&wan,(int *)&gId);
     ilen=ilen+ret+1;
     UserPerm=(AML_UserPerm_type *)(rebuf);
     UserPerm->globalId=gId;
     UserPerm->wan=wan;
     count++;

     LOGD("UserPermToByte buf=%s wan=%d gId=%d\n",buf, UserPerm->wan,(int)UserPerm->globalId);
     for(i=ilen;i<=len;){
         ret = strcspn(pbuf+i,"+");
         if(ret <= 0){
             break;
         }
         memset(buf,0,sizeof(buf));
         memcpy(buf,pbuf+i,ret);
         sscanf(buf,"%d",(int *)&gId);

         UserPerm=(AML_UserPerm_type *)(rebuf+(count * slen));
         UserPerm->globalId=gId;
         UserPerm->wan=wan;
         count++;
         i=i+ret+1;
         LOGD("buf=%s gId=%d\n",buf,UserPerm->globalId);
     }

    ret=count*slen;
    return ret;
}

int AmlSmartconfigToByteUserPermissionReq(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{   AML_Smartconfig_type * Smartconfig=(AML_Smartconfig_type *)(netBuf);
    static int ret,len,count,rlen,plen,msgformat;
    char pbuf[200];

#ifdef AML_DEBUG_INFO
    LOGI("AmlSmartconfigToByteUserPermissionReq ascii:%s gb2312:%s\n",ascii,gb2312);
#endif //AML_DEBUG_INFO

    Smartconfig->fixedHeader.header = fixedHeader.header;
    Smartconfig->fixedHeader.len=0;
    sscanf(ascii,"%d,",&msgformat);

    Smartconfig->msgformat=(unsigned char)msgformat;
    Smartconfig->reserve  =0;
    count=sizeof(AML_Smartconfig_type)-4;

#ifdef AML_DEBUG_INFO
    LOGI("UserPermissionReq broadcast=%d\n",Smartconfig->fixedHeader.header.broadcast);
    LOGI("UserPermissionReq messageType=%d\n",Smartconfig->fixedHeader.header.messageType);
    LOGI("UserPermissionReq relayFlag=%d\n",Smartconfig->fixedHeader.header.relayFlag);
    LOGI("UserPermissionReq len=%d\n",Smartconfig->fixedHeader.len);
    LOGI("UserPermissionReq msgformat=%d\n",Smartconfig->msgformat);
#endif //AML_DEBUG_INFO

    len=strlen(ascii);
    ret = strcspn(ascii,";");
    if(ret <= 0){
        LOGE("AmlSmartconfigToByteUserPermissionReq  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii,ret);
    rlen=UserPermToByte(pbuf+2,netBuf+count);
    plen=rlen;
    LOGI("UserPermissionReq rlen=%d\n",rlen);

    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+1+ret,len-1-ret);
    rlen=UserPermToByte(pbuf,netBuf+count+plen);
    LOGI("UserPermissionReq rlen=%d\n",rlen);
    plen=plen+rlen;

    ret=count+plen;
    Smartconfig->fixedHeader.len =(unsigned char)(ret-2);

    return ret;
}

int AmlGetInformationToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{   char pbuf[AML_FRAME_Buff];
    static int ret,count,msgformat;

    count=0;
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlGetInformationToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",&msgformat);

#ifdef AML_DEBUG_INFO
    LOGD("AmlGetInformationToByte msgformat=%d\n",msgformat);
#endif //AML_DEBUG_INFO

    switch (msgformat){
        case AML_GetInfo_Request:
            ret=AmlGetInformationToByteRequest(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        case AML_GetInfo_Response:
            ret=AmlGetInformationToByteResponse(fixedHeader,ascii,gb2312,netBuf);
            if(ret>0){
                return ret;
            }
            break;
        default:
            break;
    }

    return ret;
}

int AmlGetInformationToByteRequest(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{    Aml_GetInformation_type *getInformation=(Aml_GetInformation_type *)(netBuf);
    char pbuf[AML_FRAME_Buff];
    static int ret,len,count,msgformat,netNum,dstApid,srcApid,cmd;
    static unsigned long  dstGlobalId,srcGlobalId;
    getInformation->fixedHeader.header = fixedHeader.header;
    getInformation->fixedHeader.len=0;

    count=0;
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlGetInformationToByteRequest  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",&msgformat);

    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlGetInformationToByteRequest  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",&netNum);

    len=strlen(ascii);
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlGetInformationToByteRequest  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    timeStrToStruct(pbuf,&getInformation->time);

    memset(pbuf,0,sizeof(pbuf));
    strcpy(pbuf,ascii+count);
    sscanf(pbuf,"%d,%d,%d,%d,%d",&dstApid,(int *)&dstGlobalId,&srcApid,(int *)&srcGlobalId,&cmd);

    getInformation->msgformat=(unsigned char)msgformat;
    getInformation->netNum   =(unsigned char)netNum;
    getInformation->dstApid  =(unsigned char)dstApid;
    getInformation->dstGlobalId  =dstGlobalId;
    getInformation->srcApid  =(unsigned char)srcApid;
    getInformation->srcGlobalId  =srcGlobalId;

    len = sizeof(Aml_GetInformation_type);
    netBuf[len]=(unsigned char)cmd;
    ret=len+1;

    getInformation->fixedHeader.len=(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
    LOGI("AmlGetInformationToByteRequest broadcast=%d\n",getInformation->fixedHeader.header.broadcast);
    LOGI("AmlGetInformationToByteRequest messageType=%d\n",getInformation->fixedHeader.header.messageType);
    LOGI("AmlGetInformationToByteRequest relayFlag=%d\n",getInformation->fixedHeader.header.relayFlag);
    LOGI("AmlGetInformationToByteRequest len=%d\n",getInformation->fixedHeader.len);

    LOGI("AmlGetInformationToByteRequest msgformat=%d\n",getInformation->msgformat);
    LOGI("AmlGetInformationToByteRequest netNum=%d\n",getInformation->netNum);

    LOGI("AmlGetInformationToByteRequest year=%d\n",getInformation->time.year);
    LOGI("AmlGetInformationToByteRequest month=%d\n",getInformation->time.month);
    LOGI("AmlGetInformationToByteRequest day=%d\n",getInformation->time.day);
    LOGI("AmlGetInformationToByteRequest hour=%d\n",getInformation->time.hour);
    LOGI("AmlGetInformationToByteRequest minute=%d\n",getInformation->time.minute);
    LOGI("AmlGetInformationToByteRequest second=%d\n",getInformation->time.second);

    LOGI("AmlGetInformationToByteRequest dstApid=%d\n",getInformation->dstApid);
    LOGI("AmlGetInformationToByteRequest dstGlobalId=%d\n",(int)getInformation->dstGlobalId);
    LOGI("AmlGetInformationToByteRequest srcApid=%d\n",getInformation->srcApid);
    LOGI("AmlGetInformationToByteRequest srcGlobalId=%d\n",(int)getInformation->srcGlobalId);
    LOGI("AmlGetInformationToByteRequest cmd=%d\n",cmd);

#endif //AML_DEBUG_INFO

    return ret;
}

int AmlGetInformationToByteResponse(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{   Aml_GetInformation_type *getInformation=(Aml_GetInformation_type *)(netBuf);
    AML_GetInfoRes_type *getInfoRes=NULL;
    char pbuf[AML_FRAME_Buff];
    static int ret,len,ilen,count,msgformat,netNum,dstApid,userId,srcApid,cmd,messageId,Enctype,errorCode;
    static unsigned long  dstGlobalId,srcGlobalId;

    getInformation->fixedHeader.header = fixedHeader.header;
    getInformation->fixedHeader.len=0;

    count=0;
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlGetInformationToByteResponse  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",&msgformat);

    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlGetInformationToByteResponse  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",&netNum);

    len=strlen(ascii);
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlGetInformationToByteResponse  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    timeStrToStruct(pbuf,&getInformation->time);

    memset(pbuf,0,sizeof(pbuf));
    strcpy(pbuf,ascii+count);

    sscanf(pbuf,"%d,%d,%d-%d,%d,%d,%d,%d,%d",&dstApid,&Enctype,(int *)&dstGlobalId,&userId,&srcApid,(int *)&srcGlobalId,&messageId,&cmd,&errorCode);

    getInformation->msgformat=(unsigned char)msgformat;
    getInformation->netNum   =(unsigned char)netNum;
    getInformation->dstApid  =(unsigned char)dstApid;
    getInformation->dstGlobalId  =dstGlobalId;
    getInformation->srcApid   =(unsigned char)srcApid;
    getInformation->srcGlobalId  =srcGlobalId;

    len = sizeof(Aml_GetInformation_type);
#ifdef AML_DEBUG_INFO
    LOGI("AmlGetInformationToByteResponse sizeof len=%d\n",len);
#endif //AML_DEBUG_INFO

    getInfoRes=netBuf+len;
    getInfoRes->messageId=(unsigned short)messageId;
    getInfoRes->cmd=cmd;
    getInfoRes->user.Enctype=Enctype;
    getInfoRes->user.userId=userId;

    ilen=len+sizeof(AML_GetInfoRes_type);
    netBuf[ilen]=errorCode;
    //getInfoRes->errorCode=(unsigned char)errorCode;

#ifdef AML_DEBUG_INFO
    LOGI("AmlGetInformationToByteResponse messageId=%d\n",getInfoRes->messageId);
    LOGI("AmlGetInformationToByteResponse cmd=%d\n",getInfoRes->cmd);
    LOGI("AmlGetInformationToByteResponse errorCode=%d\n",errorCode);
#endif //AML_DEBUG_INFO

    count=sizeof(AML_GetInfoRes_type);
    count=len+count;
    len=strlen(gb2312);
    strncpy(netBuf+count,gb2312,len);
    ret=count+len;

    getInformation->fixedHeader.len=(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
    LOGI("AmlGetInformationToByteResponse broadcast=%d\n",getInformation->fixedHeader.header.broadcast);
    LOGI("AmlGetInformationToByteResponse messageType=%d\n",getInformation->fixedHeader.header.messageType);
    LOGI("AmlGetInformationToByteResponse relayFlag=%d\n",getInformation->fixedHeader.header.relayFlag);
    LOGI("AmlGetInformationToByteResponse len=%d\n",getInformation->fixedHeader.len);

    LOGI("AmlGetInformationToByteResponse msgformat=%d\n",getInformation->msgformat);
    LOGI("AmlGetInformationToByteResponse netNum=%d\n",getInformation->netNum);

    LOGI("AmlGetInformationToByteResponse year=%d\n",getInformation->time.year);
    LOGI("AmlGetInformationToByteResponse month=%d\n",getInformation->time.month);
    LOGI("AmlGetInformationToByteResponse day=%d\n",getInformation->time.day);
    LOGI("AmlGetInformationToByteResponse hour=%d\n",getInformation->time.hour);
    LOGI("AmlGetInformationToByteResponse minute=%d\n",getInformation->time.minute);
    LOGI("AmlGetInformationToByteResponse second=%d\n",getInformation->time.second);

    LOGI("AmlGetInformationToByteResponse dstApid=%d\n",getInformation->dstApid);
    LOGI("AmlGetInformationToByteResponse Enctype=%d\n",getInfoRes->user.Enctype);
    LOGI("AmlGetInformationToByteResponse userId=%d\n",getInfoRes->user.userId);
    LOGI("AmlGetInformationToByteResponse dstGlobalId=%d\n",(int)getInformation->dstGlobalId);
    LOGI("AmlGetInformationToByteResponse srcApid=%d\n",getInformation->srcApid);
    LOGI("AmlGetInformationToByteResponse srcGlobalId=%d\n",(int)getInformation->srcGlobalId);
    LOGI("AmlGetInformationToByteResponse messageId=%d\n",getInfoRes->messageId);
    LOGI("AmlGetInformationToByteResponse cmd=%d\n",getInfoRes->cmd);
    LOGI("AmlGetInformationToByteResponse errorCode=%d\n",errorCode);
#endif //AML_DEBUG_INFO

    return ret;
}

int AmlSendTextToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{    Aml_SendTextMsg_type *sendTextMsg=(Aml_SendTextMsg_type *)(netBuf);
    char pbuf[AML_FRAME_Buff];
    static int ret,len,count,Group,Enctype,netNum,dstApid,srcApid,userId;
    unsigned short messageId;
    static unsigned long  dstGlobalId,srcGlobalId;

    sendTextMsg->fixedHeader.header = fixedHeader.header;
    sendTextMsg->fixedHeader.len=0;

    count=0;
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendTextToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",(unsigned int *)&messageId);

    len=strlen(ascii);
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendTextToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    timeStrToStruct(pbuf,&sendTextMsg->time);

    memset(pbuf,0,sizeof(pbuf));
    strcpy(pbuf,ascii+count);

    sscanf(pbuf,"%d,%d,%d,%d-%d,%d,%d,%d",&dstApid,&Group,&Enctype,(int *)&dstGlobalId,&userId,&srcApid,(int *)&srcGlobalId,&netNum);

    sendTextMsg->messageId       =messageId;
    sendTextMsg->dstApid         =(unsigned char)dstApid;
    sendTextMsg->User.Group_Super=(unsigned char)Group;
    sendTextMsg->User.Enctype    =(unsigned char)Enctype;
    sendTextMsg->User.userId     =(unsigned char)userId;
    sendTextMsg->dstGlobalId     =dstGlobalId;
    sendTextMsg->srcApid         =(unsigned char)srcApid;
    sendTextMsg->srcGlobalId     =srcGlobalId;

    count=sizeof(Aml_SendTextMsg_type);
    netBuf[count]=(unsigned char)netNum;
    count=count+1;

    len=strlen(gb2312);
    memcpy(netBuf+count,gb2312,len);
    ret=count+len;
    sendTextMsg->fixedHeader.len=(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
    LOGI("AmlSendTextToByte broadcast=%d\n",sendTextMsg->fixedHeader.header.broadcast);
    LOGI("AmlSendTextToByte messageType=%d\n",sendTextMsg->fixedHeader.header.messageType);
    LOGI("AmlSendTextToByte relayFlag=%d\n",sendTextMsg->fixedHeader.header.relayFlag);
    LOGI("AmlSendTextToByte len=%d\n",sendTextMsg->fixedHeader.len);

    LOGI("AmlSendTextToByte messageId=%d\n",sendTextMsg->messageId);

    LOGI("AmlSendTextToByte year=%d\n",sendTextMsg->time.year);
    LOGI("AmlSendTextToByte month=%d\n",sendTextMsg->time.month);
    LOGI("AmlSendTextToByte day=%d\n",sendTextMsg->time.day);
    LOGI("AmlSendTextToByte hour=%d\n",sendTextMsg->time.hour);
    LOGI("AmlSendTextToByte minute=%d\n",sendTextMsg->time.minute);
    LOGI("AmlSendTextToByte second=%d\n",sendTextMsg->time.second);

    LOGI("AmlSendTextToByte dstApid=%d\n",sendTextMsg->dstApid);
    LOGI("AmlSendTextToByte Group=%d\n",sendTextMsg->User.Group_Super);
    LOGI("AmlSendTextToByte Enctype=%d\n",sendTextMsg->User.Enctype);
    LOGI("AmlSendTextToByte dstGlobalId=%d\n",(int)sendTextMsg->dstGlobalId);
    LOGI("AmlSendTextToByte userId=%d\n",sendTextMsg->User.userId);
    LOGI("AmlSendTextToByte srcApid=%d\n",sendTextMsg->srcApid);
    LOGI("AmlSendTextToByte srcGlobalId=%d\n",(int)sendTextMsg->srcGlobalId);
    LOGI("AmlSendTextToByte netNum=%d\n",netNum);

#endif //AML_DEBUG_INFO

    return ret;
}

int AmlSendAcronymToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{    Aml_SendTextMsg_type *sendTextMsg=(Aml_SendTextMsg_type *)(netBuf);
    char pbuf[AML_FRAME_Buff];
    static int ret,len,count,Group,Enctype,netNum,userId,dstApid,srcApid,Acronym;
    unsigned short messageId;
    static unsigned long  dstGlobalId,srcGlobalId;

    sendTextMsg->fixedHeader.header = fixedHeader.header;
    sendTextMsg->fixedHeader.len=0;

    count=0;
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendAcronymToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",(unsigned int *)&messageId);

    len=strlen(ascii);
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendAcronymToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    timeStrToStruct(pbuf,&sendTextMsg->time);

    memset(pbuf,0,sizeof(pbuf));
    strcpy(pbuf,ascii+count);
    sscanf(pbuf,"%d,%d,%d-%d,%d,%d,%d,%d",&dstApid,&Group,(int *)&dstGlobalId,&userId,&srcApid,(int *)&srcGlobalId,&netNum,&Acronym);

    sendTextMsg->messageId       =messageId;
    sendTextMsg->dstApid         =(unsigned char)dstApid;
    sendTextMsg->User.Group_Super=(unsigned char)Group;
    sendTextMsg->User.Enctype    =(unsigned char)Enctype;
    sendTextMsg->dstGlobalId     =dstGlobalId;
    sendTextMsg->User.userId     =(unsigned char)userId;
    sendTextMsg->srcApid         =(unsigned char)srcApid;
    sendTextMsg->srcGlobalId     =srcGlobalId;

    count=sizeof(Aml_SendTextMsg_type);
    netBuf[count]=(unsigned char)netNum;
    count=count+1;

    netBuf[count]=(unsigned char)Acronym;
    count=count+1;

    len=strlen(gb2312);
    memcpy(netBuf+count,gb2312,len);
    ret=count+len;
    sendTextMsg->fixedHeader.len=(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
    LOGI("AmlSendAcronymToByte broadcast=%d\n",sendTextMsg->fixedHeader.header.broadcast);
    LOGI("AmlSendAcronymToByte messageType=%d\n",sendTextMsg->fixedHeader.header.messageType);
    LOGI("AmlSendAcronymToByte relayFlag=%d\n",sendTextMsg->fixedHeader.header.relayFlag);
    LOGI("AmlSendAcronymToByte len=%d\n",sendTextMsg->fixedHeader.len);

    LOGI("AmlSendAcronymToByte messageId=%d\n",sendTextMsg->messageId);

    LOGI("AmlSendAcronymToByte year=%d\n",sendTextMsg->time.year);
    LOGI("AmlSendAcronymToByte month=%d\n",sendTextMsg->time.month);
    LOGI("AmlSendAcronymToByte day=%d\n",sendTextMsg->time.day);
    LOGI("AmlSendAcronymToByte hour=%d\n",sendTextMsg->time.hour);
    LOGI("AmlSendAcronymToByte minute=%d\n",sendTextMsg->time.minute);
    LOGI("AmlSendAcronymToByte second=%d\n",sendTextMsg->time.second);

    LOGI("AmlSendAcronymToByte dstApid=%d\n",sendTextMsg->dstApid);
    LOGI("AmlSendAcronymToByte Group=%d\n",sendTextMsg->User.Group_Super);
    LOGI("AmlSendAcronymToByte Enctype=%d\n",sendTextMsg->User.Enctype);
    LOGI("AmlSendAcronymToByte dstGlobalId=%d\n",(int)sendTextMsg->dstGlobalId);
    LOGI("AmlSendAcronymToByte userId=%d\n",sendTextMsg->User.userId);
    LOGI("AmlSendAcronymToByte srcApid=%d\n",sendTextMsg->srcApid);
    LOGI("AmlSendAcronymToByte srcGlobalId=%d\n",(int)sendTextMsg->srcGlobalId);
    LOGI("AmlSendAcronymToByte netNum=%d\n",netNum);
    LOGI("AmlSendAcronymToByte Acronym=%d\n",Acronym);

#endif //AML_DEBUG_INFO

    return ret;
}

int AmlSendVoiceToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{    Aml_SendTextMsg_type *sendTextMsg=(Aml_SendTextMsg_type *)(netBuf);
    char pbuf[AML_FRAME_Buff];
    static int ret,len,count,Group,Enctype,netNum,dstApid,srcApid;
    unsigned short messageId;
    static unsigned long  dstGlobalId,srcGlobalId;

    sendTextMsg->fixedHeader.header = fixedHeader.header;
    sendTextMsg->fixedHeader.len=0;

    LOGI("AmlSendVoiceToByte ascii=%s gb2312=%s\n",ascii,gb2312);

    count=0;
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendVoiceToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",(unsigned int *)&messageId);

    len=strlen(ascii);
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendVoiceToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    timeStrToStruct(pbuf,&sendTextMsg->time);

    memset(pbuf,0,sizeof(pbuf));
    strcpy(pbuf,ascii+count);

    sscanf(pbuf,"%d,%d,%d,%d,%d,%d,%d",&dstApid,&Group,&Enctype,(int *)&dstGlobalId,&srcApid,(int *)&srcGlobalId,&netNum);

    sendTextMsg->messageId       =messageId;
    sendTextMsg->dstApid         =(unsigned char)dstApid;
    sendTextMsg->User.Group_Super=(unsigned char)Group;
    sendTextMsg->User.Enctype    =(unsigned char)Enctype;
    sendTextMsg->dstGlobalId     =dstGlobalId;
    sendTextMsg->srcApid         =(unsigned char)srcApid;
    sendTextMsg->srcGlobalId     =srcGlobalId;

    count=sizeof(Aml_SendTextMsg_type);
    netBuf[count]=(unsigned char)netNum;
    count=count+1;

    len=strlen(gb2312);
    memcpy(netBuf+count,gb2312,len);
    ret=count+len;
    sendTextMsg->fixedHeader.len=(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
    LOGI("AmlSendVoiceToByte broadcast=%d\n",sendTextMsg->fixedHeader.header.broadcast);
    LOGI("AmlSendVoiceToByte messageType=%d\n",sendTextMsg->fixedHeader.header.messageType);
    LOGI("AmlSendVoiceToByte relayFlag=%d\n",sendTextMsg->fixedHeader.header.relayFlag);
    LOGI("AmlSendVoiceToByte len=%d\n",sendTextMsg->fixedHeader.len);

    LOGI("AmlSendVoiceToByte messageId=%d\n",sendTextMsg->messageId);

    LOGI("AmlSendVoiceToByte year=%d\n",sendTextMsg->time.year);
    LOGI("AmlSendVoiceToByte month=%d\n",sendTextMsg->time.month);
    LOGI("AmlSendVoiceToByte day=%d\n",sendTextMsg->time.day);
    LOGI("AmlSendVoiceToByte hour=%d\n",sendTextMsg->time.hour);
    LOGI("AmlSendVoiceToByte minute=%d\n",sendTextMsg->time.minute);
    LOGI("AmlSendVoiceToByte second=%d\n",sendTextMsg->time.second);

    LOGI("AmlSendVoiceToByte dstApid=%d\n",sendTextMsg->dstApid);
    LOGI("AmlSendVoiceToByte Group=%d\n",sendTextMsg->User.Group_Super);
    LOGI("AmlSendVoiceToByte Enctype=%d\n",sendTextMsg->User.Enctype);
    LOGI("AmlSendVoiceToByte dstGlobalId=%d\n",(int)sendTextMsg->dstGlobalId);
    LOGI("AmlSendVoiceToByte srcApid=%d\n",sendTextMsg->srcApid);
    LOGI("AmlSendVoiceToByte srcGlobalId=%d\n",(int)sendTextMsg->srcGlobalId);
    LOGI("AmlSendVoiceToByte netNum=%d\n",netNum);

#endif //AML_DEBUG_INFO

    return ret;
}


int AmlSyncMsgToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{   Aml_SyncMsg_type     *syncMsg=(Aml_SyncMsg_type *)(netBuf);
    Aml_SyncMsgId_type   *syncMsgId=NULL;
    Aml_SyncMsgTime_type *syncMsgTime=NULL;
    char pbuf[AML_FRAME_Buff];
    static int ret,len,count,netNum,msgformat;
    static unsigned short msgIdStart,msgIdEnd;
    static unsigned long       globalId;

    syncMsg->fixedHeader.header = fixedHeader.header;
    syncMsg->fixedHeader.len=0;

    count=0;
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSyncMsgToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",&msgformat);

    switch (msgformat){
       case AML_SendSync_ID:
            sscanf(ascii+count,"%d,%u,%d,%d",&netNum,(unsigned int *)&globalId,(unsigned int *)&msgIdStart,(unsigned int *)&msgIdEnd);
            syncMsg->msgformat=(unsigned char)msgformat;
            syncMsg->netNum   =(unsigned char)netNum;
            syncMsg->globalId =globalId;

            count=sizeof(Aml_SyncMsg_type);
            syncMsgId=(Aml_SyncMsgId_type *)(netBuf+count);
            syncMsgId->msgIdStart=msgIdStart;
            syncMsgId->msgIdEnd  =msgIdEnd;

            ret=count+sizeof(Aml_SyncMsgId_type);
            syncMsg->fixedHeader.len=(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
            LOGI("AmlSyncMsgToByte broadcast=%d\n",syncMsg->fixedHeader.header.broadcast);
            LOGI("AmlSyncMsgToByte messageType=%d\n",syncMsg->fixedHeader.header.messageType);
            LOGI("AmlSyncMsgToByte relayFlag=%d\n",syncMsg->fixedHeader.header.relayFlag);
            LOGI("AmlSyncMsgToByte len=%d\n",syncMsg->fixedHeader.len);

            LOGI("AmlSyncMsgToByte msgformat=%d\n",syncMsg->msgformat);
            LOGI("AmlSyncMsgToByte netNum=%d\n",syncMsg->netNum);
            LOGI("AmlSyncMsgToByte globalId=%d\n",(int)syncMsg->globalId);

            LOGI("AmlSyncMsgToByte msgIdStart=%d\n",syncMsgId->msgIdStart);
            LOGI("AmlSyncMsgToByte msgIdEnd=%d\n",syncMsgId->msgIdEnd);
#endif //AML_DEBUG_INFO
            break;
       case AML_SendSync_TimeQuantum:
       case AML_SendSync_LoginAndLogout:

            ret = strcspn(ascii+count,",");
            if(ret <= 0){
                LOGE("AmlSyncMsgToByte  'ascii' is NULL!\n");
                return 0;
            }
            memset(pbuf,0,sizeof(pbuf));
            memcpy(pbuf,ascii+count,ret);
            count=count+ret+1;
            sscanf(pbuf,"%d",&netNum);

            ret = strcspn(ascii+count,",");
            if(ret <= 0){
                LOGE("AmlSyncMsgToByte  'ascii' is NULL!\n");
                return 0;
            }
            memset(pbuf,0,sizeof(pbuf));
            memcpy(pbuf,ascii+count,ret);
            count=count+ret+1;
            sscanf(pbuf,"%d",(unsigned int *)&globalId);

            syncMsg->msgformat=(unsigned char)msgformat;
            syncMsg->netNum   =(unsigned char)netNum;
            syncMsg->globalId =globalId;

            len=sizeof(Aml_SyncMsg_type);
            syncMsgTime=(Aml_SyncMsgTime_type *)(netBuf+len);

            ret = strcspn(ascii+count,",");
            if(ret <= 0){
                LOGE("AmlSyncMsgToByte  'ascii' is NULL!\n");
                return 0;
            }
            memset(pbuf,0,sizeof(pbuf));
            memcpy(pbuf,ascii+count,ret);
            count=count+ret+1;
            timeStrToStruct(pbuf,&syncMsgTime->timeStart);

            ret = strcspn(ascii+count,",");
            if(ret <= 0){
                LOGE("AmlSyncMsgToByte  'ascii' is NULL!\n");
                return 0;
            }
            memset(pbuf,0,sizeof(pbuf));
            memcpy(pbuf,ascii+count,ret);
            count=count+ret+1;
            timeStrToStruct(pbuf,&syncMsgTime->timeEnd);

            ret=sizeof(Aml_SyncMsg_type)+sizeof(Aml_SyncMsgTime_type);
            syncMsg->fixedHeader.len=(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
            LOGI("AmlSyncMsgToByte broadcast=%d\n",syncMsg->fixedHeader.header.broadcast);
            LOGI("AmlSyncMsgToByte messageType=%d\n",syncMsg->fixedHeader.header.messageType);
            LOGI("AmlSyncMsgToByte relayFlag=%d\n",syncMsg->fixedHeader.header.relayFlag);
            LOGI("AmlSyncMsgToByte len=%d\n",syncMsg->fixedHeader.len);

            LOGI("AmlSyncMsgToByte msgformat=%d\n",syncMsg->msgformat);
            LOGI("AmlSyncMsgToByte netNum=%d\n",syncMsg->netNum);
            LOGI("AmlSyncMsgToByte globalId=%d\n",(int)syncMsg->globalId);

            LOGI("AmlSyncMsgToByte timeStart.year=%d\n",syncMsgTime->timeStart.year);
            LOGI("AmlSyncMsgToByte timeStart.month=%d\n",syncMsgTime->timeStart.month);
            LOGI("AmlSyncMsgToByte timeStart.day=%d\n",syncMsgTime->timeStart.day);
            LOGI("AmlSyncMsgToByte timeStart.hour=%d\n",syncMsgTime->timeStart.hour);
            LOGI("AmlSyncMsgToByte timeStart.minute=%d\n",syncMsgTime->timeStart.minute);
            LOGI("AmlSyncMsgToByte timeStart.second=%d\n",syncMsgTime->timeStart.second);

            LOGI("AmlSyncMsgToByte timeEnd.year=%d\n",syncMsgTime->timeEnd.year);
            LOGI("AmlSyncMsgToByte timeEnd.month=%d\n",syncMsgTime->timeEnd.month);
            LOGI("AmlSyncMsgToByte timeEnd.day=%d\n",syncMsgTime->timeEnd.day);
            LOGI("AmlSyncMsgToByte timeEnd.hour=%d\n",syncMsgTime->timeEnd.hour);
            LOGI("AmlSyncMsgToByte timeEnd.minute=%d\n",syncMsgTime->timeEnd.minute);
            LOGI("AmlSyncMsgToByte timeEnd.second=%d\n",syncMsgTime->timeEnd.second);
#endif //AML_DEBUG_INFO
            break;
       default:
            break;
    }

    return ret;
}

int AmlSendGroupMsgToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf)
{   Aml_GroupMsg_type     *groupMsg=(Aml_GroupMsg_type *)(netBuf);
    char pbuf[AML_FRAME_Buff];
    static int ret,len,ilen,count,netNum,srcApid,Group,Enctype,msgId;
    static unsigned long  GlobalId;

    groupMsg->fixedHeader.header = fixedHeader.header;
    groupMsg->fixedHeader.len=0;

#ifdef AML_DEBUG_INFO
    LOGD("AmlSendGroupMsgToByte ascii=%s gb2312=%s\n",ascii,gb2312);
#endif //AML_DEBUG_INFO

    count=0;
    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendGroupMsgToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",&msgId);

    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendGroupMsgToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    timeStrToStruct(pbuf,&groupMsg->time);

    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendGroupMsgToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",&srcApid);

    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendGroupMsgToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",&Enctype);

    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendGroupMsgToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d-%d",(int *)&GlobalId,&Group);

    ret = strcspn(ascii+count,",");
    if(ret <= 0){
        LOGE("AmlSendGroupMsgToByte  'ascii' is NULL!\n");
        return 0;
    }
    memset(pbuf,0,sizeof(pbuf));
    memcpy(pbuf,ascii+count,ret);
    count=count+ret+1;
    sscanf(pbuf,"%d",&netNum);

    groupMsg->messageId   =msgId;
    groupMsg->srcApid     =(unsigned char)srcApid;
    groupMsg->User.Enctype=(unsigned char)Enctype;
    groupMsg->User.userId =(unsigned char)Group;
    groupMsg->GlobalId    =GlobalId;
    groupMsg->netNum      =(unsigned char)netNum;
    ilen=strlen(gb2312);

    groupMsg->nameLen=(unsigned char)(ilen >AML_UserName_MAX_LEN ? AML_UserName_MAX_LEN : ilen);

    len= sizeof(Aml_GroupMsg_type);
    strncpy(netBuf+len,gb2312,groupMsg->nameLen);
    ilen=GroupStrToStruct(ascii+count,netBuf + groupMsg->nameLen+len);
    ret=len+groupMsg->nameLen+ilen;
    groupMsg->fixedHeader.len=(unsigned char)(ret-2);

#ifdef AML_DEBUG_INFO
    LOGI("AmlSendGroupMsgToByte broadcast=%d\n",groupMsg->fixedHeader.header.broadcast);
    LOGI("AmlSendGroupMsgToByte messageType=%d\n",groupMsg->fixedHeader.header.messageType);
    LOGI("AmlSendGroupMsgToByte relayFlag=%d\n",groupMsg->fixedHeader.header.relayFlag);
    LOGI("AmlSendGroupMsgToByte len=%d\n",groupMsg->fixedHeader.len);

    LOGI("AmlSendGroupMsgToByte messageId=%d\n",groupMsg->messageId);
    LOGI("AmlSendGroupMsgToByte srcApid=%d\n",groupMsg->srcApid);
    LOGI("AmlSendGroupMsgToByte Enctype=%d\n",groupMsg->User.Enctype);
    LOGI("AmlSendGroupMsgToByte GlobalId=%d\n",groupMsg->GlobalId);
    LOGI("AmlSendGroupMsgToByte Group=%d\n",groupMsg->User.userId);
    LOGI("AmlSendGroupMsgToByte netNum=%d\n",groupMsg->netNum);
#endif //AML_DEBUG_INFO

    return ret;
}

int GroupStrToStruct(const char *ascii,char *netBuf)
{   int ret=0,i,len,count=0;
    char pbuf[200];
    unsigned long *Gid =NULL;

    len=strlen(ascii);
    for(i=0;i<len;){
        ret = strcspn(ascii+i,"+");
        if(ret <= 0){
            LOGE("GroupStrToStruct  'ascii' is NULL!\n");
            return 0;
        }

        Gid=(unsigned long *)(netBuf+(count*4));
        memset(pbuf,0,sizeof(pbuf));
        memcpy(pbuf,ascii+i,ret);
        sscanf(pbuf,"%d",Gid);
        i=i+ret+1;

#ifdef AML_DEBUG_INFO
        LOGD("GroupStrToStruct pbuf=%s Gid=%d\n",pbuf,*Gid);
#endif //AML_DEBUG_INFO
        count++;
    }
    ret=count*4;

#ifdef AML_DEBUG_INFO
    LOGD("GroupStrToStruct ret=%d\n",ret);
#endif //AML_DEBUG_INFO

    return ret;
}

