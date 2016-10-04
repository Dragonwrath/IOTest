//
// Created by smxcyp on 2016/6/27.
//

#ifndef AMLUDP_AMLUDP_H
#define AMLUDP_AMLUDP_H

#include <android/log.h>

//#define AML_DEBUG_INFO                1

#define LOG    "AMLUDP"
#define LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG,__VA_ARGS__)
#define LOGI(...)  __android_log_print(ANDROID_LOG_INFO,LOG,__VA_ARGS__)
#define LOGW(...)  __android_log_print(ANDROID_LOG_WARN,LOG,__VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG,__VA_ARGS__)
#define LOGF(...)  __android_log_print(ANDROID_LOG_FATAL,LOG,__VA_ARGS__)

#define NULL   ((void *)0)
#define AML_UserName_MAX_LEN          40
#define AML_FRAME_MAX_SIZE	          180
#define AML_FRAME_Buff	              512

#define AML_Broadcast_Local      0
#define AML_Broadcast_WAN        1
#define AML_Broadcast_Super      2
#define AML_Broadcast_Reserver   3

#define AML_MessageType_Smartconfig   0
#define AML_MessageType_Login         1
#define AML_MessageType_Logout        2
#define AML_MessageType_GetInfo       3
#define AML_MessageType_SendText      4
#define AML_MessageType_SendAcronym   5
#define AML_MessageType_SendVoice     6
#define AML_MessageType_SendGroup     7
#define AML_MessageType_SendSync      8


#define AML_Smartconfig_Request            1
#define AML_Smartconfig_Response           2
#define AML_Smartconfig_UserStateReq       3
#define AML_Smartconfig_UserStateRes       4
#define AML_Smartconfig_UserPermissionReq  5
#define AML_Smartconfig_UserPermissionRes  6

#define AML_GetInfo_Request            1
#define AML_GetInfo_Response           2

#define AML_SendSync_ID             1
#define AML_SendSync_TimeQuantum    2
#define AML_SendSync_LoginAndLogout 3

#define AML_GroupName_MAX_LEN          38
#define AML_UserState_MAX_NUM          15

#define AML_DATE_BASE_YEAR	          2016

#define AML_RelayFlag_TRUE      1
#define AML_RelayFlag_False     0


typedef struct HEADER_T{
    char            :1;
    char relayFlag  :1;
    char messageType:4;
    char broadcast  :2;
}Header_type;

typedef struct FIXED_HEADER_T{
    Header_type  header;
    unsigned char	    len;
}FixedHeader_type;

typedef struct TIME_T{
    unsigned  year  :6;
    unsigned  month :4;
    unsigned  day   :5;
    unsigned  hour  :5;
    unsigned  minute:6;
    unsigned  second:6;
}Time_type;

typedef struct USER_INFO_T{
    char    Group_Super:1;
    char        Enctype:3;
    char         userId:4;
}UserInfo_type;


typedef struct AML_LoginAndLogout_T{
    FixedHeader_type  fixedHeader;
    unsigned short      messageId;
    Time_type           time;
    float       latitude;
    float      longitude;
    unsigned long       globalId;
    unsigned char         netNum;
    unsigned char           apid;
    UserInfo_type        userInfo;
    unsigned char          image;
}Aml_LoginAndLogout_type;

typedef struct AML_GetInfoRes_T{
    unsigned short      messageId;
    UserInfo_type            user;
    unsigned char             cmd;
}AML_GetInfoRes_type;

typedef struct AML_GetInformation_T{
    FixedHeader_type  fixedHeader;
    unsigned char       msgformat;
    unsigned char          netNum;
    Time_type                time;
    unsigned long     dstGlobalId;
    unsigned long     srcGlobalId;
    unsigned char         dstApid;
    unsigned char         srcApid;
}Aml_GetInformation_type;

typedef struct AML_GroupMsg_T{
    FixedHeader_type  fixedHeader;
    unsigned short      messageId;
    Time_type                time;
    unsigned long        GlobalId;
    unsigned char         srcApid;
    UserInfo_type            User;
    unsigned char          netNum;
    unsigned char         nameLen;
}Aml_GroupMsg_type;

typedef struct AML_SendTextMsg_T{
    FixedHeader_type  fixedHeader;
    unsigned short      messageId;
    Time_type                time;
    unsigned long     dstGlobalId;
    unsigned long     srcGlobalId;
    unsigned char         dstApid;
    unsigned char         srcApid;
    UserInfo_type            User;
    unsigned char          netNum;
}Aml_SendTextMsg_type;

typedef struct AML_SyncMsg_T{
    FixedHeader_type  fixedHeader;
    unsigned char       msgformat;
    unsigned char          netNum;
    unsigned long       globalId;
}Aml_SyncMsg_type;

typedef struct AML_SyncMsgId_T{
    unsigned short     msgIdStart;
    unsigned short       msgIdEnd;
}Aml_SyncMsgId_type;

typedef struct AML_SyncMsgTime_T{
        Time_type      timeStart;
        Time_type        timeEnd;
}Aml_SyncMsgTime_type;

typedef struct AML_Smartconfig_T{
    FixedHeader_type  fixedHeader;
    unsigned char       msgformat;
    unsigned char         reserve;
    unsigned long        globalId;
}AML_Smartconfig_type;

typedef struct AML_UserState_T{
    unsigned long        globalId;
    float                latitude;
    float               longitude;
    UserInfo_type        userInfo;
    unsigned char	      data[3];
}AML_UserState_type;

typedef struct AML_UserPerm_T{
    unsigned long        globalId;
    unsigned char             wan;
    unsigned char	      data[3];
}AML_UserPerm_type;



int AmlDecoder(const char *netBuf, const int len,char *retStrBuf);
int AmlLoginAndLogoutToStr(const char *netBuf, const int len,char *retStrBuf);
int AmlSmartconfigToStr(const char *netBuf, const int len,char *retStrBuf);
int AmlSmartconfigToStrResponse(const char *netBuf, const int len,char *retStrBuf);
int AmlSmartconfigToStrUserStateRes(const char *netBuf, const int len,char *retStrBuf);
int AmlSmartconfigToStrUserPermissionRes(const char *netBuf, const int len,char *retStrBuf);
int AmlGetInformationToStr(const char *netBuf, const int len,char *retStrBuf);
int AmlSendTextToStr(const char *netBuf, const int len,char *retStrBuf);
int AmlSendAcronymToStr(const char *netBuf, const int len,char *retStrBuf);
int AmlSendSyncToStr(const char *netBuf, const int len,char *retStrBuf);
int AmlSendGroupToStr(const char *netBuf, const int len,char *retStrBuf);


int AmlEncoder(const char *fixHeader, const char *ascii, const char *gb2312, char *netBuf);
int AmlLoginAndLogoutToByte(FixedHeader_type FixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int timeStrToStruct(const char *Strtime, Time_type *time);
int AmlSmartconfigToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlSmartconfigToByteRequest(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlSmartconfigToByteUserStateReq(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlSmartconfigToByteUserPermissionReq(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlGetInformationToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlGetInformationToByteRequest(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlGetInformationToByteResponse(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlSendTextToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlSendAcronymToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlSendVoiceToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlSendGroupMsgToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int AmlSyncMsgToByte(FixedHeader_type fixedHeader,const char *ascii, const char *gb2312, char *netBuf);
int GroupStrToStruct(const char *ascii,char *netBuf);
int UserPermToByte(const char *pbuf,char *rebuf);


#endif //AMLUDP_AMLUDP_H
