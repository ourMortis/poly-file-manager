#include "bidirectional_map.hpp"
#include <cassert>
#include <iostream>
#include <string>

void test_bimap() {
    // 测试 T1 和 T2 不同的情况
    BiMap<int, std::string> bm1;

    // 测试插入 T1 和 T2
    assert(bm1.insert_T1(1));
    assert(bm1.insert_T2("test"));

    // 测试关联
    assert(bm1.associate(1, "test"));
    assert(bm1.contains_association(1, "test"));

    // 测试计数
    assert(bm1.count_associated_T1(1) == 1);
    assert(bm1.count_associated_T2("test") == 1);

    // 测试获取所有 T1 和 T2
    auto all_T1 = bm1.get_all_T1();
    auto all_T2 = bm1.get_all_T2();
    assert(all_T1.size() == 1 && *all_T1.begin() == 1);
    assert(all_T2.size() == 1 && *all_T2.begin() == "test");

    // 测试删除
    assert(bm1.erase_T1(1));
    assert(!bm1.contains_T1(1));
    assert(!bm1.contains_association(1, "test"));

    // 测试 T1 和 T2 相同的情况
    BiMap<int, int> bm2;

    // 测试插入 T1 和 T2
    assert(bm2.insert_T1(1));
    assert(bm2.insert_T2(2));

    // 测试关联
    assert(bm2.associate(1, 2));
    assert(bm2.contains_association(1, 2));

    // 测试计数
    assert(bm2.count_associated_T1(1) == 1);
    assert(bm2.count_associated_T2(2) == 1);

    // 测试获取关联
    auto associated_T2 = bm2.get_associated_T1(1);
    auto associated_T1 = bm2.get_associated_T2(2);
    assert(associated_T2.size() == 1 && *associated_T2.begin() == 2);
    assert(associated_T1.size() == 1 && *associated_T1.begin() == 1);

    // 测试删除
    assert(bm2.erase_T1(1));
    assert(!bm2.contains_T1(1));
    assert(!bm2.contains_association(1, 2));

    std::cout << "All tests passed!" << std::endl;
}

int main() {
    test_bimap();
    return 0;
}