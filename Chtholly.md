## Chtholly
Chtholly是一门基于C++实现的编程语言，Chtholly以简洁，高性能，接管所有权，编译期为特征，出于个人爱好而非实现更好的编程语言
Chtholly很大程度上向Rust靠拢，但是Chtholly不是照搬Rust，Chtholly具有很多独特的语法    
也就是说，Chtholly遵循零成本抽象以及运行时极简原则，尽可能把事情交给编译期进行

Chtholly文件后缀为.cns

注意！Chtholly不是一门正规的编程语言，仅供个人学习使用，请不要用于生产环境

### 接管所有权
像Rust一样，Chtholly使用一套所有权系统来管理内存，确保内存安全和性能。核心原则是“一个值在任何时候都只有一个所有者”。

- **Move (移动)**: 对于复杂类型（如 `string`, `array`, `struct`），当它们被赋值给另一个变量或作为参数传递给函数时，所有权会发生转移（move）。原始变量将变得无效，无法再被使用。这可以防止数据竞争和悬垂指针。

- **Copy (复制)**: 对于基本类型（如 `int`, `double`, `bool`, `char`），它们的值会被复制（copy）。赋值或传参后，原始变量和新变量都持有各自独立的数据副本。

```Chtholly
// 基本类型 (int) 的 Copy 行为
let x = 10;
let y = x; // x 的值被复制给 y
print(x);  // 这是合法的，x 仍然有效，值为 10

// 复杂类型 (string) 的 Move 行为
let s1 = "hello";
let s2 = s1; // s1 的所有权被移动给 s2
// print(s1); // 这将导致编译错误！s1 不再有效
print(s2); // 这是合法的
```

### 注释
```Chtholly
// 单行注释

/*
多行注释
*/
```

### 程序入口
```Chtholly
func main(args: array[string]) -> result<void, string>
{

}
```

### 错误类型
主函数会返回一个result<T, K>类型
这个类型有两个方法表示pass和fail
T类型使用pass方法，K类型使用fail方法

pass()和fail()方法能够接收任何值作为输出信息  
这两个方法将直接决定程序的结果  

result对象共有pass，fail，is_pass，is_fail四个方法  
其中pass和fail表示结果  
而is_pass和is_fail表示过程，它们会计算表达式，返回bool值      

is_pass表示是否通过，这通常是可容忍的  
而is_fail表示是否失败，这通常是不可容忍的  
通过这两个方法的使用，你需要灵活使用if语句与pass，fail方法进行错误的处理  

```Chtholly
func main(args: array[string]) -> result<void, string>
{
    if (result::is_fail(表达式))
    {
        return result::fail("发生异常");  // 直接程序终止
    }
    return result::pass();
}
```

### 变量
Chtholly使用let声明不可变变量
使用mut声明可变变量

Chtholly编译器会自动推导变量的类型

```Chtholly
let a = 10;
let b = 30.4;
let c = 'a';
let d = "HelloWorld";
let e = [1, 2, 3, 4];

mut a2 = 10;
```

#### 引用
你可以使用&创建对应的引用变量
默认情况下属于不可变引用，即let&

所有权与引用规则如下：
- `T`: 值类型，表示所有权转移 (move)。
- `&T`: 不可变引用 (borrow)。
- `&mut T`: 可变引用 (mutable borrow)。

```Chtholly
let a = 20;
let b = &a; // b 是对 a 的不可变引用

mut a2 = 30;
let b2 = &a2; // b2 是对 a2 的不可变引用
mut c2 = &mut a2; // c2 是对 a2 的可变引用
```

#### 空值消除
Chtholly提供了一个option<T>类型
这个类型无需显性创建，而是对变量进行解包

如果需要表示没有值，请使用none

option<T>类型会提供unwarp和unwarp_or方法
unwarp在没有值时将抛出异常
unwarp_or在没有值时将使用默认值替换

```Chtholly
let a : option<int> = none;
print(a.unwarp());  // 如果a为none，程序终止
a.unwarp_or(20);  // 如果a为none，则使用20替代这个值

let b = option{ value: 20 };  // 创建option对象
let c = option<int>{ value: 20 };  // option本质是一个泛型结构体，由于自动推断的存在，因此可以省略泛型
let d = option<int>{
    value: 20
};
```

#### 类型限定
你可以在变量名后面使用: <数据类型>来限定变量的类型

Chtholly有如下内置数据类型
int(默认使用的类型)

以及精细划分的类型
int8，int16，int32，int64
uint8，uint16，uint32，uint64

char

double(默认使用的类型)

以及精细划分的类型
float，double，long double

void

bool

string

array

struct

function

