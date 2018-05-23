
#include "Util.h"
#include <utility>
#include <string.h>
#include <algorithm>

namespace nsYLX
{ 
	CUtil::CUtil(void)
	{
	}

	CUtil::~CUtil(void)
	{
	}
    
    int CUtil::GetParentPath(IN const char* ptchsPath, OUT char* pOutPath, IN int nOutPathLen, IN int nLevel)
    {
		int nRtn =0;
		if (!ptchsPath)
		{
			return nRtn;
		}
		int nSrcLen = strlen(ptchsPath);
		if (0==nSrcLen || !pOutPath || nOutPathLen<=0 || nLevel<0)
		{
			return 0;
		}
		pOutPath[0]=0; 
		
		int nIndex  = -1;
		for (int i= nSrcLen-1;i>=0;i--)
		{
			if (ptchsPath[i] == '/' 
			 || ptchsPath[i] == '\\')
			{
				if (nLevel-- == 0)
				{
					nIndex  = i;
					break;
				}
			}
		} 
		if (nIndex==-1)
		{ 
			pOutPath[0]=0;
			return 0;
		}
		nIndex = std::min<int>(nIndex+1+1, nOutPathLen);//+1 means copy src's ends.i.e'/'or'\',again +1，stand for copy '\0'(if ptchsPath have)
		strncpy(pOutPath,ptchsPath, nIndex);
		pOutPath[nIndex-1]=0;

		nRtn = nIndex; 

		return nRtn;        
        return 0;
    }
}
