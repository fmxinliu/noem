# C++/CLI 语法要点

## 一、类型

### 常用基本类型

|     基本类型      |     CLI值类型      |
| :-----------: | :-------------: |
|     bool      | System::Boolean |
|     char      |  System::SByte  |
|    wchar_t    |  System::Char   |
| unsigned char |  System::Byte   |
|     short     |  System::Int16  |
|  int / long   |  System::Int32  |
|     float     | System::Single  |
|    double     | System::Double  |

### 枚举

```c#
enum class Suit {Clubs, Diamonds, Hearts, Spades};
Suit suit = Suit::Clubs;
int value = safe_cast<int>(suit);
Console::WriteLine(L"Suit is {0} and the value is {1}", suit, value);
---
> Suit is Clubs and the value is 0
```
### 字符串类型

```c#
String^ s1 = "你好123"; // 多字节字符，编译器将确保将其转换成宽字符
String^ s2 = L"你好123"; // Unicode字符
int len1 = s1->Length; // 5
int len2 = s2->Length; // 5
```

### 数组类型

```c#
array<int>^ data = gcnew array<int>(100);
array<String^>^ strings = {
    L"Land ahoy!",
    L"Splice the mainbrace!",
    L"Shiver me timbers!",
    L"Never throw into the wind!"
};
```

### 强制类型转换

在 `CLR` 环境中，可以使用`safe_cast`将一种类型转换为另一种类型，转换失败会抛出异常。

```c#
double value1 = 10.5;
double value2 = 15.5;
int whole_number = safe_cast<int>(value1) + safe_cast<int>(value2);
```
### for each循环
```c#
int count = 0;
String^ proverb = L"A nod is as good as a wink to a blind horse.";
for each(wchar_t ch in proverb) { // Unicode 字符
    if (Char::IsLetter(ch)) {
        count++;
    }
}
Console::WriteLine(L"The proverb contains {0} letters.", count);
---
> The proverb contains 32 letters.
```
## 二、句柄

### 跟踪句柄（类似于C++指针）

存放**托管堆上对象的地址**。当被跟踪对象的地址变化后，由`CLR`垃圾回收器自动更新**（会变化）**。因此，不允许对跟踪句柄进行地址的算术运算和强制类型转换。

```c#
//int *p = 123; // 报错!
int^ q = 123; // 等价于 int^ q = gcnew int(123);
//int a = *p + 2;
int b = *q + 2; // 解引用

String^ proverb;
proverb = nullptr;
proverb = L"hello world.";

// 数组句柄
array<int>^ value = {3, 5, 6, 8, 6};
array<int>^ data = gcnew array<int>(100); // 长度 100
int length = data->Length;
```
### 跟踪引用（类似于C++引用）

表示某对象**（可以是栈对象、托管堆对象）**的别名，**跟踪引用本身总是在栈上创建的**。如果垃圾回收移动了被引用的对象，跟踪引用会被**自动更新**。

```c#
// 引用栈上值对象
int value = 10;
int% trackValue = value;
value = 666;
Cons

// 引用托管堆上引用对象
int^ pValue = 123;
int% trackValue2 = *pValue;
*pValue = 666;

Console::WriteLine("trackValue = {0}, trackValue2 = {1}", trackValue, trackValue2);
---
> trackValue = 666, trackValue2 = 666
```
### 内部指针

内部指针中存储的地址，由CLR垃圾回收自动更新。隐藏，允许进行地址的算术操作。
```c#
//interior_ptr<String^> pstr1;// OK -- pointer to a handle
//interior_ptr<String>  pstr2;// ERROR -- pointer to a String object

array<double>^ data = {1.5, 3.5, 6.7, 4.2, 2.1};
interior_ptr<double> pstart = &data[0];
interior_ptr<double> pend = &data[data->Length - 1];
double sum = 0;

while(pstart<=pend)
    sum += *pstart++;

Console::WriteLine(L"Total of data array elements = {0}\n", sum);

array<String^>^ strings = {
    L"Land ahoy!",
    L"Splice the mainbrace!",
    L"Shiver me timbers!",
    L"Never throw into the wind!"
};

for(interior_ptr<String^> pstrings = &strings[0]; pstrings-&strings[0] < strings->Length; ++pstrings)
    Console::WriteLine(*pstrings);
```
## 三、函数

