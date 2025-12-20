#include "file_system_organizer.h"
#include <cassert>
#include <filesystem>
#include <fstream>
#include <iostream>


namespace fs = std::filesystem;

// 辅助函数：检查路径是否存在
bool path_exists(const FilePath &path) { return fs::exists(path); }

// 辅助函数：创建临时测试文件
FilePath create_temp_file(const FilePath &parent, const std::string &name)
{
    FilePath file_path = parent / name;
    std::ofstream ofs(file_path);
    ofs.close();
    assert(path_exists(file_path) && "临时文件创建失败");
    return file_path;
}

// 清理测试生成的文件和目录
void cleanup_test_resources(FileSystemOrganizer &organizer, const std::vector<std::string> &categories,
                            const std::vector<FilePath> &temp_files)
{
    // 移除测试文件
    for (const auto &file : temp_files)
    {
        if (path_exists(file))
        {
            fs::remove(file);
        }
    }

    // 移除所有可能的分类目录（包括重命名后的）
    for (const auto &cat : categories)
    {
        FilePath cat_path = organizer.get_symlink_path(cat, FilePath()).parent_path();
        if (!cat_path.empty() && path_exists(cat_path))
        {
            fs::remove_all(cat_path);
        }
    }
    // 处理测试中重命名的目录
    FilePath renamed_cat = organizer.get_symlink_path("影视", FilePath()).parent_path();
    if (!renamed_cat.empty() && path_exists(renamed_cat))
    {
        fs::remove_all(renamed_cat);
    }
}

int main()
{
    std::cout << "====== 开始文件系统组织器测试 ======\n";
    FilePath repo_path = fs::current_path();
    std::cout << "测试仓库路径: " << repo_path << "\n\n";

    FileSystemOrganizer organizer(repo_path);
    std::vector<std::string> test_categories = {"文档", "图片", "视频"};
    std::vector<FilePath> temp_files;

    try
    {
        // 测试1: 创建分类目录
        std::cout << "测试1: 创建分类目录...\n";
        organizer.create_category_dirs(test_categories);
        for (const auto &cat : test_categories)
        {
            // 使用get_symlink_path获取分类目录路径（通过空文件路径）
            FilePath cat_path = organizer.get_symlink_path(cat, FilePath()).parent_path();
            assert(path_exists(cat_path) && "分类目录未创建成功");
            std::cout << "  验证成功: " << cat << " 目录创建\n";
        }
        std::cout << "测试1: 成功\n\n";

        // 测试2: 创建单个符号链接
        std::cout << "测试2: 创建单个符号链接...\n";
        FilePath doc_file = create_temp_file(repo_path, "report.docx");
        temp_files.push_back(doc_file);

        organizer.create_symlink_in_category("文档", doc_file);
        FilePath doc_symlink = organizer.get_symlink_path("文档", doc_file);
        assert(path_exists(doc_symlink) && "单个符号链接创建失败");
        std::cout << "  验证成功: 单个符号链接创建\n";
        std::cout << "测试2: 成功\n\n";

        // 测试3: 创建批量符号链接
        std::cout << "测试3: 创建批量符号链接...\n";
        FilePath img1 = create_temp_file(repo_path, "photo1.jpg");
        FilePath img2 = create_temp_file(repo_path, "photo2.png");
        temp_files.push_back(img1);
        temp_files.push_back(img2);

        std::vector<FilePath> img_files = {img1, img2};
        organizer.create_symlink_in_category("图片", img_files);

        for (const auto &file : img_files)
        {
            FilePath symlink = organizer.get_symlink_path("图片", file);
            assert(path_exists(symlink) && "批量符号链接创建失败");
        }
        std::cout << "  验证成功: 批量符号链接创建\n";
        std::cout << "测试3: 成功\n\n";

        // 测试4: 获取分类中的符号链接
        std::cout << "测试4: 获取分类中的符号链接...\n";
        std::vector<FilePath> doc_links = organizer.get_symlinks_in_category("文档");
        assert(doc_links.size() == 1 && "文档分类链接数量错误");

        std::vector<FilePath> img_links = organizer.get_symlinks_in_category("图片");
        assert(img_links.size() == 2 && "图片分类链接数量错误");
        std::cout << "  验证成功: 符号链接获取正确\n";
        std::cout << "测试4: 成功\n\n";

        // 测试5: 重命名分类目录
        std::cout << "测试5: 重命名分类目录...\n";
        organizer.rename_category_dir("视频", "影视");

        FilePath old_cat = organizer.get_symlink_path("视频", FilePath()).parent_path();
        FilePath new_cat = organizer.get_symlink_path("影视", FilePath()).parent_path();

        assert(!path_exists(old_cat) && "原分类目录未删除");
        assert(path_exists(new_cat) && "新分类目录未创建");
        std::cout << "  验证成功: 分类目录重命名\n";
        std::cout << "测试5: 成功\n\n";

        // 测试6: 删除单个符号链接
        std::cout << "测试6: 删除单个符号链接...\n";
        organizer.remove_symlink_in_category("文档", doc_file);
        FilePath removed_symlink = organizer.get_symlink_path("文档", doc_file);
        assert(!path_exists(removed_symlink) && "单个符号链接未删除");
        std::cout << "  验证成功: 单个符号链接删除\n";
        std::cout << "测试6: 成功\n\n";

        // 测试7: 删除批量符号链接
        std::cout << "测试7: 删除批量符号链接...\n";
        organizer.remove_symlink_in_category("图片", img_files);
        for (const auto &file : img_files)
        {
            FilePath symlink = organizer.get_symlink_path("图片", file);
            assert(!path_exists(symlink) && "批量符号链接未删除");
        }
        std::cout << "  验证成功: 批量符号链接删除\n";
        std::cout << "测试7: 成功\n\n";

        // 测试8: 删除分类目录
        std::cout << "测试8: 删除分类目录...\n";
        organizer.remove_category_dir("图片");
        FilePath deleted_cat = organizer.get_symlink_path("图片", FilePath()).parent_path();
        assert(!path_exists(deleted_cat) && "分类目录未删除");
        std::cout << "  验证成功: 分类目录删除\n";
        std::cout << "测试8: 成功\n\n";

        std::cout << "====== 所有测试通过 ======\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }

    // 清理测试资源
    cleanup_test_resources(organizer, test_categories, temp_files);
    return 0;
}