## Chtholly
Chtholly是一门基于C++实现的编程语言，Chtholly以简洁，高性能，接管所有权，编译期为特征，出于个人爱好而非实现更好的编程语言
Chtholly很大程度上向Rust靠拢，但是Chtholly不是照搬Rust，Chtholly具有很多独特的语法    
也就是说，Chtholly遵循零成本抽象以及运行时极简原则，尽可能把事情交给编译期进行

Chtholly文件后缀为.cns

注意！Chtholly不是一门正规的编程语言，仅供个人学习使用，请不要用于生产环境

### 接管所有权
像Rust一样，Chtholly使用复杂的所有权管理系统接管所有权的管理
全面拒绝隐式传递

对于基本数据类型，采用copy
对于引用数据类型，采用move

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

#### self关键字与对象关联
在Chtholly中，可以使用self表示对实例自身的引用。
共有三种形式：
- `self`: 表示获取实例的所有权 (move)。
- `&self`: 表示对实例的不可变引用 (borrow)。
- `&mut self`: 表示对实例的可变引用 (mutable borrow)。

通常情况下，带有self参数的函数，需要由明确的对象实例来调用。
而对于那些没有self参数的函数，则不需要明确的对象实例调用，类似其他语言的静态函数，使用 `结构体::函数名称` 进行调用。

**在方法内部，访问实例自身的字段或调用其他实例方法时，必须显式使用 `self.` 前缀，不允许隐式调用。**

```Chtholly
struct Test
{
    private:
        name: string;
        age: int;

    public:
        test(self)
        {
            return self.name; // 必须使用 self.
        }

        test2(&mut self)
        {
            self.name = "HelloWolrd"; // 必须使用 self.
        }

        // 参数没有self，与本身无关系
        test3()
        {

        }
}

func main(args: array[string])
{
    let test = Test{};
    test.test();

    Test::test3();
}
```

### 运算符
和C++一致

### 命名规则
与C++一致

### 选择结构和循环结构
与C++一致。
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

### Filesystem Module
Chtholly 的标准库 `filesystem` 模块提供了与文件系统交互的功能。要使用这些函数，您必须首先导入 `filesystem` 模块。

```Chtholly
import filesystem;
```

#### 函数

- **`filesystem::read(path: string) -> string`**
  读取文件 `path` 的全部内容，并将其作为字符串返回。

  ```Chtholly
  let content = filesystem::read("my_file.txt");
  print(content);
  ```

- **`filesystem::write(path: string, content: string)`**
  将字符串 `content` 写入到文件 `path` 中。如果文件不存在，则会创建它；如果文件已存在，则会覆盖其内容。

  ```Chtholly
  filesystem::write("my_file.txt", "Hello, Chtholly!");
  ```

- **`filesystem::list_dir(path: string) -> array[string]`**
  返回一个字符串数组，其中包含目录 `path` 下所有条目（文件和子目录）的名称。

  ```Chtholly
  let entries = filesystem::list_dir(".");
  for entry in entries {
      print(entry);
  }
  ```

- **`filesystem::is_file(path: string) -> bool`**
  检查路径 `path` 是否指向一个文件。

  ```Chtholly
  let is_f = filesystem::is_file("my_file.txt"); // true
  ```

- **`filesystem::is_dir(path: string) -> bool`**
  检查路径 `path` 是否指向一个目录。

  ```Chtholly
  let is_d = filesystem::is_dir("."); // true
  ```