```Chtholly
let a : uint8 = 25;
let b : array = ["1145", "2235", 24];
let c : array[string] = ["1145", "2345"];
```

### 枚举
Chtholly使用enum创建枚举  

```
enum color
{
    red,
    green
}

let a : color = color::red;
```

### 函数
Chtholly使用func作为函数关键字
支持C++版本的lambda函数

```Chtholly
func 函数名称(参数列表) <返回值类型>
{

}
```
返回值类型是可选部分，可以直接省略，指出更好，和类型限定一个套路

```Chtholly
func add(x: int, y: int) -> int
{
    return x + y;
}
```

#### lambda函数
Chtholly的lambda函数使用与C++完全一致的语法
默认情况下，捕获属于不可变引用

```Chtholly
let add = [](a: int, b: int) -> int { return a + b; };
```

#### function类型
Chtholly使用`function`关键字来表示函数类型，可以用于变量的类型注解。

```Chtholly
let my_func: function(int, int) -> int = add;
```

#### 参数所有权
和Rust很类似

```Chtholly
func test(x: string, x2: &string, x3: &mut string) -> int
{
    // x: string，表示接管所有权
    // x2: &string，表示不可变引用
    // x3: &mut string，表示可变引用
}
```

### 明确数组长度
你可以在类型限定中添加额外的参数项，表示数组的大小

```Chthollt
let arr2 : array[string ; 5];  // 指定类型并指定长度
```

### struct结构体
在Chtholly之中，使用struct创建结构体。不支持继承语法，可以使用组合式继承。
使用 `public:` 和 `private:` 标签来控制成员的访问权限，标签下的所有成员都将应用该权限，直到下一个标签出现。默认权限为公开（public）。
所有成员变量的声明都必须以分号结尾。

```Chtholly
struct Test
{
    private:
        name: string;  // 可以赋予默认值
        id: int;

    public:
        // 结构体内的函数不需要写func
        add(x: int, y: int) -> int
        {
            return x + y;
        }
}

func main(args: array[string])
{
    let test = Test{};  // 第一种创建方式
    let test2 = Test{  // 第二种
        name: "xxx",
        id: 18
    };

    // 第三种创建方式，按位置初始化
    let test3 = Test{"xxx", 18};

    print(test2.name);
}
```

#### 成员函数与静态函数
结构体内部的函数分为**成员函数**（也称实例方法）和**静态函数**。

- **成员函数**: 第一个参数是 `self`、`&self` 或 `&mut self`。这类函数与一个具体的结构体实例相关联，必须通过实例来调用（使用 `.` 符号）。它们用于操作或查询特定实例的数据。
- **静态函数**: 没有 `self` 参数。这类函数不与任何具体实例绑定，而是直接与结构体本身相关联。它们通过结构体名称和 `::` 符号来调用。

##### `self` 参数详解
`self` 参数决定了函数如何访问实例数据：

- **`&self`** (不可变借用): 函数将获得对实例的**不可变引用**。这是最常见的形式，允许函数读取实例的字段，但不能修改它们。
- **`&mut self`** (可变借用): 函数将获得对实例的**可变引用**。这允许函数修改实例的字段。
- **`self`** (所有权转移): 函数将获得实例的**所有权**。这意味着在调用该函数后，原始的实例变量将变得无效（其所有权被移交给了函数）。这通常用于将实例转换为另一种类型或执行会消耗掉实例的操作。

**注意**: 在方法内部，访问实例字段或调用其他成员函数时，必须显式使用 `self.` 前缀。

```Chtholly
struct Counter {
    value: int;

    public:
        // 静态函数: 创建一个新的 Counter 实例
        new() -> Counter {
            return Counter{ value: 0 };
        }

        // 成员函数 (&self): 不可变地读取数据
        get_value(&self) -> int {
            return self.value; // 使用 self. 访问字段
        }

        // 成员函数 (&mut self): 可变地修改数据
        increment(&mut self) {
            self.value = self.value + 1;
        }

        // 成员函数 (self): 消耗实例
        consume(self) {
            print("Counter with value " + self.value + " consumed.");
        }
}

func main() {
    // 调用静态函数 `new`，使用 :: 语法
    mut my_counter = Counter::new(); // my_counter.value is 0

    // 调用成员函数，使用 . 语法
    my_counter.increment(); // my_counter.value is now 1
    print(my_counter.get_value()); // 输出 1

    // 调用消耗性成员函数
    my_counter.consume();
    // 此时 my_counter 不再有效，后续的调用将导致编译错误
    // my_counter.get_value(); // -> COMPILE ERROR!
}
```

