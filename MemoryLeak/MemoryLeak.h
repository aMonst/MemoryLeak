#pragma once
#include <windows.h>
#include <tchar.h>

#define MAX_BUFFER_SIZE	1000
typedef struct tag_ST_BLOCK_INFO
{
	TCHAR m_szSourcePath[MAX_PATH];
	INT m_iLine;
	BOOL m_bDelete;
	void *pBlock;
}ST_BLOCK_INFO, *LP_ST_BLOCK_INFO;

class CMemoryLeak
{
public:
	CMemoryLeak(void);
	~CMemoryLeak(void);
	void MemoryLeak();
	void add(LPCTSTR m_szSourcePath, INT m_iLine, void *pBlock);
	int GetLength();
	ST_BLOCK_INFO& operator [](int nSite);
protected:
	HANDLE m_heap;//自定义堆
	LP_ST_BLOCK_INFO m_pBlockInfo;
	int m_BlockSize; //当前缓冲区大小
	int m_hasInfo;//当前记录了多少值 
};

//采用C++默认的调用方式
void* __cdecl operator new(size_t nSize,LPCTSTR pszCppFile,int iLine);
void __cdecl operator delete(void *p, TCHAR *pstrPath, int nLine);
void __cdecl operator delete(void* p);
extern CMemoryLeak g_MemoryLeak;