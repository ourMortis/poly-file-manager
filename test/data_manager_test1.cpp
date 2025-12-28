#include "data_manager.hpp"
#include <algorithm>
#include <cassert>
#include <iostream>


void test_tag_management()
{
    std::cout << "========== 测试标签管理 ==========\n";
    DataManager fm;

    // 测试创建标签
    fm.create_tag("工作");
    fm.create_tag("学习");
    fm.create_tag("娱乐");
    fm.create_tag("工作"); // 重复创建应被忽略

    auto all_tags = fm.get_all_tags();
    std::cout << "所有标签: ";
    for (const auto &tag : all_tags)
    {
        std::cout << tag << " ";
    }
    std::cout << std::endl;
    assert(all_tags.size() == 3);
    assert(std::find(all_tags.begin(), all_tags.end(), "工作") != all_tags.end());
    assert(std::find(all_tags.begin(), all_tags.end(), "学习") != all_tags.end());
    assert(std::find(all_tags.begin(), all_tags.end(), "娱乐") != all_tags.end());

    // 测试重命名标签
    fm.rename_tag("工作", "工作任务");
    all_tags = fm.get_all_tags();
    std::cout << "重命名后所有标签: ";
    for (const auto &tag : all_tags)
    {
        std::cout << tag << " ";
    }
    std::cout << std::endl;
    assert(std::find(all_tags.begin(), all_tags.end(), "工作任务") != all_tags.end());
    assert(std::find(all_tags.begin(), all_tags.end(), "工作") == all_tags.end());

    // 测试删除标签
    fm.remove_tag("学习");
    all_tags = fm.get_all_tags();
    std::cout << "删除后所有标签: ";
    for (const auto &tag : all_tags)
    {
        std::cout << tag << " ";
    }
    std::cout << std::endl;
    assert(all_tags.size() == 2);
    assert(std::find(all_tags.begin(), all_tags.end(), "学习") == all_tags.end());

    // 测试删除不存在的标签
    fm.remove_tag("不存在的标签"); // 不应崩溃
    std::cout << "✓ 标签管理测试通过\n";
}

void test_path_management()
{
    std::cout << "\n========== 测试路径管理 ==========\n";
    DataManager fm;

    // 测试创建路径
    fm.create_path("/home/user/docs/report.txt");
    fm.create_path("/home/user/docs/notes.md");
    fm.create_path("/home/user/pictures/photo.jpg");
    fm.create_path("/home/user/docs/report.txt"); // 重复创建应被忽略

    auto all_paths = fm.get_all_paths();
    std::cout << "所有路径:\n";
    for (const auto &path : all_paths)
    {
        std::cout << "  " << path.string() << std::endl;
    }
    assert(all_paths.size() == 3);

    // 测试重命名路径
    fm.rename_path("/home/user/docs/notes.md", "/home/user/docs/meeting_notes.md");
    all_paths = fm.get_all_paths();
    std::cout << "重命名后所有路径:\n";
    for (const auto &path : all_paths)
    {
        std::cout << "  " << path.string() << std::endl;
    }

    // 测试删除路径
    fm.remove_path("/home/user/pictures/photo.jpg");
    all_paths = fm.get_all_paths();
    std::cout << "删除后所有路径:\n";
    for (const auto &path : all_paths)
    {
        std::cout << "  " << path.string() << std::endl;
    }
    assert(all_paths.size() == 2);

    // 测试删除不存在的路径
    fm.remove_path("/nonexistent/path");
    std::cout << "✓ 路径管理测试通过\n";
}

void test_tag_path_associations()
{
    std::cout << "\n========== 测试标签-路径关联 ==========\n";
    DataManager fm;

    // 设置测试数据
    fm.create_path("/home/user/docs/report1.txt");
    fm.create_path("/home/user/docs/report2.txt");
    fm.create_path("/home/user/docs/notes.md");
    fm.create_path("/home/user/music/song.mp3");

    fm.create_tag("工作");
    fm.create_tag("重要");
    fm.create_tag("紧急");
    fm.create_tag("娱乐");

    // 测试为路径添加标签
    fm.assign_tag_to_path("/home/user/docs/report1.txt", "工作");
    fm.assign_tag_to_path("/home/user/docs/report1.txt", "重要");
    fm.assign_tag_to_path("/home/user/docs/report1.txt", "紧急");

    fm.assign_tag_to_path("/home/user/docs/report2.txt", "工作");
    fm.assign_tag_to_path("/home/user/docs/notes.md", "工作");

    fm.assign_tag_to_path("/home/user/music/song.mp3", "娱乐");

    // 测试查询路径的标签
    auto tags1 = fm.get_tags_for_path("/home/user/docs/report1.txt");
    std::cout << "report1.txt 的标签: ";
    for (const auto &tag : tags1)
    {
        std::cout << tag << " ";
    }
    std::cout << std::endl;
    assert(tags1.size() == 3);
    assert(tags1.find("工作") != tags1.end());
    assert(tags1.find("重要") != tags1.end());
    assert(tags1.find("紧急") != tags1.end());

    auto tags2 = fm.get_tags_for_path("/home/user/music/song.mp3");
    std::cout << "song.mp3 的标签: ";
    for (const auto &tag : tags2)
    {
        std::cout << tag << " ";
    }
    std::cout << std::endl;
    assert(tags2.size() == 1);
    assert(tags2.find("娱乐") != tags2.end());

    // 测试查询具有特定标签的路径
    auto work_paths = fm.get_paths_with_tag("工作");
    std::cout << "具有\"工作\"标签的路径:\n";
    for (const auto &path : work_paths)
    {
        std::cout << "  " << path.string() << std::endl;
    }
    assert(work_paths.size() == 3);

    auto important_paths = fm.get_paths_with_tag("重要");
    std::cout << "具有\"重要\"标签的路径:\n";
    for (const auto &path : important_paths)
    {
        std::cout << "  " << path.string() << std::endl;
    }
    assert(important_paths.size() == 1);

    // 测试移除标签关联
    fm.remove_tag_from_path("/home/user/docs/report1.txt", "紧急");
    tags1 = fm.get_tags_for_path("/home/user/docs/report1.txt");
    std::cout << "移除\"紧急\"标签后 report1.txt 的标签: ";
    for (const auto &tag : tags1)
    {
        std::cout << tag << " ";
    }
    std::cout << std::endl;
    assert(tags1.size() == 2);
    assert(tags1.find("紧急") == tags1.end());

    // 测试不存在的关联操作
    fm.remove_tag_from_path("/home/user/docs/report1.txt", "不存在的标签"); // 应安全处理
    fm.remove_tag_from_path("/nonexistent/path.txt", "工作");               // 应安全处理

    std::cout << "✓ 标签-路径关联测试通过\n";
}

