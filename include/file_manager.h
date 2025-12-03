#pragma once

#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <set>
#include <memory>
#include <algorithm>
#include <vector>

using file_path = std::filesystem::path;
using file_tag = std::string;

using sptr_file_path = std::shared_ptr<file_path>;
using sptr_file_tag = std::shared_ptr<file_tag>;

class FileManager
{
public:
    FileManager();
    ~FileManager();

    void create_tag(file_tag tag);
    void change_tag(const file_tag &old_tag, file_tag new_tag);
    void delete_tag(const file_tag &tag);
    std::vector<file_tag> get_tags();

    void create_path(file_path path);
    void change_path(const file_path &old_path, file_path new_path);
    void delete_path(const file_path &path);
    std::vector<file_path> get_paths();

    void add_file_tag(file_path &path, file_tag tag);
    void delete_file_tag(file_path &path, file_tag tag);


    std::vector<file_tag> get_tags(file_path path);
    
    std::vector<file_path> get_paths(file_tag tag);

    // template<template<typename...> class Container, typename T, typename E>
    // auto get_extracted(const Container<T>& container)
    // {
    //     Container<E> res;
    //     for(const auto i : container)
    //     {
    //         res.insert(*i);
    //     }
    //     return res;
    // }

    void print_tags();
    void print_tag_with_paths();
    void print_paths();
    void print_path_with_tags();


private: 
    std::unordered_map<file_tag, sptr_file_tag> tags;
    std::unordered_map<sptr_file_tag, std::set<sptr_file_path>> tag_with_paths;
    std::unordered_map<file_path, sptr_file_path> paths;
    std::unordered_map<sptr_file_path, std::set<sptr_file_tag>> path_with_tags;

};