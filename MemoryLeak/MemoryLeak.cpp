#include "MemoryLeak.h"
#include < strsafe.h>

CMemoryLeak::CMemoryLeak(void)
{
	if (m_heap == NULL)
	{
		//打开异常检测
		m_heap = HeapCreate(HEAP_GENERATE_EXCEPTIONS,0,0);
		ULONG  HeapFragValue = 2;
		//允许系统记录堆内存的使用
		HeapSetInformation( m_heap,HeapCompatibilityInformation,&HeapFragValue ,sizeof(HeapFragValue)) ;
	}

	if (NULL == m_pBlockInfo)
	{
		m_pBlockInfo = (LP_ST_BLOCK_INFO)HeapAlloc(m_heap, HEAP_ZERO_MEMORY, MAX_BUFFER_SIZE * sizeof(ST_BLOCK_INFO));
		m_BlockSize = MAX_BUFFER_SIZE;
		m_hasInfo = 0;
	}
}

void CMemoryLeak::add(LPCTSTR m_szSourcePath, INT m_iLine, void *pBlock)
{
	//当前缓冲区已满
	if (m_hasInfo >= m_BlockSize)
	{
		//扩大缓冲区容量
		HeapReAlloc(m_heap, HEAP_ZERO_MEMORY, m_pBlockInfo, m_BlockSize * 2 * sizeof(ST_BLOCK_INFO));
		m_BlockSize *= 2;
	}

	m_pBlockInfo[m_hasInfo].m_bDelete = FALSE;
	m_pBlockInfo[m_hasInfo].m_iLine = m_iLine;
	_tcscpy(m_pBlockInfo[m_hasInfo].m_szSourcePath, m_szSourcePath);
	m_pBlockInfo[m_hasInfo].pBlock = pBlock;
	m_hasInfo++;
}


CMemoryLeak::~CMemoryLeak(void)
{
	HeapFree(m_heap, 0, m_pBlockInfo);
	HeapDestroy(m_heap);
}

void CMemoryLeak::MemoryLeak()
{
	TCHAR pszOutPutInfo[2*MAX_PATH]; //调试字符串
	BOOL  bRecord = FALSE; //当前内存是否被记录
	PROCESS_HEAP_ENTRY phe = {};
	HeapLock(GetProcessHeap()); //检测时锁定堆防止对堆内存进行写入
	OutputDebugString(_T("开始检查内存泄露情况.........\n"));

	while (HeapWalk(GetProcessHeap(), &phe))
	{
		if( PROCESS_HEAP_ENTRY_BUSY & phe.wFlags )
		{
			bRecord = FALSE;
			for(UINT i = 0; i < m_hasInfo; i ++ )
			{
				if( phe.lpData == m_pBlockInfo[i].pBlock)
				{
					if(!m_pBlockInfo[i].m_bDelete)
					{
						StringCchPrintf(pszOutPutInfo,2*MAX_PATH,_T("%s(%d):内存块(Point=0x%08X,Size=%u)\n")
							,m_pBlockInfo[i].m_szSourcePath,m_pBlockInfo[i].m_iLine,phe.lpData,phe.cbData);
						OutputDebugString(pszOutPutInfo);
					}
					bRecord = TRUE;
					break;
				}
			}
			if( !bRecord )
			{
				StringCchPrintf(pszOutPutInfo,2*MAX_PATH,_T("未记录的内存块(Point=0x%08X,Size=%u)\n")
					,phe.lpData,phe.cbData);
				OutputDebugString(pszOutPutInfo);
			}
		}

	}

	HeapUnlock(GetProcessHeap());
	OutputDebugString(_T("内存泄露检查完毕.\n"));

}

int CMemoryLeak::GetLength()
{

	return m_hasInfo;
}

ST_BLOCK_INFO& CMemoryLeak::operator [](int nSite)
{
	return m_pBlockInfo[nSite];
}


CMemoryLeak g_MemoryLeak;

void* __cdecl operator new(size_t nSize,LPCTSTR pszCppFile,int iLine)
{
	void *p = HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nSize);
	g_MemoryLeak.add(pszCppFile, iLine, p);
	return p;
}

void __cdecl operator delete(void *p, TCHAR *pstrPath, int nLine)
{
	::operator delete(p);
	HeapFree(GetProcessHeap(), 0, p);
}

void __cdecl operator delete(void* p)
{
	for (int i = 0; i < g_MemoryLeak.GetLength(); i++)
	{
		if (p == g_MemoryLeak[i].pBlock)
		{
			g_MemoryLeak[i].m_bDelete = TRUE;
		}
	}

	HeapFree(GetProcessHeap(), 0, p);
}
