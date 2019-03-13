#include <vector>
#include <iostream>


class String {
public:
    // constructor
    String() : data(new char[1]) {
        *data = '\0';
    }
    String(const char *str) : data(new char[strlen(str)+1]) {
        strcpy(data, str);
    }
    /*
    String(const String &rhs) : data(new char[rhs.size()+1]) {
        strcpy(data, rhs.c_str());
    }*/
    // delegate
    String(const String &rhs) : String(rhs.data) {}

    String& operator=(const String &rhs) {
        String temp(rhs);
        swap(temp);
        return *this;
    }

    // 移动构造
    String(String&& rhs) : data(rhs.data) {
        rhs.data = nullptr;
    }
    String &operator=(String&& rhs) {
        swap(rhs);
        return *this;
    }

    ~String() {
        delete[] data;
    }

    friend std::ostream& operator<<(std::ostream &os, const String &s);
    size_t size() const {
        return strlen(data);
    }
    const char* c_str() const {
        return data;
    }
    void swap(String& rhs) {
        std::swap(data, rhs.data);
    }

private:
    char *data;
};

std::ostream& operator<<(std::ostream &os, const String &s) {
        os << s.data;
        return os;
}

// test
// 函数形参
void foo(String x) {}
// const常量引用
void bar(const String &x) {}
// 返回值，构造、复制构造
String baz() {
    String ret("world");
    return ret;
}

int main() {
    // 构造
    String s0;
    String s1("hello");
    String s2(s0);
    String s3 = s1;
    // =重载
    s2 = s1;

    foo(s1);
    bar(s1);
    foo("temporary");
    bar("temporary");
    String s4 = baz();

    std::vector<String> svec;
    svec.push_back(s0);
    svec.push_back(s1);
    svec.push_back(baz());
    svec.push_back("good job");
    for (auto c: svec) {
        std::cout << c << std::endl;
    }
}