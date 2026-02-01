# poly-file-manager



> Multilingual support: [English](#poly-file-manager) | [中文](#poly-file-manager-中文)

## Description

The primary function of this software is to categorize and manage files. The software is only responsible for organizing the structure and assisting the file manager of the operating system to display it.

## Features

Attach tags to files.

Establish a category repository for quick tag based queries.

Automatically organize symbolic link(shortcut in Windows) in the repository directory.

Supports Windows.

## Installation

**Direct installation**

In Progress

**Compile requirements**

1. cmake version 4.x

2. MinGW-w64 or GCC supporting C++23

```
git clone ...
```

Select and compile the content you need through CMakeLists.txt

Add the program to the environment variable

## Usage

**command line tool**

**General Syntax**

```
# Add or remove multiple labels to a path
pfm [--repo REPOSITORY_PATH] [-r] <PATH> -t <TAG>...

# Add or remove the same label to multiple paths
pfm [--repo REPOSITORY_PATH] [-r] <TAG> -p <PATH>...

# Create a repository or check if the repository content is consistent with the data or synchronize the repository content to be consistent with the data, if not specified with the --repo option, defaults to the current working directory
pfm [--repo REPOSITORY_PATH] (-c|--check|-s)

# Add, delete, or modify paths or labels
pfm [--repo REPOSITORY_PATH] (tag|path) (-a <ARG>...| -r <ARG>...| -m <OLD_ARG> <NEW_ARG>)

# Query paths with multiple labels simultaneously
pfm [--repo REPOSITORY_PATH] <TAG>...

# Query tags containing multiple paths
pfm [--repo REPOSITORY_PATH] -p <PATH>...
```

**Subcommands**

tag: Specially manage tags

path: Specially manage paths

**Options**

|             Option              |                         Description                          |
| :-----------------------------: | :----------------------------------------------------------: |
|         `--repo <path>`         | Specify the repository path (defaults to current working directory if not provided) |
|          `-r,--remove`          | Remove associations between a tag and a path with this flag  |
|         `-c, --create`          |  Create a new repository(Uses --repo to specify directory)   |
|          `tag_or_path`          | This positional argument must be a tag if the "-t" option is used, or a path if the "-p" option is used |
|           `-t,--tag`            |       Specify one or more tags (supports 1-99 values)        |
|           `-p,--path`           | Specify one or more file/directory paths (supports 1-99 values) |
|           `-a,--add`            |             Add tags or paths using subcommands              |
| `-r,--remove`(using subcommand) |            Remove tags or paths using subcommands            |
|    `-m,--modify <old> <new>`    | Modify tag or path using subcommands. This must have two parameters, one old and one new |
|            `--check`            | Check if the repository content is consistent with the data  |
|           `-s,--sync`           | Synchronize the repository content to be consistent with the data |

## License

GPLv3

## Acknowledgements

nlohmann/json

CLI11

# poly-file-manager 中文

## 描述

本软件的核心功能是对文件进行分类与管理。软件仅负责文件结构的组织，并辅助操作系统的文件管理器展示该结构。

## 功能特性

1. 为文件添加标签
2. 建立分类仓库，支持基于标签的快速查询
3. 自动整理仓库目录下的符号链接（即 Windows 系统中的快捷方式）
4. 支持 Windows 系统

## 安装

### 直接安装

开发中

### 编译环境要求

1. CMake 4.x 版本
2. 支持 C++23 标准的 MinGW-w64 或 GCC 编译器

```
git clone ...
```

通过 CMakeLists.txt 文件选择并编译你需要的功能模块

将程序添加到环境变量

## 使用方法

**命令行工具**

**通用语法**

```
# 为单个路径 添加/删除 多个标签
pfm [--repo 仓库路径] [-r] <目标路径> -t <标签>...

# 为多个路径 添加/删除 同一个标签
pfm [--repo 仓库路径] [-r] <标签> -p <目标路径>...

# 创建仓库/检查仓库内容是否与数据一致/同步仓库内容到与数据一致，若未指定 --repo 参数，则默认在当前工作目录创建
pfm [--repo 仓库路径] (-c|--check|-s)

# 新增、删除或修改路径与标签
pfm [--repo 仓库路径] (tag|path) (-a <参数>...| -r <参数>...| -m <旧参数> <新参数>)

# 查询同时具有多个标签的路径
pfm [--repo 仓库路径] <标签>...

查询含有多个路径的标签
pfm [--repo 仓库路径] -p <路径>...
```

#### 子命令

- tag：专门用于管理标签
- path：专门用于管理路径

#### 选项说明

|              选项               | 描述                                                         |
| :-----------------------------: | ------------------------------------------------------------ |
|         `--repo <路径>`         | 指定仓库路径（若未指定，默认使用当前工作目录）               |
|          `-r,--remove`          | 使用此参数，移除标签与路径之间的关联关系                     |
|         `-c, --create`          | 创建新仓库（需通过 --repo 参数指定仓库目录）                 |
|          `tag_or_path`          | 位置参数：当使用 `-t` 选项时，该参数为标签；当使用 `-p` 选项时，该参数为路径 |
|           `-t,--tag`            | 指定一个或多个标签（最多支持 99 个标签）                     |
|           `-p,--path`           | 指定一个或多个文件 / 目录路径（最多支持 99 个路径）          |
|           `-a,--add`            | 通过子命令新增标签或路径                                     |
|   `-r,--remove`（子命令模式）   | 通过子命令删除标签或路径                                     |
| `-m,--modify <旧参数> <新参数>` | 通过子命令修改标签或路径，该选项必须传入两个参数：旧参数与新参数 |
|            `--check`            | 检查仓库内容是否与数据一致                                   |
|           `-s,--sync`           | 同步仓库内容到与数据一致                                     |

## 许可证

GPLv3 协议

## 致谢

- nlohmann/json 库
- CLI11 库
