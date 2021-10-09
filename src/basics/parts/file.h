#ifndef _W_FILE_H_
#define _W_FILE_H_

#include <iostream>
#include <string>
#include <fstream>
#include <functional>
#include <cstdarg>
#include <stdio.h>
#include <memory>
#include <future>


namespace gdres {

class WFile {
public:
    // 判断文件是否存在
    static bool IsFileExist(const std::string& filename);
    // 创建文件夹
    static bool MkFile(const std::string& filename);
    // 打开以读取文件
    static bool OpenForRead(std::ifstream& ifs, const std::string& filename,
                            std::ios_base::openmode mode);
    // 打开以写入文件
    static bool OpenForWrite(std::ofstream& ofs, const std::string& filename,
                             std::ios_base::openmode mode);

};




}

#endif
