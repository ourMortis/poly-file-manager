#include "data_manager_new.hpp"

DataManagerNew::DataManagerNew(const FileTagData &data)
{
    for (const auto &tag : data.index_to_tag)
    {
        create_tag(tag);
    }
    for (const auto &[path, v] : data.path_to_index_map)
    {
        create_path(path);
        for (const auto &index : v)
        {
            if (index < data.index_to_tag.size())
            {
                assign_tag_to_path(path, data.index_to_tag[index]);
            }
        }
    }
}



FileTagData DataManagerNew::get_file_tag_data() const
{
    FileTagData data;
    std::map<FileTag, int> tag_to_index;
    int index = 0;

    for (const auto &tag : tag_path_map_.get_all_T1())
    {
        data.index_to_tag.push_back(tag);
        tag_to_index[tag] = index++;
    }

    for (const auto &path : tag_path_map_.get_all_T2())
    {
        auto tags = tag_path_map_.get_associated_T2(path);
        for (const auto &tag : tags)
        {
            data.path_to_index_map[path].push_back(tag_to_index[tag]);
        }
    }

    return data;
}