### 运算符
Chtholly 支持标准的算术、比较、逻辑和位运算符。大部分运算符的行为与 C++ 和 Rust 类似。

#### 算术运算符
- `+` (加)
- `-` (减)
- `*` (乘)
- `/` (除)
- `%` (取模)

#### 比较运算符
- `==` (等于)
- `!=` (不等于)
- `>` (大于)
- `<` (小于)
- `>=` (大于等于)
- `<=` (小于等于)

#### 逻辑运算符
- `&&` (逻辑与)
- `||` (逻辑或)
- `!` (逻辑非)

#### 位运算符 (适用于整数类型)
- `&` (按位与)
- `|` (按位或)
- `^` (按位异或)
- `~` (按位取反)
- `<<` (左移)
- `>>` (右移)

#### 赋值运算符
- `=` (赋值)
- `+=`, `-=`, `*=`, `/=`, `%=` (复合赋值)

所有这些运算符都可以通过实现 `operator` 模块中的相应 trait 来进行重载。

### 命名规则
为了保持代码的清晰和一致性，Chtholly 推荐遵循以下命名约定：

- **类型 (Structs, Enums, Traits)**: 使用 `PascalCase` 或 `UpperCamelCase`。
  - 示例: `MyStruct`, `Color`, `ToString`

- **函数和变量**: 使用 `snake_case`。
  - 示例: `my_function`, `let my_variable = ...`

- **常量**: 使用 `UPPER_SNAKE_CASE`。
  - 示例: `let MAX_CONNECTIONS = 100;`

- **泛型参数**: 使用简短的 `PascalCase`，通常是单个大写字母。
  - 示例: `func my_generic<T>(value: T) { ... }`

遵循这些约定并非强制性的，但强烈建议这样做。

### 选择结构和循环结构
与C++一致。

#### if-else 表达式
Chtholly 支持标准的 `if-else` 表达式，用于根据条件执行不同的代码块。条件表达式的类型必须是 `bool`。

```Chtholly
let number = 10;
if (number > 5) {
    print("Number is greater than 5");
} else if (number == 5) {
    print("Number is exactly 5");
} else {
    print("Number is less than 5");
}
```

#### while 循环
`while` 循环会持续执行一个代码块，只要给定条件为 `true`。条件表达式的类型也必须是 `bool`。

```Chtholly
mut i = 0;
while (i < 5) {
    print(i);
    i = i + 1;
}
```

#### for 循环
Chtholly 支持两种 `for` 循环语法：C-style 循环和 `for-each` 循环。

##### for-each 循环
这是遍历集合（如数组）的首选方式。它使用冒号 (`:`) 语法。

```Chtholly
let numbers = [10, 20, 30, 40, 50];
for (let num : numbers) {
    print(num);
}
```

在未来的版本中，`for` 循环还将支持传统的 C-style 语法（`for (let i = 0; i < 10; i = i + 1)`）。

##### 循环控制语句
Chtholly 提供了 `break` 和 `continue` 语句来精确控制循环的执行流程。

- **`break`**: 立即终止当前循环。
- **`continue`**: 跳过当前迭代的剩余部分，直接开始下一次迭代。

```Chtholly
mut i = 0;
while (i < 10) {
    if (i == 5) {
        break; // 当 i 等于 5 时，退出循环
    }
    if (i % 2 == 0) {
        i = i + 1;
        continue; // 如果 i 是偶数，跳过 print
    }
    print(i); // 只会打印 1, 3
    i = i + 1;
}
```

#### switch 语句
Chtholly的`switch`行为与C++不同，为了防止意外的穿透，每个`case`块在执行完毕后会自动中断（implicit break）。如果您确实需要穿透到下一个`case`，必须显式使用 `fallthrough` 关键字。

```Chtholly
switch(任意类型的变量 / 表达式)
{
    case 值1: {
        // 执行代码，然后自动中断
    }
    case 表达式: {
        // 执行代码，然后自动中断
    }
    case 表达式2: {
        // ...
        fallthrough;  // 使用 fallthrough 来继续执行下一个 case
    }
    case 表达式3: {
        // 表达式2的代码执行完后会进入这里
    }
}
```

### 泛型
#### 泛型函数
```Chtholly
// 定义一个泛型函数，接受一个类型参数 T
func swap_values<T>(a: &mut T, b: &mut T)  // T可以指定默认值，使用<T = int>即可
{
    // 必须通过可变引用 (&mut T) 来修改值
    let temp = *a; // 自动解引用 (Dereference) 获取值
    *a = *b;
    *b = temp;
}

func swap_values<string>(a: &mut string, b: &mut string)  // 特例化操作
{

}

func main(args: array[string]) -> result<void, string>
{
    mut num1 = 100;
    mut num2 = 200;
    swap_values(&mut num1, &mut num2); // 编译器自动推断 T 为 int
    // num1 现在是 200, num2 现在是 100

    mut s1 = "Alpha";
    mut s2 = "Beta";
    swap_values<string>(&mut s1, &mut s2); // 显式指定 T 为 string
    // s1 现在是 "Beta", s2 现在是 "Alpha"

    return result::pass();
}
```

