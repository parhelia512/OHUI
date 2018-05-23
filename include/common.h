

#pragma once

  
typedef void (*pfn_dataCallback)(const void*, int n);

#ifndef WM_USER
#define WM_USER                         0x0400
#endif

#define GECKO_1_8_VER    18/10
#define GECKO_1_9_2_VER  192/100
#define GECKO_22_0_VER   22

#define GECKO_VERSION  GECKO_22_0_VER


enum 
{
	WM_USER_0=WM_USER+200,
	WM_BROWSERBUILD_COMPLETE,
	WM_SENDTO_JS,
	WM_SHOWDEMO,
	WM_PRINTPREVIEW,

};
