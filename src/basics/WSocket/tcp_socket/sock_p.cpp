#include <sock_p.h>
#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN  // 该宏可避免库冲突
    #include <WinSock2.h>  // 需要在windows.h前调用
    #include <windows.h>
#endif


/*==============================================
 * description: 启动或者关闭windows的socket环境
 *==============================================*/
void WStartOrCLeanWinSocket(int state)
{
#ifdef _WIN32
    // 启动windows中的socket环境
    if (state == START_WIN_SOCKET) {
        WORD ver = MAKEWORD(2, 2);
        WSADATA dat;
        WSAStartup(ver, &dat);
    }

    // 关闭windows的socket环境
    if (state == CLEAN_WIN_SOCKET) {
        WSACleanup();
    }
#endif

    return;
}