#### 泛型结构体
```Chtholly
// 定义一个泛型结构体 Point，它接受一个类型参数 T
struct Point<T>  // 这个T可以写默认值，使用<T = int>即可指定默认值，同理，也具有特例化操作，例如<int>
{
    x: T;
    y: T;

    // 结构体内的函数也可以使用泛型 T
    public:
        swap(&mut self)
        {
            // 交换 x 和 y 的值
            let temp = self.x;
            self.x = self.y;
            self.y = temp;
        }
}

func main(args: array[string]) -> result<void, string>
{
    // 实例化 Point<int>
    let p1 = Point{ x: 10, y: 20 };
    // 编译器会自动推导出类型为 Point<int>

    // 实例化 Point<double>
    mut p2 = Point{ x: 1.5, y: 3.8 };
    // 编译器推导出类型为 Point<double>

    p2.swap();
    // p2 现在是 { x: 3.8, y: 1.5 }

    // 实例化 Point<string>
    let p3 : Point<string> = Point{
        x: "Hello",
        y: "World"
    };

    return result::pass();
}
```

#### 结构体内的泛型函数
在Chtholly之中，结构体，无论自身是否是泛型，都可以拥有泛型成员函数。

##### 常规结构体内的泛型函数
```Chtholly
// 一个常规的、非泛型的结构体
struct Printer {
    // 拥有一个泛型方法，可以打印任何类型的值
    public:
        print<T>(self, value: T) {
            // ... 具体的打印逻辑
        }
}

func main() {
    let p = Printer{};
    p.print<int>(10);       // 调用时指定类型
    p.print("hello");     // 或者让编译器自动推断类型
}
```

##### 泛型结构体内的泛型函数
泛型结构体内部也可以创建独立的泛型函数。
```Chtholly
struct Point<T>
{
  public:
      // 方法的泛型参数 K, F 与结构体的泛型参数 T 是独立的
      test<K, F>(self) // 需要对象调用
      {}

      test2<K, F>() // 不需要对象调用 (静态方法)
      {}
      // 和前面是一样的，默认类型，类型特例化，都得到支持
}

func main(args: array[string]) -> result<void, string>
{
    let t: Point<string> = Point{};
    t.test<int, bool>();
    Point::test2<char, string>();
    return result::pass();
}
```

### 约束
Chtholly使用`?`符号来为泛型参数指定约束。

```Chtholly
// 假设 Comparable 特性已定义，它要求实现一个 gt(大于) 方法
trait Comparable<T>
{
    // 也支持泛型函数
    func gt(&self, other: &T) -> bool;
}

// 定义一个泛型结构体 value，并实现 Comparable 约束
struct value<T> impl Comparable<value<T>>
{
    value: T;

    public:
        func gt(&self, other: &value<T>) -> bool
        {
            return self.value > other.value;
        }
}

// 泛型特例化操作以及实现多个约束
struct value<int> impl Comparable<value<int>>, OtherTrait
{
    value: int;

    // 针对int类型的value进行具体化规则
    public:
        func gt(&self, other: &value<int>) -> bool
        {
            return self.value > other.value;
        }
}

// 泛型约束：只接受实现了 Comparable 特性的类型 T
func get_greater<T ? Comparable>(val1: &T, val2: &T) -> &T
{
    if (val1.gt(val2))
    {
        return val1;
    }
    else
    {
        return val2;
    }
}

func main(args: array[string]) -> result<void, string>
{
    let val1 = value{ value: 10 };
    get_greater(&val1, &value{ value: 5 });
}
```

### 约束函数
除了接入约束的形式，还有实现约束的形式  

```Chtholly
// 接入约束
struct Point<T> impl Comparable
{

}

// 实现约束
struct Point<T>
{
    impl Comparable
    public:
        gt(&self, other: &Point<T>) -> bool
        {

        }

    impl OtherTrait
    public:
        other_method(&self) -> void
        {

        }
}
```

### 模块与 `import`
Chtholly 支持模块系统，允许您将代码组织到多个文件中或使用标准库功能。`import` 关键字用于加载另一个模块中的代码并将其合并到当前作用域。