void test_data_consistency()
{
    std::cout << "\n========== 测试数据一致性 ==========\n";
    DataManager fm;

    // 创建测试数据
    fm.create_path("/test/file1.txt");
    fm.create_path("/test/file2.txt");
    fm.create_tag("A");
    fm.create_tag("B");
    fm.create_tag("C");

    // 建立复杂关联
    fm.assign_tag_to_path("/test/file1.txt", "A");
    fm.assign_tag_to_path("/test/file1.txt", "B");
    fm.assign_tag_to_path("/test/file2.txt", "A");
    fm.assign_tag_to_path("/test/file2.txt", "C");

    std::cout << "初始状态:\n";

    // 测试删除标签时的关联清理
    std::cout << "\n删除标签\"A\"后:\n";
    fm.remove_tag("A");

    auto file1_tags = fm.get_tags_for_path("/test/file1.txt");
    auto file2_tags = fm.get_tags_for_path("/test/file2.txt");

    assert(file1_tags.size() == 1); // 只剩下 B
    assert(file1_tags.find("A") == file1_tags.end());
    assert(file1_tags.find("B") != file1_tags.end());

    assert(file2_tags.size() == 1); // 只剩下 C
    assert(file2_tags.find("A") == file2_tags.end());
    assert(file2_tags.find("C") != file2_tags.end());

    // 测试删除路径时的关联清理
    std::cout << "\n删除路径\"/test/file1.txt\"后:\n";
    fm.remove_path("/test/file1.txt");

    auto tag_b_paths = fm.get_paths_with_tag("B");
    assert(tag_b_paths.empty()); // 标签B应该没有关联的路径了

    std::cout << "✓ 数据一致性测试通过\n";
}

void test_edge_cases()
{
    std::cout << "\n========== 测试边界情况 ==========\n";
    DataManager fm;

    // 测试空管理器
    auto empty_tags = fm.get_all_tags();
    auto empty_paths = fm.get_all_paths();
    assert(empty_tags.empty());
    assert(empty_paths.empty());

    // 测试查询不存在的路径/标签
    auto non_exist_tags = fm.get_tags_for_path("/nonexistent/path");
    auto non_exist_paths = fm.get_paths_with_tag("nonexistent");
    assert(non_exist_tags.empty());
    assert(non_exist_paths.empty());

    // 测试自动创建（通过assign_tag_to_path）
    fm.assign_tag_to_path("/auto/created/path.txt", "AutoTag");

    auto tags = fm.get_tags_for_path("/auto/created/path.txt");
    auto paths = fm.get_paths_with_tag("AutoTag");

    assert(!tags.empty());
    assert(!paths.empty());
    assert(tags.find("AutoTag") != tags.end());
    assert(paths.find("/auto/created/path.txt") != paths.end());

    // 测试同名重命名
    fm.create_tag("TestTag");
    fm.rename_tag("TestTag", "TestTag"); // 重命名为相同名称
    auto all_tags = fm.get_all_tags();
    assert(std::find(all_tags.begin(), all_tags.end(), "TestTag") != all_tags.end());

    std::cout << "✓ 边界情况测试通过\n";
}

void test_dump_functions()
{
    std::cout << "\n========== 测试调试输出函数 ==========\n";
    DataManager fm;

    // 设置测试数据
    fm.create_path("/docs/a.txt");
    fm.create_path("/docs/b.txt");
    fm.create_path("/music/c.mp3");

    fm.create_tag("工作");
    fm.create_tag("娱乐");

    fm.assign_tag_to_path("/docs/a.txt", "工作");
    fm.assign_tag_to_path("/docs/b.txt", "工作");
    fm.assign_tag_to_path("/music/c.mp3", "娱乐");

}

int main()
{
    try
    {
        std::cout << "开始 DataManager 测试...\n\n";

        test_tag_management();
        test_path_management();
        test_tag_path_associations();
        test_data_consistency();
        test_edge_cases();
        test_dump_functions();

        std::cout << "\n=================================\n";
        std::cout << "所有测试通过！✓\n";
        std::cout << "=================================\n";

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
}