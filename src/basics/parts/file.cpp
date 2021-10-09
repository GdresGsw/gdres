/* =======================================================================
 * @brief  : 文件相关操作的封装
 * @author : guoshuaiwei
 * @email  : 1820807893@qq.com
 * @date   : 2021-09-10
 * ======================================================================= */

#include <file.h>
#include <string>
#include <iostream>
#include <sstream>
#include <thread>


#ifdef _WIN32
#include <direct.h>
#else
#include <unistd.h>
#endif

namespace gdres {

// 判断文件是否存在
bool WFile::IsFileExist(const std::string& filename)
{
    return (access(filename.c_str(), 0) == 0);
}

// 创建文件
bool WFile::MkFile(const std::string& filename)
{
    if (WFile::IsFileExist(filename)) {
        return true;  // 存在则不创建
    }
    std::string cmd{""};
    #ifdef _WIN32
    cmd = "type nul>" + filename;    
    #else
    cmd = "touch " + filename;
    #endif
    return (system(cmd.c_str()) == 0);

}


bool WFile::OpenForRead(std::ifstream& ifs, const std::string& filename,
                        std::ios_base::openmode mode)
{
    ifs.open(filename.c_str(), mode);
    return ifs.is_open();
}

bool WFile::OpenForWrite(std::ofstream& ofs, const std::string& filename,
                         std::ios_base::openmode mode) {
    ofs.open(filename.c_str(), mode);   
    if(!ofs.is_open()) {
        MkFile(filename);
        ofs.open(filename.c_str(), mode);
    }
    return ofs.is_open();
}




}

