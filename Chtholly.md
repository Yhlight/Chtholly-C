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

```Chtholly
let a = 20;
let b = &a;

mut a2 = 30;
let b2 = &a2;
mut c2 = &mut a2;
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

let b = option(20);  // 创建option对象
let c = option<int>(20);  // option本质是一个泛型结构体，由于自动推断的存在，因此可以省略泛型
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

#### lambda函数
Chtholly的lambda函数使用与C++完全一致的语法  
默认情况下，捕获属于不可变引用  

```Chtholly
let add = [](a: int, b: int) -> int { return a + b; };
```

#### 参数所有权
和Rust很类似

```Chtholly
func test(x: string, x2: &string, x3: &mut string) -> int
{
    // x: string，表示接管所有权
    // x2: &string，表示不可变引用
    // x3: mut& string，表示可变引用，对于mut的变量
}
```

### 明确数组长度
你可以在类型限定中添加额外的参数项，表示数组的大小

```Chthollt
let arr2 : array[string ; 5];  // 指定类型并指定长度
```

### struct结构体
在Chtholly之中，使用struct创建结构体
不支持继承语法，可以使用组合式继承，具有public，private两种权限，默认公开

```Chtholly
struct Test
{
    private name: string,  // 可以赋予默认值
    private id: int,

    public add(x, y) -> int  // 结构体内的函数不需要写func
    {
        return x + y;
    }
}

func main(args: array[string])
{
    let test = Test();  // 第一种创建方式
    let test2 = Test{  // 第二种
        name: "xxx",
        id: 18
    }

    print(test2.name);
}
```

#### self关键字与对象关联
在Chtholly中，可以使用self表示自引用
共有三种权限
self  所有权
&self  只读
&mut self  可写

通常情况下，带有self参数的函数，需要明确的对象调用
而对于那些没有self参数的函数，则不需要明确的对象调用，类似其他语言的静态函数
使用结构体::函数名称进行调用

```Chtholly
struct Test
{
    private name: string,
    private age: int,

    public test(self)
    {
        return self.name;
    }

    public test2(&mut self)
    {
        self.name = "HelloWolrd";
        return self.name;
    }

    public test3()  // 参数没有self，与本身无关系
    {

    }
}

