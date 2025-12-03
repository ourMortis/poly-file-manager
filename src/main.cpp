#include<file_manager.h>

int main()
{
    FileManager fm;
    fm.create_tag("tag111");

    auto tags = fm.get_tags();
    for(auto i : tags)
    {
        std::cout << i << ' ';
    }
    std::cout << std::endl;

    fm.change_tag("tag111", "tag222");

    tags = fm.get_tags();
    for(auto i : tags)
    {
        std::cout << i << ' ';
    }
    std::cout << std::endl;

    return 0;
}