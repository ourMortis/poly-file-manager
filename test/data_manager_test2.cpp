#include "data_manager.hpp"

int main()
{
    FileTagData data;
    data.index_to_tag = {"0", "1", "2", "3", "4"};
    data.path_to_index_map = {
        {"path1", {1, 2}}, {"path2", {0, 1, 2, 3, 4}}, {"path3", {}}, {"path4", {2}}, {"path5", {5}}};

    DataManager fm(data);

    FileTagData fm_data = fm.get_file_tag_data();

    for (const auto &i : fm_data.path_to_index_map)
    {
        std::cout << i.first << ':';
        for (const auto &j : i.second)
        {
            std::cout << ' ' << fm_data.index_to_tag[j];
        }
        std::cout << '\n';
    }
}