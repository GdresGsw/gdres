/* =======================================================================
 * @brief  : 基于协程调度器和epoll封装io管理
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 
 * ======================================================================= */

#include "iomanage.h"
#include <sys/epoll.h>
#include <unistd.h>

namespace gdres {

/*==================================================================
 * FuncName : IOManager::IOManager
 * description : 构造
 * author : guoshuaiwei
 *==================================================================*/
IOManager::IOManager(size_t threads, const std::string& name, bool useCaller) : Scheduler(threads, name, useCaller)
{
    m_epfd = epoll_create(1);
    int rt = pipe(m_tickleFds);

    epoll_event event;
    memset(&event, 0, sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;   // 边缘触发 读事件
    event.data.fd = m_tickleFds[0];


}










}