### main函数
```c#
// 命令行参数中不包括程序名称本身
int main(array<System::String ^> ^args) {
    Console::WriteLine(L"There were {0} command line arguments.", args->Length);
    Console::WriteLine(L"Command line arguments received are:");
    int i = 1;
    for each(String^ str in args)
        Console::WriteLine(L"Argument {0}: {1}", i++, str);
    return 0;
}
```

### 可变长度参数的函数

```c#
double sum(...array<double>^ args) {
    double sum = 0.0;
    for each(double arg in args)
        sum += arg;
    return sum;
}

Console::WriteLine( sum(2.0, 4.0, 6.0, 8.0, 10.0, 12.0) );
Console::WriteLine( sum(1.1, 2.2, 3.3, 4.4, 5.5, 6.6, 7.7, 8.8, 9.9) );
```

### 类函数

类似于函数模板，但原理上却迥然不同。

- 使用函数模板时，编译器根据模板生成函数源代码，然后将其与其它代码一起编译。这种方法可能会产生许多函数，从而使得生成模块的体积增加，这就是所谓的“代码膨胀”。

- 类函数与之不同，类函数本身将被编译，在调用类函数时，实际类型在运行时取代了类函数的类型形参，这不会导致新增代码的问题。

```c#
generic<typename T> where T : IComparable
T MaxElement(array<T>^ x) {
    T max = x[0];
    for(int i=1; i < x->Length; i++)
        if (max->CompareTo(x[i]) < 0)
            max = x[i];
    return max;
}

array<int>^ data1 = {1, 2, 3, 4, 5};
int maxData1 = MaxElement<int>(data1);

array<double>^ data2 = {1.5, 3.5, 6.7, 4.2, 2.1};
double maxData2 = MaxElement(data2);
```

## 四、类

### 1. 类别

#### 数值类（struct）

```c#
value class Height {
    private: // 不指定，默认 public
        int feet;
        int inches;

        // 常量定义
        literal int inchesPerFoot1 = 12; // 字面量，类似于C# const
        initonly int inchesPerFoot2; // 类似C# readonly
        initonly static int inchesPerFoot3_1 = 12; // 静态，定义时初始化
        initonly static int inchesPerFoot3_2; // 静态，在静态构造函数中初始化

        // 静态构造函数
        static Height() {
            inchesPerFoot3_2 = 12;
        }

    public:
        Height(int ins) : inchesPerFoot2(12) {
            feet = ins / inchesPerFoot1;
            inches = ins % inchesPerFoot1;
        }

        Height(int ft, int ins) : feet(ft), inches(ins), inchesPerFoot2(12) {}

        // 函数重写
        virtual String^ ToString() override {
            return feet + L" feet " + inches + L" inches";
        }
        // 不能重写默认构造函数（值类型数据成员初始化为0，引用类型初始化为nullptr）
        // 也不能重载拷贝构造函数、赋值操作符（依次复制每一个数据成员的值）
    };

Height^ pheight = Height(70); // 引用
Height height1 = Height(6, 3); // 值
Height height2 = height1; // height1、height2 都有自己独立的数据，仅复制数据
Height height3 = *pheight; // height3 被分配在栈上，值类型，有自己独立的数据，仅复制数据
String^ s0 = pheight.ToString();
String^ s1 = height1->ToString();
String^ s2 = height2.ToString();
String^ s3 = height3.ToString();
```

#### 引用类（class）