#### 语法
`import` 语句接受两种形式：
1.  **文件路径**: 一个字符串字面量，表示您想要包含的 Chtholly 文件（`.cns`）的路径。
2.  **标准库模块名**: 一个标识符，表示您想要导入的标准库模块。

```Chtholly
// 导入文件模块
import "path/to/your_module.cns";

// 导入标准库模块
import iostream;
```

为了获得最佳的跨平台兼容性，建议在文件路径中使用正斜杠 (`/`) 作为分隔符。

#### 行为
当您导入一个文件时，其所有的顶级定义（例如函数、结构体和 trait）在包含 `import` 语句的文件中都将变为可直接访问。这有助于代码重用和项目组织。

#### 示例
假设您有一个名为 `math.cns` 的文件：

```Chtholly
// math.cns
func add(a: int, b: int) -> int {
    return a + b;
}
```

您可以在另一个文件 `main.cns` 中通过导入 `math.cns` 来使用 `add` 函数：

```Chtholly
// main.cns
import "math.cns";

func main() {
    let result = add(5, 10);
    print(result); // 将输出 15
}
```

### iostream
Chtholly 的标准库 `iostream` 模块提供了基本的输入/输出功能。

#### print 函数
`print` 是一个内置函数，用于将一个值输出到控制台，并在末尾添加换行符。

```Chtholly
// print 是一个内置函数，无需导入即可使用
print("HelloWorld");
print(123);
```

尽管 `print` 是内置的，但 `import iostream;` 语句用于正式地将 `iostream` 模块的功能引入作用域。在未来的版本中，`iostream` 模块将包含更多功能。

#### input 函数
`input` 是一个内置函数，用于从控制台读取一行文本，并将其作为字符串返回。

```Chtholly
let name = input();
print("Hello, " + name);
```

### filesystem
Chtholly 的标准库 `filesystem` 模块提供了基本的文件系统功能。

#### fs_read 函数
`fs_read` 是一个内置函数，用于读取文件的全部内容，并将其作为字符串返回。

```Chtholly
let content = fs_read("my_file.txt");
print(content);
```

#### fs_write 函数
`fs_write` 是一个内置函数，用于将字符串内容写入文件。

```Chtholly
fs_write("my_file.txt", "Hello, Chtholly!");
```

#### fs_exists 函数
`fs_exists` 是一个内置函数，用于检查文件或目录是否存在。

```Chtholly
if (fs_exists("my_file.txt")) {
    print("File exists!");
}
```

#### fs_is_file 函数
`fs_is_file` 是一个内置函数，用于检查路径是否指向一个文件。

```Chtholly
if (fs_is_file("my_file.txt")) {
    print("It's a file!");
}
```

#### fs_is_dir 函数
`fs_is_dir` 是一个内置函数，用于检查路径是否指向一个目录。

```Chtholly
if (fs_is_dir("/my_directory")) {
    print("It's a directory!");
}
```

#### fs_list_dir 函数
`fs_list_dir` 是一个内置函数，用于获取目录中的所有条目，并以字符串数组的形式返回。

```Chtholly
let entries = fs_list_dir(".");
for (let entry : entries) {
    print(entry);
}
```

#### fs_remove 函数
`fs_remove` 是一个内置函数，用于删除文件或目录。

```Chtholly
fs_remove("my_file.txt");
```

### Math Module
Chtholly 的标准库 `math` 模块提供了常用的数学函数和常数。要使用这些功能，您必须首先导入 `math` 模块。

```Chtholly
import math;
```

#### 函数

- **`math::sqrt(x: double) -> double`**: 返回 `x` 的平方根。
- **`math::pow(base: double, exp: double) -> double`**: 返回 `base` 的 `exp` 次幂。
- **`math::sin(x: double) -> double`**: 返回 `x` 的正弦值。
- **`math::cos(x: double) -> double`**: 返回 `x` 的余弦值。
- **`math::tan(x: double) -> double`**: 返回 `x` 的正切值。
- **`math::log(x: double) -> double`**: 返回 `x` 的自然对数。
- **`math::log10(x: double) -> double`**: 返回 `x` 的以10为底的对数。
- **`math::abs(x: double) -> double`**: 返回 `x` 的绝对值。
- **`math::ceil(x: double) -> double`**: 返回不小于 `x` 的最小整数值。
- **`math::floor(x: double) -> double`**: 返回不大于 `x` 的最大整数值。
- **`math::round(x: double) -> double`**: 返回与 `x` 最接近的整数值。

#### 常数

- **`math::PI`**: 圆周率 π (约 3.14159)。
- **`math::E`**: 自然对数的底 e (约 2.71828)。

