#include "common_types.hpp"
#include "data_manager_new.hpp"
#include <cassert>
#include <iostream>


void test_create_and_remove_tag()
{
    DataManager manager;
    FileTag tag = "example_tag";

    // Test creating a tag
    assert(manager.create_tag(tag));
    assert(manager.contains_tag(tag));

    // Test removing a tag
    assert(manager.remove_tag(tag));
    assert(!manager.contains_tag(tag));
}

void test_create_and_remove_path()
{
    DataManager manager;
    FilePath path = "example_path";

    // Test creating a path
    assert(manager.create_path(path));
    assert(manager.contains_path(path));

    // Test removing a path
    assert(manager.remove_path(path));
    assert(!manager.contains_path(path));
}

void test_assign_and_remove_tag_from_path()
{
    DataManager manager;
    FilePath path = "example_path";
    FileTag tag = "example_tag";

    manager.create_path(path);
    manager.create_tag(tag);

    // Test assigning a tag to a path
    assert(manager.assign_tag_to_path(path, tag));
    assert(manager.contains_association(path, tag));

    // Test removing a tag from a path
    assert(manager.remove_tag_from_path(path, tag));
    assert(!manager.contains_association(path, tag));
}

void test_get_paths_with_tag()
{
    DataManager manager;
    FilePath path1 = "path1";
    FilePath path2 = "path2";
    FileTag tag = "example_tag";

    manager.create_path(path1);
    manager.create_path(path2);
    manager.create_tag(tag);

    manager.assign_tag_to_path(path1, tag);
    manager.assign_tag_to_path(path2, tag);

    auto paths = manager.get_paths_with_tag(tag);
    assert(paths.size() == 2);
    assert(paths.find(path1) != paths.end());
    assert(paths.find(path2) != paths.end());
}

void test_get_tags_for_path()
{
    DataManager manager;
    FilePath path = "example_path";
    FileTag tag1 = "tag1";
    FileTag tag2 = "tag2";

    manager.create_path(path);
    manager.create_tag(tag1);
    manager.create_tag(tag2);

    manager.assign_tag_to_path(path, tag1);
    manager.assign_tag_to_path(path, tag2);

    auto tags = manager.get_tags_for_path(path);
    assert(tags.size() == 2);
    assert(tags.find(tag1) != tags.end());
    assert(tags.find(tag2) != tags.end());
}

int main()
{
    test_create_and_remove_tag();
    test_create_and_remove_path();
    test_assign_and_remove_tag_from_path();
    test_get_paths_with_tag();
    test_get_tags_for_path();

    std::cout << "All tests passed!" << std::endl;

    FileTagData data{{"a", "b", "c", "d"}, {{"1", {0, 1, 2}}, {"2", {0, 2, 3}}, {"3", {1, 2}}}};
    DataManager manager(data);

    auto st = manager.get_paths_with_tag("a");
    for (const auto &i : st)
    {
        std::cout << i << '\n';    
    }
    return 0;
}