```c#
ref class Box {
public:
    Box():Length(1.0), Width(1.0), Height(1.0) {
        Console::WriteLine(L"No-arg constructot called.");
    }

    Box(double lv, double bv, double hv) : Length(lv), Width(bv), Height(hv) {
        Console::WriteLine(L"Constructor called.");
    }

    double Volume() {
        return Length * Width * Height;
    }

    // 没有默认构造函数和赋值运算符，如果需要，必须显式添加

private:
    double Length;
    double Width;
    double Height;
};

Box^ aBox;
Box^ newBox = gcnew Box(10, 15, 20);
aBox = gcnew Box;

Console::WriteLine(L"Default box volume is {0}", aBox->Volume());
Console::WriteLine(L"New box volume is {0}", newBox->Volume());
```
### 2. 属性

#### 标量属性（单值）

```c#
--- point.h
value class Point {
public:
    // 1.在 .h 中定义
    property int X {
        void set(int value) { // 可写
            this->x = value;
        }
        int get() {           // 可读
            return this->x;
        }
    }

    // 2.在 .cpp 中定义
    property int Y {
        void set(int x);
        int get();
    }

    // 3.平凡标量属性，由编译器提供set、get
    property int Z;

private:
    int x;
    int y;
};

--- point.cpp
int Point::Y::get() { // 添加“属性限定名”
    return this->y;
}

void Point::Y::set(int value) {
    this->y = value;
}
```

#### 索引属性（一组属性值）

```c#
ref class Name {
private:
    array<String ^>^ Names;

public:
    Name(...array<String ^>^ names) : Names(names) {}

    property int NameCount {
        int get() { return Names->Length; }
    }

    // 无名属性
    property String^ default[int] {
        String^ get(int index) {
            if (index < 0)
                throw gcnew Exception(L"Index < 0");
            if (index >= Names->Length)
                throw gcnew Exception(L"Index out of range");
            return Names[index];
        }
    }

    // 有名属性
    property bool Contains[int, String ^] { // 索引可以不是整型，可以包含多个
        bool get(int index, String ^subName) {
            return Names[index]->Contains(subName);
        }
    }

    static void foreachTest() {
        Name^ myName = gcnew Name(L"Ebenezer", L"Isaiah", L"Ezra", L"Inigo");
        for(int i = 0; i < myName->NameCount; i++)
            Console::WriteLine(L"Name {0} is {1}", i+1, myName[i]);
        bool isContiains;
        isContiains = myName->Contains[0, L"xyz"];
        isContiains = myName->Contains[0, L"nez"];
    }
};
```

#### 静态属性（类似静态成员变量）

```c#
value class Length {
public:
    static property String ^ Units {
        String ^ get() { return L"feet and inches"; }
    }
};
```

### 3. 运算符重载

#### 数值类中重载运算符

```c#
value class Complex {
private:
    int rel;
    int im;

public:
    Complex(int a, int b) : rel(a), im(b) {}

    virtual String^ ToString() override {
        return rel + " + " + im + "i";
    }

    // 1.成员函数实现方式
    //Complex operator+(Complex obj) {
    //    return Complex(rel + obj.rel, im + obj.im);
    //}

    // 2.静态成员函数实现方式
    static Complex operator+(Complex obj1, Complex obj2) {
        return Complex(obj1.rel + obj2.rel, obj1.im + obj2.im);
    }

    // 二元数乘运算符
    static Complex operator*(int x, Complex obj) {
        return Complex(x * obj.rel, x * obj.im);
    }
    static Complex operator*(Complex obj, int x) {
        return Complex(x * obj.rel, x * obj.im);
    }

    // 一元递增运算符（前缀、后缀由编译器自动识别）
    static Complex operator++(Complex obj) {
        ++obj.rel;
        ++obj.im;
        return obj;
    }
};
```

#### 引用类中重载运算符
- 与数值类中重载运算符基本相同，主要区别是形参和返回值一般都是句柄。
