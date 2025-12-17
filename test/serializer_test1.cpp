#include "serializer.h"
#include <iostream>
#include <iomanip>

// 辅助函数：打印FileTagData的内容，用于验证序列化/反序列化结果
void print_file_tag_data(const FileTagData& data) {
    std::cout << "\n==================== FileTagData Content ====================" << std::endl;

    // 打印tag_to_paths_map
    std::cout << "\n1. tag_to_paths_map:" << std::endl;
    if (data.tag_to_paths_map.empty()) {
        std::cout << "  (empty)" << std::endl;
    } else {
        for (const auto& pair : data.tag_to_paths_map) {
            std::cout << "  Tag: " << std::quoted(pair.first) << " -> Paths: ";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                std::cout << std::quoted(pair.second[i].string());
                if (i != pair.second.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << std::endl;
        }
    }

    // 打印path_to_tags_map
    std::cout << "\n2. path_to_tags_map:" << std::endl;
    if (data.path_to_tags_map.empty()) {
        std::cout << "  (empty)" << std::endl;
    } else {
        for (const auto& pair : data.path_to_tags_map) {
            std::cout << "  Path: " << std::quoted(pair.first.string()) << " -> Tags: ";
            for (size_t i = 0; i < pair.second.size(); ++i) {
                std::cout << std::quoted(pair.second[i]);
                if (i != pair.second.size() - 1) {
                    std::cout << ", ";
                }
            }
            std::cout << std::endl;
        }
    }

    std::cout << "===============================================================" << std::endl;
}

int main() {
    // --------------------------
    // 1. 初始化测试用的Serializer对象
    // --------------------------
    Serializer serializer;
    // 设置仓库路径为当前工作目录（也可以改为自定义路径，如"./test_repo"）
    FilePath repo_path = std::filesystem::current_path();
    serializer.set_repo_path(repo_path);
    std::cout << "Repository Path: " << serializer.get_repo_path().string() << std::endl;
    std::cout << "Config File: " << (repo_path / ".poly_file_manager").string() << std::endl;

    // --------------------------
    // 2. 构造测试数据
    // --------------------------
    FileTagData original_data;
    // 填充tag_to_paths_map
    original_data.tag_to_paths_map["document"] = {
        "/home/user/file1.txt",
        "/home/user/report.pdf",
        "D:\\docs\\notes.md" // 测试Windows路径
    };
    original_data.tag_to_paths_map["picture"] = {
        "/home/user/photo.jpg",
        "/home/user/pic.png"
    };
    original_data.tag_to_paths_map["empty_tag"] = {}; // 测试空vector

    // 填充path_to_tags_map
    original_data.path_to_tags_map["/home/user/file1.txt"] = {"document", "text", "txt"};
    original_data.path_to_tags_map["/home/user/photo.jpg"] = {"picture", "jpg", "image"};
    original_data.path_to_tags_map["D:\\docs\\notes.md"] = {"document", "markdown", "notes"};
    original_data.path_to_tags_map["/home/user/empty_path"] = {}; // 测试空vector

    std::cout << "\n----- Original Data -----" << std::endl;
    print_file_tag_data(original_data);

    // --------------------------
    // 3. 序列化到文件
    // --------------------------
    bool serialize_success = serializer.serialize_to_file(original_data);
    if (serialize_success) {
        std::cout << "\n✅ Serialization to file succeeded!" << std::endl;
    } else {
        std::cerr << "\n❌ Serialization to file failed!" << std::endl;
        return 1;
    }

    // --------------------------
    // 4. 从文件反序列化
    // --------------------------
    FileTagData deserialized_data = serializer.deserialize_from_file();
    std::cout << "\n----- Deserialized Data -----" << std::endl;
    print_file_tag_data(deserialized_data);

    // --------------------------
    // 5. 简单验证（可选：对比原始数据和反序列化数据的关键内容）
    // --------------------------
    bool verify_success = true;
    // 验证tag_to_paths_map的关键项
    if (original_data.tag_to_paths_map["document"] != deserialized_data.tag_to_paths_map["document"]) {
        verify_success = false;
    }
    if (original_data.tag_to_paths_map["picture"] != deserialized_data.tag_to_paths_map["picture"]) {
        verify_success = false;
    }
    // 验证path_to_tags_map的关键项
    if (original_data.path_to_tags_map["/home/user/file1.txt"] != deserialized_data.path_to_tags_map["/home/user/file1.txt"]) {
        verify_success = false;
    }

    if (verify_success) {
        std::cout << "\n✅ Data verification succeeded! Original and deserialized data match." << std::endl;
    } else {
        std::cerr << "\n❌ Data verification failed! Original and deserialized data do not match." << std::endl;
    }

    return 0;
}