### String Methods
Chtholly 的 `string` 类型提供了一系列内置方法来执行常见的字符串操作。这些方法可以直接在任何字符串变量或字面量上调用，无需导入任何模块。

#### 方法

- **`length() -> int`**
  返回字符串的长度。

  ```Chtholly
  let s = "hello";
  let length = s.length(); // length 将是 5
  ```

- **`substr(start: int, count: int) -> string`**
  返回一个从索引 `start` 开始，长度为 `count` 的新子字符串。

  ```Chtholly
  let s = "hello world";
  let sub = s.substr(6, 5); // sub 将是 "world"
  ```

- **`find(sub: string) -> option<int>`**
  在字符串中查找子字符串 `sub`。如果找到，则返回一个包含起始索引的 `option<int>`；如果未找到，则返回 `none`。

  ```Chtholly
  let s = "hello world";
  let pos1 = s.find("world"); // pos1 将是 option{value: 6}
  let pos2 = s.find("galaxy"); // pos2 将是 none
  ```

- **`split(delimiter: string) -> array[string]`**
  使用 `delimiter` 将字符串分割成一个字符串数组。

  ```Chtholly
  let s = "hello,world,chtholly";
  let parts = s.split(","); // parts 将是 ["hello", "world", "chtholly"]
  ```

- **`to_upper() -> string`**
  将字符串中的所有字符转换为大写，并返回一个新字符串。

  ```Chtholly
  let s = "hello world";
  let upper = s.to_upper(); // upper 将是 "HELLO WORLD"
  ```

- **`to_lower() -> string`**
  将字符串中的所有字符转换为小写，并返回一个新字符串。

  ```Chtholly
  let s = "HELLO WORLD";
  let lower = s.to_lower(); // lower 将是 "hello world"
  ```

- **`trim() -> string`**
  移除字符串两端的空白字符，并返回一个新字符串。

  ```Chtholly
  let s = "  hello world  ";
  let trimmed = s.trim(); // trimmed 将是 "hello world"
  ```

- **`starts_with(prefix: string) -> bool`**
  检查字符串是否以 `prefix` 开头。

  ```Chtholly
  let s = "hello world";
  let starts = s.starts_with("hello"); // starts 将是 true
  ```

- **`ends_with(suffix: string) -> bool`**
  检查字符串是否以 `suffix` 结尾。

  ```Chtholly
  let s = "hello world";
  let ends = s.ends_with("world"); // ends 将是 true
  ```

- **`is_empty() -> bool`**
  检查字符串是否为空。

  ```Chtholly
  let s1 = "";
  let empty1 = s1.is_empty(); // empty1 将是 true
  let s2 = "hello";
  let empty2 = s2.is_empty(); // empty2 将是 false
  ```

- **`replace(from: string, to: string) -> string`**
  将字符串中所有出现的 `from` 子字符串替换为 `to`，并返回一个新字符串。

  ```Chtholly
  let s = "hello world, hello";
  let replaced = s.replace("hello", "hi"); // replaced 将是 "hi world, hi"
  ```

- **`contains(sub: string) -> bool`**
  检查字符串是否包含子字符串 `sub`。

  ```Chtholly
  let s = "hello world";
  let has_world = s.contains("world"); // has_world 将是 true
  let has_galaxy = s.contains("galaxy"); // has_galaxy 将是 false
  ```

### Array Methods
Chtholly 的 `array` 类型提供了一系列内置方法来操作动态数组。这些方法可以直接在任何数组变量上调用，无需导入任何模块。

#### 方法

- **`length() -> int`**
  返回数组中元素的数量。

  ```Chtholly
  let arr = [1, 2, 3];
  let len = arr.length(); // len 将是 3
  ```

- **`push(value: T)`**
  在数组的末尾添加一个元素 `value`。此方法要求数组是可变的。

  ```Chtholly
  mut arr = [1, 2];
  arr.push(3); // arr 现在是 [1, 2, 3]
  ```

- **`pop() -> T`**
  移除并返回数组的最后一个元素。此方法要求数组是可变的。

  ```Chtholly
  mut arr = [1, 2, 3];
  let last = arr.pop(); // last 将是 3, arr 现在是 [1, 2]
  ```

- **`contains(value: T) -> bool`**
  检查数组是否包含 `value`。

  ```Chtholly
  let arr = [1, 2, 3];
  let has_two = arr.contains(2); // has_two 将是 true
  ```

- **`reverse()`**
  反转数组中元素的顺序。此方法要求数组是可变的。

  ```Chtholly
  mut arr = [1, 2, 3];
  arr.reverse(); // arr 现在是 [3, 2, 1]
  ```

