#include <windows.h>
#include <tchar.h>
#include "MemoryLeak.h"
#include < strsafe.h>

#ifdef _UNICODE
#define GRS_WIDEN2(x) L ## x
#define GRS_WIDEN(x) GRS_WIDEN2(x)
#define __WFILE__ GRS_WIDEN(__FILE__)
//��δ��벻�������ص�������ͬһ��ͷ�ļ��£������ڱ���ʱ�Ὣ�����new���������滻
#define new new(__WFILE__,__LINE__)
#define delete(p) ::operator delete(p,__WFILE__,__LINE__)
#else
#define new new(__FILE__,__LINE__)
#define delete(p) ::operator delete(p,__FILE__,__LINE__)
#endif

int _tmain()
{
	int* pInt1 = new int;
	int* pInt2 = new int;
	float* pFloat1 = new float;

	BYTE* pBt = new BYTE[100];


	delete[] pBt;

	//��DEBUG���������ü��
#ifdef _DEBUG
	g_MemoryLeak.MemoryLeak();
#endif
	return 0;
}