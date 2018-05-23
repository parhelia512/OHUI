
#pragma once

#ifndef OUT
#define OUT
#endif

#ifndef IN
#define IN
#endif

namespace nsYLX
{ 
    class CUtil
    {
	public:
        CUtil(void);
		~CUtil(void);
        
		//nLevel stand for parent path's level，default nLevel=1，means father path，nLevel=2，means grand father path，and so on
		//retval is pOutPath actual's bytes(no include '\0'),if retval==0 ,means it's faild.
		//caller need malloc and free memory for pOutPath
		//ptchsPath is full path file name,or directory,when it's directory, it must be end with '/'or'\'
		static int GetParentPath(IN const char* pchsPath, OUT char* pOutPath, IN int nOutPathLen, IN int nLevel=1);    
    };
}
