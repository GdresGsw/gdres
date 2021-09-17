/* ===============================================================
 * description: socket一些公共函数
 *
 * DATA : 2021/8/17
 * =============================================================== */
#ifndef TCP_SOCKET_P_H
#define TCP_SOCKET_P_H


#ifdef _WIN32
    #define FD_SETSIZE 1000     // 定义该宏使windows下可以连接1000个socket 经测试本电脑默认最多链接socket数量16346
    #define WIN32_LEAN_AND_MEAN  // 该宏可避免库冲突
    #include <WinSock2.h>  // 需要在windows.h前调用
    #include <windows.h>
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <string.h>

    #define SOCKET int
    #define INVALID_SOCKET  (SOCKET)(~0)
    #define SOCKET_ERROR            (-1)
#endif

#define START_WIN_SOCKET 1
#define CLEAN_WIN_SOCKET 0
/*==============================================
 * description: 启动或者关闭windows的socket环境
 *==============================================*/
void WStartOrCLeanWinSocket(int state);









#endif