- **`is_empty() -> bool`**
  检查数组是否为空。

  ```Chtholly
  let arr1 = [];
  let empty1 = arr1.is_empty(); // empty1 将是 true
  let arr2 = [1];
  let empty2 = arr2.is_empty(); // empty2 将是 false
  ```

- **`clear()`**
  清空数组，移除所有元素。此方法要求数组是可变的。

  ```Chtholly
  mut arr = [1, 2, 3];
  arr.clear(); // arr 现在是 []
  ```

- **`sort()`**
  对数组进行原地排序。此方法要求数组是可变的。

  ```Chtholly
  mut arr = [3, 1, 4, 1, 5];
  arr.sort(); // arr 现在是 [1, 1, 3, 4, 5]
  ```
- **`join(separator: string) -> string`**
  使用 `separator` 将一个字符串数组连接成单个字符串。

  ```Chtholly
  let arr = ["hello", "world", "chtholly"];
  let s = arr.join(","); // s 将是 "hello,world,chtholly"
  ```

### OS Module
Chtholly 的标准库 `os` 模块提供了与操作系统交互的功能，例如控制进程或访问环境变量。要使用这些功能，您必须首先导入 `os` 模块。

```Chtholly
import os;
```

#### 函数

- **`os::exit(code: int)`**
  以给定的状态码 `code` 终止程序。这对于在程序的特定点上指示成功（通常为 `0`）或失败（非零值）非常有用。

  ```Chtholly
  import os;

  func main() -> int {
      // ... do some work
      os::exit(0); // 成功退出
  }
  ```

- **`os::env(name: string) -> option<string>`**
  获取名为 `name` 的环境变量的值。如果环境变量存在，则返回一个包含其值的 `option<string>`；如果不存在，则返回 `none`。

  ```Chtholly
  import os;
  import iostream;

  func main() -> int {
      let shell = os::env("SHELL");
      print(shell.unwarp_or("SHELL not found"));
      return 0;
  }
  ```

### Random Module
Chtholly 的标准库 `random` 模块提供了生成随机数的功能。要使用这些函数，您必须首先导入 `random` 模块。

```Chtholly
import random;
```

#### 函数

- **`random::rand() -> double`**
  返回一个介于 0.0 (含) 和 1.0 (不含) 之间的随机浮点数。

  ```Chtholly
  let r = random::rand(); // r 可能是 0.12345
  ```

- **`random::randint(min: int, max: int) -> int`**
  返回一个在 [`min`, `max`] (含) 范围内的随机整数。

  ```Chtholly
  let num = random::randint(1, 100); // num 将是 1 到 100 之间的一个随机整数
  ```

### Time Module
Chtholly 的标准库 `time` 模块提供了与时间相关的功能。要使用这些函数，您必须首先导入 `time` 模块。

```Chtholly
import time;
```

#### 函数

- **`time::now() -> int`**
  返回当前的 Unix 时间戳，即自 1970 年 1 月 1 日午夜（UTC）以来经过的秒数。

  ```Chtholly
  let timestamp = time::now();
  print(timestamp);
  ```

### operator
#### 操作符自定义
Chtholly支持操作符自定义，此功能由模块operator提供  
此为自举前实现的功能  

```Chtholly
import operator;

struct Point impl operator::add  // +
, operator::sub  // -
, operator::mul  // *
, operator::div  // /
, operator::mod  // %
, operator::prefix_add  // ++
, operator::prefix_sub  // --
, operator::postfix_add  // ++
, operator::postfix_add  // --
, operator::assign_add  // +=
, operator::assign_sub  // -=
, operator::assign_mul  // *=
, operator::assign_div  // /=
, operator::assign_mod  // %=
, operator::assign  // ==
, operator::not_equal  // !=
, operator::less  // <
, operator::less_equal  // <=
, operator::greater  // >
, operator::greater_equal  // >=
, operator::and  // &&
, operator::or  // ||
, operator::not  // !
, operator::binary  // 自定义二元操作符，暂时不考虑三元
// 更多待补充...
{
    // 这里只演示自定义操作符的使用
    public:
        binary(self, operator: string, other: Point)
        {
            if(operator == "**")
                return pow(self.x, other.x);  // 举个例子
        }
}

在实现操作符重载函数时，除了第一个 `self` 参数外，第二个参数（通常命名为 `other`）的类型非常灵活。它可以是值类型 (`T`)、不可变引用 (`&T`) 或可变引用 (`&mut T`)，只要能满足您的逻辑需求即可。这同样适用于 `binary` 函数的 `other` 参数。
```

## 类型转换
Chtholly支持类型转换，使用type_cast<T>()进行转换  
此函数为内置函数，不需要导入  

```chtholly
let a: int8 = type_cast<int8>(10.5);
```

