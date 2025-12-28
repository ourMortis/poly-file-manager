#include "poly_file_manager.hpp"
#include "common_types.hpp"
#include <filesystem>
#include <cassert>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

// 辅助函数：创建测试文件
void create_test_file(const FilePath& path) {
    std::ofstream ofs(path);
    ofs << "test content";
}

// 辅助函数：清理测试目录
void cleanup_test_dir(const fs::path& repo_path) {
    if (fs::exists(repo_path)) {
        fs::remove_all(repo_path);
    }
}

int main() {
    // 初始化测试路径
    fs::path base_path = fs::current_path() / "poly_file_manager_test";
    fs::create_directory(base_path);
    std::cout << "base_path: " << base_path << std::endl;

    try {
        // 测试1：创建PolyFileManager实例（有效的repo_path）
        PolyFileManager manager(base_path);
        assert(manager.get_repo_path() == base_path && "测试1：仓库路径不正确");
        std::cout << "测试1通过\n";

        // 测试2：添加路径
        FilePath test_file1 = base_path / "file1.txt";
        create_test_file(test_file1);
        manager.add_path(test_file1);
        std::vector<FilePath> all_paths = manager.get_all_paths();
        assert(all_paths.size() == 1 && all_paths[0] == test_file1 && "测试2：添加路径失败");
        std::cout << "测试2通过\n";

        // 测试3：添加标签
        FileTag tag1 = "tag1";
        manager.add_tag(tag1);
        std::vector<FileTag> all_tags = manager.get_all_tags();
        assert(all_tags.size() == 1 && all_tags[0] == tag1 && "测试3：添加标签失败");
        std::cout << "测试3通过\n";

        // 测试4：给文件分配标签
        bool assign_success = manager.assign_tag_to_file(test_file1, tag1);
        assert(assign_success && "测试4：分配标签失败");
        std::vector<FileTag> file_tags = manager.get_tags_for_file(test_file1);
        assert(file_tags.size() == 1 && file_tags[0] == tag1 && "测试4：获取文件标签失败");
        std::cout << "测试4通过\n";

        // 测试5：获取带标签的文件
        std::vector<FilePath> tagged_files = manager.get_paths_with_tag(tag1);
        assert(tagged_files.size() == 1 && tagged_files[0] == test_file1 && "测试5：获取带标签文件失败");
        std::cout << "测试5通过\n";

        // 测试6：重命名文件
        FilePath test_file1_new = base_path / "file1_renamed.txt";
        int rename_cnt = manager.rename_path(test_file1, test_file1_new);
        assert(rename_cnt == 1 && "测试6：重命名符号链接失败");
        all_paths = manager.get_all_paths();
        assert(all_paths.size() == 1 && all_paths[0] == test_file1_new && "测试6：重命名路径记录失败");
        file_tags = manager.get_tags_for_file(test_file1_new);
        assert(file_tags.size() == 1 && file_tags[0] == tag1 && "测试6：重命名后标签关联失败");
        std::cout << "测试6通过\n";

        // 测试7：重命名标签
        FileTag tag1_new = "tag1_renamed";
        bool rename_tag_success = manager.rename_tag(tag1, tag1_new);
        assert(rename_tag_success && "测试7：重命名标签目录失败");
        all_tags = manager.get_all_tags();
        assert(all_tags.size() == 1 && all_tags[0] == tag1_new && "测试7：重命名标签记录失败");
        tagged_files = manager.get_paths_with_tag(tag1_new);
        assert(tagged_files.size() == 1 && tagged_files[0] == test_file1_new && "测试7：重命名后标签关联文件失败");
        std::cout << "测试7通过\n";

        // 测试8：从文件移除标签
        bool remove_tag_success = manager.remove_tag_from_file(test_file1_new, tag1_new);
        assert(remove_tag_success && "测试8：移除符号链接失败");
        file_tags = manager.get_tags_for_file(test_file1_new);
        assert(file_tags.empty() && "测试8：移除文件标签记录失败");
        std::cout << "测试8通过\n";

        // 测试9：添加第二个文件和标签并分配
        FilePath test_file2 = base_path / "file2.txt";
        create_test_file(test_file2);
        manager.add_path(test_file2);
        FileTag tag2 = "tag2";
        manager.add_tag(tag2);
        manager.assign_tag_to_file(test_file1_new, tag2);
        manager.assign_tag_to_file(test_file2, tag2);
        tagged_files = manager.get_paths_with_tag(tag2);
        assert(tagged_files.size() == 2 && "测试9：标签关联多个文件失败");
        std::cout << "测试9通过\n";

        // 测试10：删除标签
        int remove_tag_cnt = manager.remove_tag(tag2);
        assert(remove_tag_cnt == 3 && "测试10：删除标签目录内容失败");
        all_tags = manager.get_all_tags();
        assert(all_tags.size() == 1 && "测试10：删除标签记录失败");
        std::cout << "测试10通过\n";

        // 测试11：删除文件
        int remove_cnt = manager.remove_path(test_file1_new);
        assert(remove_cnt == 0 && "测试11：删除文件符号链接失败"); // 此时文件已无标签关联
        all_paths = manager.get_all_paths();
        assert(all_paths.size() == 1 && all_paths[0] == test_file2 && "测试11：删除文件记录失败");
        std::cout << "测试11通过\n";

        // 测试12：保存和加载数据
        manager.add_tag(tag1);
        manager.assign_tag_to_file(test_file2, tag1);
        bool save_success = manager.save();
        assert(save_success && "测试12：保存数据失败");
        
        PolyFileManager manager2(base_path);
        manager2.load(base_path);
        all_tags = manager2.get_all_tags();
        assert(all_tags.size() == 2 && all_tags[0] == tag1 && "测试12：加载标签失败");
        all_paths = manager2.get_all_paths();
        assert(all_paths.size() == 1 && all_paths[0] == test_file2 && "测试12：加载路径失败");
        std::cout << "测试12通过\n";

        std::cout << "所有测试通过！\n";
    } catch (const std::exception& e) {
        std::cerr << "测试失败：" << e.what() << std::endl;
        cleanup_test_dir(base_path);
        return 1;
    }

    cleanup_test_dir(base_path);
    return 0;
}