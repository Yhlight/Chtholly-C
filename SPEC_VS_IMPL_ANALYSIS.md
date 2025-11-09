# Chtholly 规范与实现差异分析报告

本文档旨在记录 Chtholly 语言规范 (`Chtholly.md`) 与当前编译器实现及测试之间存在的差异，并为后续的统一工作提供决策依据。

---

### 差异点 1: 访问修饰符 (Access Modifiers)

*   **特性**: 结构体字段和方法的权限控制。
*   **`Chtholly.md` 规范**: 规范中的示例代码在每个字段或方法前独立声明访问权限，例如 `private name: string,` 和 `public add() ...`。
    ```chtholly
    struct Test {
        private name: string,
        public add(x, y) -> int { ... }
    }
    ```
*   **实现/测试现状**: 代码的实现（`Parser.cpp` 和 `Transpiler.cpp`）采用了类似 C++/C# 的方式，使用 `public:` 和 `private:` 作为标签，在其后定义的所有成员都遵循该访问级别，直到遇到下一个访问修饰符标签。测试用例 `TranspilerTests.cpp: StructAccessModifiers` 也验证了这种行为。
    ```cpp
    // TranspilerTests.cpp 中的源码
    struct Test {
        public:
            x: int;
            func foo() {}
        private:
            y: int;
            func bar() {}
    }
    ```
*   **分析与建议**:
    *   **存在差异**: 是。
    *   **分析**: 实现的方式更符合主流编程语言的习惯，也更加简洁，可以减少大量的重复性代码。规范中的逐行定义方式显得较为冗长。
    *   **建议**: **实现优于规范**。建议更新 `Chtholly.md`，将访问修饰符的用法修改为与当前实现一致的标签形式。

### 差异点 2: `Result` 类型的大小写与用法

*   **特性**: 错误处理机制。
*   **`Chtholly.md` 规范**: 规范文档通篇使用小写的 `result`，例如 `result<void, string>` 和 `result::fail("...")`。用法上存在模糊之处，既描述为返回一个可调用 `.is_fail()` 方法的对象，又展示了类似静态方法调用的 `result::is_fail(表达式)`。
*   **实现/测试现状**: 从 `Transpiler.cpp` 的 `get_type` 和 `visitCallExpr` 等方法的实现来看，代码期望的是大写的 `Result` 类型，并将其转换为 C++ 的 `Result` 类（需要包含 `Result.h`）。`Result::pass()` 和 `Result::fail()` 被实现为创建 `Result` 对象的工厂方法。
*   **分析与建议**:
    *   **存在差异**: 是。
    *   **分析**: 在 C++ 系语言中，类型名称普遍使用大驼峰命名法（PascalCase），因此 `Result` 比 `result` 更符合常规。将 `pass` 和 `fail` 作为类型的静态方法来构造实例，是一种清晰且常见的设计模式。
    *   **建议**: **实现优于规范**。建议将 `Chtholly.md` 中所有的 `result` 修改为 `Result`，并明确其用法为通过 `Result::pass(...)` 和 `Result::fail(...)` 来构造实例。

### 差异点 3: `switch` 语句的 `break` 行为

*   **特性**: `switch` 分支控制流。
*   **`Chtholly.md` 规范**: 规范明确指出 `break` 用于“跳出匹配”，而 `fallthrough` 用于“穿透”。这暗示着如果没有 `break`，行为可能是未定义的，或者需要显式写 `break` 来结束 `case`。
    ```chtholly
    case 值1: {
        break;  // break现在不是防止穿透，而是跳出匹配
    }
    ```
*   **实现/测试现状**: `Transpiler.cpp` 的 `visitCaseStmt` 方法实现了一个更安全的机制：如果在 `case` 的块作用域内没有找到 `break` 或 `fallthrough` 语句，它会自动在生成的 C++ 代码末尾添加一个 `break;`。`TranspilerTests.cpp: SwitchStatement` 测试也验证了这一点。
*   **分析与建议**:
    *   **存在差异**: 是。
    *   **分析**: 实现的行为（默认 `break`）可以有效防止 C/C++ 中常见的意外 `case` 穿透错误，这通常被认为是更现代、更安全的设计。
    *   **建议**: **实现优于规范**。建议更新 `Chtholly.md`，明确说明 `switch` 的 `case` 默认会自动中断，只有在需要时才使用 `fallthrough` 关键字来执行穿透。

### 差异点 4: 可变引用 (`&mut`) 的表示法

*   **特性**: 可变引用的语法。
*   **`Chtholly.md` 规范**: 规范在描述函数参数所有权时，使用了 `x3: mut& string` 的写法，但在描述 `self` 关键字时，又使用了 `&mut self` 的写法。这两种写法不一致。
*   **实现/测试现状**: `Parser.cpp` 的 `type()` 方法中，解析引用的逻辑是先匹配 `&`，再匹配可选的 `mut`，即 `&mut T` 的形式。`functionDeclaration` 中解析 `self` 的逻辑也遵循 `&mut self`。
*   **分析与建议**:
    *   **存在差异**: 是，主要在于规范自身的描述不一致。
    *   **分析**: `&mut T` 的形式源自 Rust，语法结构清晰，将修饰符 `mut` 紧跟在引用符号 `&` 之后，明确表示这是一个“可变”的“引用”。`mut& T` 的写法较为罕见且容易产生歧义。
    *   **建议**: **实现与更优的规范写法一致**。建议统一 `Chtholly.md` 中的所有相关描述，全部采用 `&mut T` 的形式。

---

### 一致性较好的方面

除了以上差异点，在很多核心特性上，规范与实现保持了良好的一致性：

*   **变量声明 (`let`/`mut`)**: 实现与规范完全一致。
*   **函数与结构体泛型**: 实现支持了规范中描述的泛型定义、约束和特例化。
*   **Lambda 表达式和 `function` 类型**: 实现与规范一致，能够正确地将 `function` 类型转换为 `std::function`。
*   **操作符重载 (`operator::add` 等)**: 实现与规范一致，通过 trait 机制将操作符映射到方法调用。
*   **模块系统 (`import`)**: 文件导入和标准库导入的基本行为与规范一致。