### 静态反射
Chtholly支持静态反射，由模块`reflect`提供静态反射功能。

`reflect`模块提供了一系列函数，用于在编译期获取类型信息。它还定义了几个结构体，`Field`，`Method`和`Trait`，用于封装反射数据。

#### 函数
- `reflect.get_field_count(T)`: 获取结构体 `T` 的字段数量。
- `reflect.get_fields(T)`: 获取一个包含 `Field` 结构体的数组，每个结构体代表 `T` 的一个字段。
- `reflect.get_field(T, "field_name")`: 获取一个代表 `T` 特定字段的 `Field` 结构体。
- `reflect.get_method_count(T)`: 获取结构体 `T` 的方法数量。
- `reflect.get_methods(T)`: 获取一个包含 `Method` 结构体的数组，每个结构体代表 `T` 的一个方法。
- `reflect.get_method(T, "method_name")`: 获取一个代表 `T` 特定方法的 `Method` 结构体。
- `reflect.get_trait_count(T)`: 获取结构体 `T` 实现的特性数量。
- `reflect.get_traits(T)`: 获取一个包含 `Trait` 结构体的数组，每个结构体代表 `T` 实现的一个特性。
- `reflect.get_trait(T, "trait_name")`: 获取一个代表 `T` 特定实现的 `Trait` 结构体。

#### 结构体

##### `Field`
- `name`: 字段名 (string)
- `type`: 字段类型名 (string)

##### `Method`
- `name`: 方法名 (string)
- `return_type`: 返回类型名 (string)
- `parameters`: 一个包含 `Parameter` 结构体的数组

##### `Parameter`
- `name`: 参数名 (string)
- `type`: 参数类型名 (string)

##### `Trait`
- `name`: 特性名 (string)

### 元编程
元编程由meta模块提供  

#### 类型诊断
你可以使用一系列的meta::is_type函数进行类型诊断  
这不会从精确的角度进行类型诊断，而是从最宽泛的角度进行类型诊断    

- meta::is_int(T) 判断T是否为int类型  
- meta::is_uint(T) 判断T是否为uint类型  
- meta::is_double(T) 判断T是否为double类型  
- meta::is_char(T) 判断T是否为char类型  
- meta::is_bool(T) 判断T是否为bool类型  
- meta::is_string(T) 判断T是否为string类型  
- meta::is_array(T) 判断T是否为array类型  
- meta::is_struct(T) 判断T是否为struct类型
- meta::type_name(T) 返回T类型的名称字符串

#### 类型修饰
- meta::is_let(T)  判断T是否为let类型  
- meta::is_mut(T)  判断T是否为mut类型
- meta::is_borrow(T) 判断T是否是借用，而不是移动  
- meta::is_borrow_mut(T) 判断T是否为可变借用  
- meta::is_move(T) 判断T是否为移动  

...元编程待补充  

#### util
util模块是工具模块，主要用于提供一些工具函数  
将分为特征与函数  

**特征**  
- util::to_string(T) 将T转换为string    

**函数**
- util::string_cast(T) 将T转换为string，通常会调用自定义类型的to_string方法  
- util::panic(message: string) 打印错误信息并立即终止程序

```Chtholly
import util;

func print_any<T ? util::to_string>(T value) -> void
{
    print(util::string_cast(value));
}
```

### CLI
使用阻塞式CLI

## 自举
Chtholly的定位是MIT开源的社区项目，不应该进行自举，这会增加维护的难度，而是尽可能维护C++版本的编译器

## 未来展望
Chtholly 语言仍在积极开发中，以下是未来计划的一些关键方向：

- **完善语义分析 (Resolver)**: 当前的 `Resolver` 已经实现了全面的类型检查。下一步是构建一个完整的符号表，以支持更复杂的名称解析，例如模块作用域和改进的错误诊断。

- **重构类型系统**: 将编译器内部的类型表示从 `std::string` 和 `TypeInfo` 结构体迁移到一个更健壮的、基于类的层次结构（例如，`Type`, `IntType`, `StructType`）。这将使类型比较和推理更加安全和高效。

- **增强模块系统**:
  - 引入 `public` 和 `private` 关键字来控制模块成员的可见性。
  - 实现模块别名 (`import my_module as mm;`)。

- **并发支持**: 探索并添加对并发编程的支持，可能通过 actor 模型或轻量级线程（协程）实现。

- **改进标准库**:
  - 添加更多的数据结构（如 `HashMap`, `Set`）。
  - 提供网络和 I/O 相关的库。

- **包管理器**: 开发一个简单的包管理器，用于获取、构建和管理 Chtholly 项目的依赖项。
