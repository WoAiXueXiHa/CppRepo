VIP 商场消费查询系统（简洁版）

简介
- 这是一个用 C++ 编写的简易 VIP 消费管理系统，演示类的继承、仿函数、STL 容器、以及基于 POSIX 的文件读写（系统调用）。
- 目标平台：Ubuntu + g++（已按 C++98 标准编译）

项目结构
- include/ : 头文件（`core.h`, `user.h`, `product.h`, `transaction.h`, `db_os.h`）
- src/ : 实现文件
- Makefile : 编译规则
- users.csv, products.csv, transactions.csv : 文本数据文件（CSV）

设计要点与接口说明
- `db_os`（include/db_os.h, src/db_os.cpp）
  - read_entire_file(path, out): 使用 `open/read` 读取整个文件到 `out` 字符串。
  - write_entire_file(path, data): 使用 `open/write` 覆盖写入整个文本。
  - append_text_file(path, data): 追加写入文本。
  这些函数直接使用 POSIX 系统调用，便于展示 OS 级文件管理思想。

- `User`, `Product`, `Transaction`（include/*.h, src/*.cpp）
  - 每个类型都提供 `toCsv()` 和 `fromCsv()` 方法，用于在内存与 CSV 文本之间互转。
  - `User` 继承自 `Person`，`Product` 继承自 `Item`，`Transaction` 继承自 `Record`（演示继承）。
  - 在代码中使用了仿函数（functor）示例用于 `find_if` 查找对象。

- `VIPSystemCore`（include/core.h, src/core.cpp）
  - 管理三个 `std::vector`：`users`, `products`, `transactions`。
  - 接口：`loadAll()`, `saveAll()`, `addUser()`, `addProduct()`, `makePurchase()`, `queryTransactionsByUser()` 等。
  - 存盘/读盘操作统一通过 `db_os` 模块完成。

编译与运行
1. 在项目根目录运行：
```bash
make
```
2. 运行：
```bash
./vipsystem
```

简单测试数据文件
- 程序在当前目录使用 `users.csv`, `products.csv`, `transactions.csv`。可以用文本方式编辑或手动创建示例行。

实现说明与限制
- 以简单、易读为主，使用 CSV 文本文件保存数据（没有使用数据库）。
- 读取/写入策略为整个文件读入/整体覆盖写出，适用于小规模数据（作业需求）。
- 不使用并发/IPC；若需后台备份可考虑 fork + 子进程（留作扩展）。

如果需要我可以：
- 添加更详细的注释或接口文档（按函数/文件分节）。
- 添加样例 CSV 数据或自动初始化示例。
- 根据你想运行的交互流程调整 `main.cpp` 菜单。
