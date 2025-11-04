## 项目环境
```
sudo apt-get update && \
sudo apt-get install -y \
  g++ \
  build-essential \
  libzstd-dev \
  libffi-dev
```

## 项目要求
1. Chtholly是一门新的编程语言，语法规范文档是Chtholly.md，采用转译C++的形式，而非LLVM代码生成
2. Chtholly项目基于C++17 / C++20编写，使用CMake构建，建议创建一个Python的CMake构建脚本(包括检索与调用生成的二进制文件)
3. 使用.gitignore进行版本控制，不要提交构建产物与二进制文件(使用版本控制隔离，特别是CMake的构建产物)
4. TDD驱动，以稳定项目为主，增量实现，避免回归，且使用LL(K)递归下降解析算法 / 更适用于Chtholly的解析算法
5. 每一个计划的推进前，都需要重新阅读Chtholly.md中相关语法规范，确保实现不偏离
6. 需要创建一个RoadMap.md记录项目的进度，每一个功能的实现都需要通过此文档进行反馈
