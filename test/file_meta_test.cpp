#include <iostream>

#include "file_meta.h"

int main()
{
    std::cout << "ok";
    
    std::cout << ecode2string(FileMeta::isValidPath("D:/files/imgs/QT安装.png"));

    FileMeta fm("D:/files/imgs/QT安装.png");

    std::string str1 = "123";
    std::string str2 = "213";


    fm.insertTag(str1);
    fm.insertTag(str2);

    std::set<std::string> tags = fm.getTags();

    for(auto i : tags)
    {
        std::cout << i << '\n';
    }

    return 0;
}