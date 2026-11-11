# programming


## 加载快捷命令

需要 Python 3，以及支持 C++20 的 GCC 或 Clang。Python 工具仅依赖标准库。

在项目根目录执行：

```bash
source tools/compro.sh
```

当前终端随即可以使用下列命令；从其他目录执行也会操作这个项目，且不会改变终端当前目录。

| 命令 | 作用 |
| --- | --- |
| `cplisten` | 接收 Competitive Companion 发送的一题样例，保存后自动退出 |
| `cprun` | 自动生成 `output.cpp`，按需编译并测试全部样例 |
| `dbrun` | 自动生成 `output.cpp`，按需编译并运行，等待手动输入 |
| `cpsave abc123_a` | 将 `main.cpp` 原样保存为 `archive/abc123_a.cpp` |

要在每次打开终端时自动加载，在 WSL Bash 的 `~/.bashrc` 或 macOS Zsh 的 `~/.zshrc` 中加入：

```bash
source "/你的路径/programming/tools/compro.sh"
```

两台机器分别填写各自的目录路径。修改后重新打开终端，或在当前终端执行这一行即可。
`compro.sh` 需要通过 `source` 加载。

## 日常流程

```bash
cplisten          # 然后在浏览器的题目页面点击 Competitive Companion 按钮
cprun             # 编写 main.cpp 后测试全部样例
dbrun             # 需要时手动输入调试
cpsave abc123_a   # 需要暂存当前题时归档，文件名不必带 .cpp
```

每次执行 `cprun` 或 `dbrun`，都会先将本地头文件展开到 `output.cpp`，再编译或复用 `a.out`。
展开失败会停止执行，保留原来的 `output.cpp`；展开成功后即使样例未通过，也会保留新生成的提交代码。
提交时使用根目录的 `output.cpp`，无需单独运行展开命令。

工具提示统一使用 `[INFO]`，错误使用 `[FAIL]`，样例通过使用 `[PASS]`。
输入输出数据和生成的 C++ 代码保持原始内容。

## 获取样例：cplisten

安装浏览器扩展 [Competitive Companion](https://github.com/jmerle/competitive-companion)，执行 `cplisten` 后，
在 AtCoder、Codeforces 或洛谷的单题页面点击扩展按钮。

样例保存到项目根目录：

```text
samples/
  sample-1.in
  sample-1.out
  sample-2.in
  sample-2.out
```

收到有效题目后会替换整个 `samples/`，包含其中的其他文件和子目录；无样例的题目会将目录清空。
无效请求或保存失败会保留旧数据，并继续等待。成功保存一题后自动关闭监听、释放端口。
批量导入比赛时，只接收第一道成功保存的题目。

默认监听 `127.0.0.1:27121`，该端口在扩展默认发送列表中。等待时可按 `Ctrl+C` 取消。
如需换端口：

```bash
cplisten --port 12345
```

同时在扩展的 **Custom ports** 设置中添加相同端口。

## 测试全部样例：cprun

```bash
cprun
cprun --timeout 5
```

按编号依次运行 `samples/*.in`，与同名 `.out` 比较，每个样例默认最多运行 2 秒。
结果仅分为 `[PASS]` 和 `[FAIL]`。通过时只显示样例名、耗时和结果；失败时显示：

```text
[INFO] sample-1
[INFO] time: 0.001234 sec
[FAIL]
[input]
5
[output]
2
[expected]
3
----------------------------------------
```

无论通过还是失败，程序的标准错误输出都会单独显示在 `[stderr]` 下，且不参与答案比较。
本地编译定义了 `LOCAL`，`SHOW(...)` 会写入标准错误，供调试查看；普通 `print(...)` 的标准输出才参与比较。
提交时未定义 `LOCAL`，`SHOW(...)` 不执行。最后汇总通过数量。
长内容只展示前 40 行，每行最多 200 个字符，比较时仍使用完整输出。
比较忽略空白差异，不使用浮点误差容限或特殊判题逻辑。

## 手动输入：dbrun

```bash
dbrun
```

直接运行程序，由 `main.cpp` 中的 `cin` / `scanf` 读取终端输入，按 `Ctrl+C` 中断。
程序需要读取到 EOF 时，可在 WSL 或 macOS 终端中按 `Ctrl+D` 结束输入。

`a.out` 会保留，也可以在项目根目录执行 `./a.out` 再次运行；直接运行不会检查代码变化或生成提交代码。

## 归档：cpsave

```bash
cpsave abc123_a
```

将当前 `main.cpp` 原样复制到 `archive/abc123_a.cpp`，保留本地引用和原始内容。
自动补上 `.cpp`，也支持显式填写后缀。同名归档已存在时拒绝覆盖，当前 `main.cpp` 会保留。

## 编译参数与缓存

`cprun` 和 `dbrun` 共用根目录的 `a.out` 和 `.build/` 中的编译记录。
默认编译参数为 `-std=c++20 -O2 -Wall -Wextra -DLOCAL -I 项目根目录`。
首次运行会编译，之后根据源码、实际引用的本地头文件内容和编译参数决定是否重新编译。
编译器自动记录嵌套头文件依赖，编译失败时停止，不运行旧程序。

```bash
cprun --force                   # 强制重编译并测试
dbrun --std c++17               # 更换 C++ 标准
cprun --cxx g++-15 --timeout 5   # 指定编译器和样例时限
```

编译器也可通过各机器的 `CXX` 环境变量指定。macOS 下如果 `g++` 实际是 Apple Clang，
而代码使用了 `<bits/stdc++.h>`，需要指定已安装的 GNU GCC，例如：

```bash
export CXX=g++-15
```

具体名称以本机安装版本为准。更换编译器或编译参数会重新编译；更新系统头文件后可使用 `--force`。
`a.out`、`output.cpp`、`.build/` 和 `samples/` 已被 Git 忽略，每台机器独立生成或获取。

## 目录与底层工具

| 路径 | 用途 |
| --- | --- |
| `main.cpp` | 当前题目的代码 |
| `book/` | 可复用的算法模板 |
| `samples/` | 当前题目的样例输入和期望输出 |
| `archive/` | 原样归档的题目代码 |
| `output.cpp` | 自动展开生成的完整提交代码 |
| `a.out` | 本地编译产物 |
| `.clang-format` | 统一的代码格式规则 |
| `tools/compro.sh` | Bash / Zsh 快捷命令入口 |

Python 工具也可在项目根目录单独调用：

| 命令 | 用途 |
| --- | --- |
| `python3 tools/samples.py` | 接收一题样例 |
| `python3 tools/test.py` | 按需编译并测试样例 |
| `python3 tools/run.py` | 按需编译并手动输入运行 |
| `python3 tools/build.py` | 仅按需编译，可加 `--force` |
| `python3 tools/expander.py main.cpp > output.cpp` | 展开本地引用 |
| `python3 tools/archive.py abc123_a` | 原样归档当前代码 |

自动生成提交代码由 Shell 快捷命令负责；单独调用 `run.py` 或 `test.py` 仅编译和运行。
