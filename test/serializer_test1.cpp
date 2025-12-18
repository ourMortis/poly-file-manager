#include "serializer.h"
#include <iostream>
#include <cassert>
#include <filesystem>
#include <vector>
#include <map>
#include <stdexcept>

// 辅助函数：清理测试生成的配置文件
void clean_test_file(const std::filesystem::path& repo_path, const std::string& config_file) {
    std::filesystem::path file_path = repo_path / config_file;
    if (std::filesystem::exists(file_path)) {
        std::filesystem::remove(file_path);
    }
}

// 辅助函数：确保指定目录存在（用于测试环境准备）
bool ensure_directory_exists(const std::filesystem::path& dir_path) {
    if (!std::filesystem::exists(dir_path)) {
        return std::filesystem::create_directories(dir_path);
    }
    return std::filesystem::is_directory(dir_path);
}

int main() {
    std::cout << "开始测试修改后的 Serializer 类..." << std::endl;

    // ========== 测试配置 ==========
    const std::filesystem::path test_repo_path = "D:/project1"; // 指定的仓库路径
    const std::string config_file = ".poly_file_manager";      // 配置文件名

    // 提前确保测试目录存在（如果不存在则创建）
    assert(ensure_directory_exists(test_repo_path) && "测试目录D:/project创建/存在失败，请检查权限");

    // ========== 1. 测试构造函数的合法性校验 ==========
    std::cout << "测试：构造函数-非绝对路径校验..." << std::endl;
    bool throw_non_absolute = false;
    try {
        // 传入相对路径，预期抛出异常
        Serializer bad_serializer1("./relative_path");
    } catch (const std::invalid_argument& e) {
        throw_non_absolute = true;
        std::cout << "捕获到预期异常：" << e.what() << std::endl;
    }
    assert(throw_non_absolute && "构造函数未捕获非绝对路径");
    std::cout << "✓ 非绝对路径校验测试通过" << std::endl;

    std::cout << "测试：构造函数-非目录路径校验..." << std::endl;
    bool throw_non_directory = false;
    try {
        // 传入一个不存在的文件路径（非目录），预期抛出异常
        std::filesystem::path non_dir_path = "D:/non_exist_file.txt";
        Serializer bad_serializer2(non_dir_path);
    } catch (const std::invalid_argument& e) {
        throw_non_directory = true;
        std::cout << "捕获到预期异常：" << e.what() << std::endl;
    }
    assert(throw_non_directory && "构造函数未捕获非目录路径");
    std::cout << "✓ 非目录路径校验测试通过" << std::endl;

    // ========== 2. 测试使用指定路径D:/project创建Serializer对象 ==========
    std::cout << "测试：使用D:/project创建Serializer对象..." << std::endl;
    Serializer serializer(test_repo_path);
    assert(serializer.get_repo_path() == test_repo_path && "D:/project路径初始化失败");
    std::cout << "✓ 指定路径初始化测试通过" << std::endl;

    // ========== 3. 准备测试数据 ==========
    FileTagData test_data;
    // 填充index_to_tag
    test_data.index_to_tag = {"tag1", "tag2", "tag3"};
    // 填充path_to_index_map（键是文件路径，值是int向量）
    test_data.path_to_index_map["./file1.txt"] = {0, 1};
    test_data.path_to_index_map["D:/project/docs/file2.md"] = {2, 0, 1};
    test_data.path_to_index_map["E:/data/file3.jpg"] = {1};

    // ========== 4. 测试set_repo_path和get_repo_path ==========
    std::cout << "测试：修改仓库路径..." << std::endl;
    const std::filesystem::path new_repo_path = "D:/project_new";
    ensure_directory_exists(new_repo_path); // 确保新路径存在
    serializer.set_repo_path(new_repo_path);
    assert(serializer.get_repo_path() == new_repo_path && "仓库路径修改失败");
    // 改回原测试路径，继续后续测试
    serializer.set_repo_path(test_repo_path);
    std::cout << "✓ 路径修改测试通过" << std::endl;

    // ========== 5. 测试data_to_json（数据转JSON） ==========
    std::cout << "测试：数据转JSON..." << std::endl;
    json json_data = serializer.data_to_json(test_data);
    // 验证JSON中的数据与原数据一致
    assert(json_data["index_to_tag"] == test_data.index_to_tag && "index_to_tag 转JSON失败");
    assert(json_data["path_to_index_map"] == test_data.path_to_index_map && "path_to_index_map 转JSON失败");
    std::cout << "✓ 数据转JSON测试通过" << std::endl;

    // ========== 6. 测试serialize_to_file（序列化到文件） ==========
    std::cout << "测试：序列化到文件..." << std::endl;
    // 先清理可能存在的旧测试文件
    clean_test_file(test_repo_path, config_file);
    // 执行序列化
    bool serialize_ok = serializer.serialize_to_file(test_data);
    assert(serialize_ok && "序列化到文件失败");
    // 验证文件是否存在
    std::filesystem::path config_file_path = test_repo_path / config_file;
    assert(std::filesystem::exists(config_file_path) && "配置文件未生成");
    std::cout << "✓ 序列化到文件测试通过" << std::endl;

    // ========== 7. 测试deserialize_from_file（从文件反序列化） ==========
    std::cout << "测试：从文件反序列化..." << std::endl;
    FileTagData loaded_data = serializer.deserialize_from_file();
    // 验证反序列化后的数据与原数据一致
    assert(loaded_data.index_to_tag == test_data.index_to_tag && "index_to_tag 反序列化失败");
    assert(loaded_data.path_to_index_map == test_data.path_to_index_map && "path_to_index_map 反序列化失败");
    std::cout << "✓ 从文件反序列化测试通过" << std::endl;

    // ========== 8. 测试边界情况：文件不存在时的反序列化 ==========
    std::cout << "测试：文件不存在时的反序列化..." << std::endl;
    // 清理测试文件
    clean_test_file(test_repo_path, config_file);
    FileTagData empty_data = serializer.deserialize_from_file();
    // 验证空数据（此时JSON对象为空，赋值后容器也为空）
    assert(empty_data.index_to_tag.empty() && "文件不存在时index_to_tag应为空");
    assert(empty_data.path_to_index_map.empty() && "文件不存在时path_to_index_map应为空");
    std::cout << "✓ 文件不存在时的反序列化测试通过" << std::endl;

    // ========== 9. 清理测试资源 ==========
    clean_test_file(test_repo_path, config_file);
    // 可选：清理临时创建的新目录（如果为空）
    if (std::filesystem::exists(new_repo_path) && std::filesystem::is_empty(new_repo_path)) {
        std::filesystem::remove(new_repo_path);
    }

    std::cout << "\n所有测试均通过！" << std::endl;

    return 0;
}