- **`filesystem::remove(path: string)`**
  删除文件或空目录 `path`。

  ```Chtholly
  filesystem::remove("my_file.txt");
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

### String Module
Chtholly 的标准库 `string` 模块提供了一系列用于操作字符串的函数。要使用这些函数，您必须首先导入 `string` 模块。

```Chtholly
import string;
```

#### 函数

- **`string::length(s: string) -> int`**
  返回字符串 `s` 的长度。

  ```Chtholly
  let s = "hello";
  let length = string::length(s); // length 将是 5
  ```

- **`string::substr(s: string, start: int, count: int) -> string`**
  返回一个从索引 `start` 开始，长度为 `count` 的新子字符串。

  ```Chtholly
  let s = "hello world";
  let sub = string::substr(s, 6, 5); // sub 将是 "world"
  ```

- **`string::find(s: string, sub: string) -> option<int>`**
  在字符串 `s` 中查找子字符串 `sub`。如果找到，则返回一个包含起始索引的 `option<int>`；如果未找到，则返回 `none`。

  ```Chtholly
  let s = "hello world";
  let pos1 = string::find(s, "world"); // pos1 将是 option{value: 6}
  let pos2 = string::find(s, "galaxy"); // pos2 将是 none
  ```

- **`string::split(s: string, delimiter: string) -> array[string]`**
  使用 `delimiter` 将字符串 `s` 分割成一个字符串数组。

  ```Chtholly
  let s = "hello,world,chtholly";
  let parts = string::split(s, ","); // parts 将是 ["hello", "world", "chtholly"]
  ```

- **`string::join(arr: array[string], separator: string) -> string`**
  使用 `separator` 将一个字符串数组 `arr` 连接成单个字符串。

  ```Chtholly
  let arr = ["hello", "world", "chtholly"];
  let s = string::join(arr, ","); // s 将是 "hello,world,chtholly"
  ```

### Array Module
Chtholly 的标准库 `array` 模块提供了一系列用于操作动态数组的函数。要使用这些函数，您必须首先导入 `array` 模块。

```Chtholly
import array;
```

#### 函数

- **`array::length(arr: &array[T]) -> int`**
  返回数组 `arr` 中元素的数量。

  ```Chtholly
  let arr = [1, 2, 3];
  let len = array::length(arr); // len 将是 3
  ```

- **`array::push(arr: &mut array[T], value: T)`**
  在数组 `arr` 的末尾添加一个元素 `value`。该函数要求数组是可变的。

  ```Chtholly
  mut arr = [1, 2];
  array::push(arr, 3); // arr 现在是 [1, 2, 3]
  ```

- **`array::pop(arr: &mut array[T]) -> T`**
  移除并返回数组 `arr` 的最后一个元素。该函数要求数组是可变的。

  ```Chtholly
  mut arr = [1, 2, 3];
  let last = array::pop(arr); // last 将是 3, arr 现在是 [1, 2]
  ```

- **`array::contains(arr: &array[T], value: T) -> bool`**
  检查数组 `arr` 是否包含 `value`。

  ```Chtholly
  let arr = [1, 2, 3];
  let has_two = array::contains(arr, 2); // has_two 将是 true
  ```

- **`array::reverse(arr: &mut array[T])`**
  反转数组 `arr` 中元素的顺序。该函数要求数组是可变的。

  ```Chtholly
  mut arr = [1, 2, 3];
  array::reverse(arr); // arr 现在是 [3, 2, 1]
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

### Time Module
Chtholly 的标准库 `time` 模块提供了与时间相关的功能。要使用这些函数，您必须首先导入 `time` 模块。

```Chtholly
import time;
```

#### 函数

- **`time::now() -> int`**
  返回当前的 Unix 时间戳（自 1970-01-01 00:00:00 UTC 以来的秒数）。

  ```Chtholly
  import time;
  import iostream;

  func main() -> int {
      let current_time = time::now();
      print(current_time);
      return 0;
  }
  ```

### Random Module
Chtholly 的标准库 `random` 模块提供了生成伪随机数的功能。要使用这些函数，您必须首先导入 `random` 模块。

```Chtholly
import random;
```

#### 函数

- **`random::rand() -> double`**
  返回 `[0.0, 1.0)` 范围内的下一个伪随机浮点数。

  ```Chtholly
  import random;
  import iostream;

  func main() -> int {
      let r = random::rand();
      print(r);
      return 0;
  }
  ```

- **`random::randint(min: int, max: int) -> int`**
  返回 `[min, max]` 范围内的下一个伪随机整数。

  ```Chtholly
  import random;
  import iostream;

  func main() -> int {
      let r = random::randint(1, 100);
      print(r);
      return 0;
  }
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
, operator::bit_and // &
, operator::bit_or // |
, operator::bit_xor // ^
, operator::bit_not // ~
, operator::shl // <<
, operator::shr // >>
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
