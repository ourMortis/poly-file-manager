#include "windows_shortcut_creator.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

namespace fs = std::filesystem;

// 辅助函数：创建临时文件
fs::path create_temp_file(const std::string& name) {
    fs::path temp_path = fs::temp_directory_path() / name;
    std::ofstream(temp_path).close(); // 创建空文件
    return temp_path;
}

// 辅助函数：创建临时目录
fs::path create_temp_dir(const std::string& name) {
    fs::path temp_path = fs::temp_directory_path() / name;
    fs::create_directory(temp_path);
    return temp_path;
}

// 验证快捷方式是否存在
bool shortcut_exists(const fs::path& shortcut_path) {
    return fs::exists(shortcut_path) && 
           shortcut_path.extension() == ".lnk";
}

// 执行单个测试用例
void run_test(const std::string& test_name, bool result) {
    std::cout << "Test " << test_name << ": " 
              << (result ? "[PASS]" : "[FAIL]") << std::endl;
}

int main() {
    ShortcutCreator creator;
    fs::path target_file, target_dir;
    bool test_result;

    // 初始化测试目标（文件和目录）
    try {
        target_file = create_temp_file("test_target.txt");
        target_dir = create_temp_dir("test_target_dir");
        std::cout << "测试环境初始化完成，临时路径：" << fs::temp_directory_path() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "初始化失败：" << e.what() << std::endl;
        return 1;
    }

    // 1. 测试文件路径包含'/'的情况
    {
        fs::path shortcut_path = fs::temp_directory_path() / "test_file_with_slash.lnk";
        test_result = creator.create(target_file, shortcut_path);
        if (test_result) test_result = shortcut_exists(shortcut_path);
        run_test("文件路径包含'/'", test_result);
        if (fs::exists(shortcut_path)) fs::remove(shortcut_path);
    }

    // 2. 测试文件路径包含'\\'的情况
    {
        std::string base = fs::temp_directory_path().string();
        fs::path shortcut_path = base + "\\test_file_with_backslash.lnk";
        test_result = creator.create(target_file, shortcut_path);
        if (test_result) test_result = shortcut_exists(shortcut_path);
        run_test("文件路径包含'\\'", test_result);
        if (fs::exists(shortcut_path)) fs::remove(shortcut_path);
    }

    // 3. 测试文件路径以'/'结尾的情况
    {
        fs::path target_with_slash = target_file.string() + "/";
        fs::path shortcut_path = fs::temp_directory_path() / "test_file_slash_end.lnk";
        test_result = creator.create(target_with_slash, shortcut_path);
        if (test_result) test_result = shortcut_exists(shortcut_path);
        run_test("文件路径以'/'结尾", test_result);
        if (fs::exists(shortcut_path)) fs::remove(shortcut_path);
    }

    // 4. 测试文件路径以'\\'结尾的情况
    {
        fs::path target_with_bs = target_file.string() + "\\";
        fs::path shortcut_path = fs::temp_directory_path() / "test_file_bs_end.lnk";
        test_result = creator.create(target_with_bs, shortcut_path);
        if (test_result) test_result = shortcut_exists(shortcut_path);
        run_test("文件路径以'\\'结尾", test_result);
        if (fs::exists(shortcut_path)) fs::remove(shortcut_path);
    }

    // 5. 测试目录路径包含'/'的情况
    {
        fs::path shortcut_path = fs::temp_directory_path() / "test_dir_with_slash.lnk";
        test_result = creator.create(target_dir, shortcut_path);
        if (test_result) test_result = shortcut_exists(shortcut_path);
        run_test("目录路径包含'/'", test_result);
        if (fs::exists(shortcut_path)) fs::remove(shortcut_path);
    }

    // 6. 测试目录路径包含'\\'的情况
    {
        std::string base = fs::temp_directory_path().string();
        fs::path shortcut_path = base + "\\test_dir_with_backslash.lnk";
        test_result = creator.create(target_dir, shortcut_path);
        if (test_result) test_result = shortcut_exists(shortcut_path);
        run_test("目录路径包含'\\'", test_result);
        if (fs::exists(shortcut_path)) fs::remove(shortcut_path);
    }

    // 7. 测试目录路径以'/'结尾的情况
    {
        fs::path target_with_slash = target_dir.string() + "/";
        fs::path shortcut_path = fs::temp_directory_path() / "test_dir_slash_end.lnk";
        test_result = creator.create(target_with_slash, shortcut_path);
        if (test_result) test_result = shortcut_exists(shortcut_path);
        run_test("目录路径以'/'结尾", test_result);
        if (fs::exists(shortcut_path)) fs::remove(shortcut_path);
    }

    // 8. 测试目录路径以'\\'结尾的情况
    {
        fs::path target_with_bs = target_dir.string() + "\\";
        fs::path shortcut_path = fs::temp_directory_path() / "test_dir_bs_end.lnk";
        test_result = creator.create(target_with_bs, shortcut_path);
        if (test_result) test_result = shortcut_exists(shortcut_path);
        run_test("目录路径以'\\'结尾", test_result);
        if (fs::exists(shortcut_path)) fs::remove(shortcut_path);
    }

    // 清理测试环境
    try {
        if (fs::exists(target_file)) fs::remove(target_file);
        if (fs::exists(target_dir)) fs::remove_all(target_dir);
    } catch (const std::exception& e) {
        std::cerr << "清理失败：" << e.what() << std::endl;
    }

    return 0;
}