func main(args: array[string])
{
    let test = Test();
    test.test();

    Test::test3();
}
```

### 运算符
和C++一致

### 命名规则
与C++一致

### 选择结构和循环结构
与C++一致，不同的是switch的case允许使用任意类型的变量作为判断依据，也能使用表达式

```Chtholly
switch(任意类型的变量 / 表达式)
{
    case 值1: {  // C语言缺陷，现代化编程语言最好强制要求{ }
        break;  // break现在不是防止穿透，而是跳出匹配
    }
    case 表达式: {
        break;
    }
    case 表达式2: {
        fallthrough;  // 如果需要穿透，请使用fallthrough
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

func main(args: array[string]) -> Result<void, string>
{
    mut num1 = 100;
    mut num2 = 200;
    swap_values(&mut num1, &mut num2); // 编译器自动推断 T 为 int
    // num1 现在是 200, num2 现在是 100

    mut s1 = "Alpha";
    mut s2 = "Beta";
    swap_values<string>(&mut s1, &mut s2); // 显式指定 T 为 string
    // s1 现在是 "Beta", s2 现在是 "Alpha"

    return Result::pass();
}
```

#### 泛型结构体
```Chtholly
// 定义一个泛型结构体 Point，它接受一个类型参数 T
struct Point<T>  // 这个T可以写默认值，使用<T = int>即可指定默认值，同理，也具有特例化操作，例如<int>
{
    x: T,
    y: T,

    // 结构体内的函数也可以使用泛型 T
    public swap(&mut self)
    {
        // 交换 x 和 y 的值
        let temp = self.x;
        self.x = self.y;
        self.y = temp;
    }
}

func main(args: array[string]) -> Result<void, string>
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

    return Result::pass();
}
```

#### 结构体内的泛型函数
在Chtholly之中，结构体，无论自身是否是泛型，都可以拥有泛型成员函数。

##### 常规结构体内的泛型函数
```Chtholly
// 一个常规的、非泛型的结构体
struct Printer {
    // 拥有一个泛型方法，可以打印任何类型的值
    func print<T>(self, value: T) {
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
  // 方法的泛型参数 K, F 与结构体的泛型参数 T 是独立的
  func test<K, F>(self) // 需要对象调用
  {}

  func test2<K, F>() // 不需要对象调用 (静态方法)
  {}
  // 和前面是一样的，默认类型，类型特例化，都得到支持
}

func main(args: array[string]) -> Result<void, string>
{
    let t: Point<string> = Point{};
    t.test<int, bool>();
    Point::test2<char, string>();
    return Result::pass();
}
```

### 约束
Chtholly使用`?`符号来为泛型参数指定约束。

```Chtholly
// 假设 Comparable 特性已定义，它要求实现一个 gt(大于) 方法
trait Comparable
{
    // 也支持泛型函数
    func gt(&self, other: &self) -> bool;
}

// 定义一个泛型结构体 value，并实现 Comparable 约束
struct value<T> impl Comparable
{
    value: T,

    func gt(&self, other: &self) -> bool
    {
        return self.value > other.value;
    }
}

// 特例化操作，可以实现多个约束
struct value<int> impl Comparable, OtherTrait
{
    value: int,

    // 针对int类型的value进行具体化规则
    func gt(&self, other: &self) -> bool
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

func main(args: array[string]) -> Result<void, string>
{
    let val1 = value{ value: 10 };
    get_greater(&val1, &value{ value: 5 });
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
    public binary(self, operator: string, other: Point)
    {
        if(operator == "**")
            return pow(self.x, other.x);  // 举个例子
    }
}
```

## 类型转换
Chtholly支持类型转换，使用type_cast<T>()进行转换  
此函数为内置函数，不需要导入  

```chtholly
let a: int8 = type_cast<int8>(10.5);
```

### 静态反射
Chtholly支持静态反射，由模块reflect提供静态反射  

#### field
你可以通过reflect::field类获取结构体的字段  

##### 常用方法
- get_name() 获取字段名  
- get_type() 获取字段类型  
- get_value() 获取字段值  
- set_value() 设置字段值  
- get_fields() 获取结构体的所有字段  
- get_field() 获取结构体的指定字段  
- get_field_value() 获取结构体的指定字段的值  
- set_field_value() 设置结构体的指定字段的值  
- get_field_type() 获取结构体的指定字段的类型  
- get_field_index() 获取结构体的指定字段的索引  
- get_field_by_index() 获取结构体的指定索引的字段  
- get_field_count() 获取结构体的字段数量  
...更多方法待补充  

#### method
你可以通过reflect::method类获取结构体的方法  

##### 常用方法
- get_name() 获取方法名  
- get_return_type() 获取方法返回值类型  
- get_parameters() 获取方法参数  
- get_parameter_count() 获取方法参数数量  
- get_methods() 获取结构体的所有方法  
- get_method() 获取结构体的指定方法  
...更多方法待补充  

#### tarit
你可以通过reflect::trait类获取结构体的特性  

##### 常用方法
- get_name() 获取特性名  
- get_traits() 获取结构体的所有特性
- get_trait() 获取结构体的指定特性
- get_trait_count() 获取结构体的特性数量
- get_return_type() 获取特征返回值类型  
- get_parameters() 获取特征参数  
- get_parameter_count() 获取特征参数数量  
...更多方法待补充  

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
- util::serialize(T) 将T序列化  
- util::deserialize(T) 将T反序列化  
- util::unique_id() 获取一个唯一的id  

### CLI
使用阻塞式CLI

## 自举
如果能够完成上述所有的内容，那么Chtholly已经是一门完整的编程语言了
这时候就要考虑通过自举实现自己的编译器，然后逐渐扩展更多的语法特征
