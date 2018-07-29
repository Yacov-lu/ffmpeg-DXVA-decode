
#ifndef __CONSOLE_ADAPTER_H__
#define __CONSOLE_ADAPTER_H__

#include <iostream>
#include <io.h>
#include <fcntl.h>

using namespace std;

typedef enum
{
    INPUT_CONS,
    OUTPUT_CONS,
    BOTH
}CONSOLETYPE_e;

class CConsoleAdapter
{
public:
    CConsoleAdapter()
    {
        m_bHasCreate = FALSE;
    }
    ~CConsoleAdapter()
    {
        if (TRUE == m_bHasCreate)
        {
            DestroyConsole();
        }
    }

public:
    DWORD CreateConsole()
    {
        DWORD dwErrorCode = 0;

        do 
        {
            if (m_bHasCreate == TRUE)
            {
                break;
            }

            m_bHasCreate = AllocConsole();
            if (FALSE == m_bHasCreate)
            {
                dwErrorCode = GetLastError();
                break;
            }
            else
            {
                m_eConsoleType = OUTPUT_CONS;
                ReplaceHandles();
            }
        } while (FALSE);

        return dwErrorCode;
    }

    void DestroyConsole()
    {
        try
        {
			if (m_bHasCreate != TRUE)
			{
				return;
			}
			m_bHasCreate = FALSE;

            FreeConsole(); 
            if( ( INPUT_CONS == m_eConsoleType ) || ( BOTH == m_eConsoleType ) )
            {
                if( 0 != m_fpCRTIn )
                {
                    fclose( m_fpCRTIn );
                }
// 				if (-1 != m_nCRTIn)
// 				{
// 					_close( m_nCRTIn );
// 					m_nCRTIn = -1;
// 				}
                
                *stdin = m_fOldStdIn;
            }

            if( ( OUTPUT_CONS == m_eConsoleType ) || ( BOTH == m_eConsoleType ) )
            {
                if( 0 != m_fpCRTOut )
                {
                    fclose( m_fpCRTOut );
                }
// 				if (-1 != m_nCRTOut)
// 				{
// 					_close( m_nCRTOut );
// 					m_nCRTOut = -1;
// 				}
                
                *stdout = m_fOldStdOut;
            }
            
        }
        catch ( ... )
        {

        }

    }

private:
    BOOL ReplaceHandles()
    {
        try
        {
            if( ( INPUT_CONS == m_eConsoleType ) || ( BOTH == m_eConsoleType ) )
            {
                m_nCRTIn= _open_osfhandle(
                    (long) GetStdHandle(STD_INPUT_HANDLE),
                    _O_TEXT );
                if( -1 == m_nCRTIn )
                {
                    return FALSE;
                }
                m_fpCRTIn = _fdopen( m_nCRTIn, "r" );
                if( !m_fpCRTIn )
                {
                    return FALSE;
                }
                m_fOldStdIn = *stdin;
                *stdin = *m_fpCRTIn;
                // if clear is not done, any cout statement before AllocConsole
                // will cause, the cin after AllocConsole to fail, so very important
                std::cin.clear();
            }
            if( ( OUTPUT_CONS == m_eConsoleType ) || ( BOTH == m_eConsoleType ) )
            {
                m_nCRTOut= _open_osfhandle(
                    (long) GetStdHandle(STD_OUTPUT_HANDLE),
                    _O_TEXT );
                if( -1 == m_nCRTOut )
                {
                    return FALSE;
                }

                m_fpCRTOut = _fdopen( m_nCRTOut, "w" );
                if( !m_fpCRTOut )
                {
                    return FALSE;
                }
                m_fOldStdOut = *stdout;
                *stdout = *m_fpCRTOut;
                // if clear is not done, any cout statement before AllocConsole
                // will cause, the cout after AllocConsole to fail, so very important
                std::cout.clear();
            }
        }
        catch ( ... )
        {
            return FALSE;
        }        
        return TRUE;
    }

private:
    BOOL m_bHasCreate;
    CONSOLETYPE_e m_eConsoleType;

    FILE m_fOldStdIn;
    FILE* m_fpCRTIn;
    int m_nCRTIn;

    FILE m_fOldStdOut;
    FILE* m_fpCRTOut;
    int m_nCRTOut;
};

#endif



