/**
 * 要求：设计并实现大整数类，并测试其加减乘除运算
**/

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <stdexcept>

using namespace std;

class BigNumber {
    friend BigNumber operator+(const BigNumber &lhs, const BigNumber &rhs);
    friend vector<int> basicAdd(const BigNumber &lhs, const BigNumber &rhs);
    friend BigNumber operator-(const BigNumber &lhs, const BigNumber &rhs);
    friend vector<int> basicMinus(const BigNumber &lhs, const BigNumber &rhs, bool &isNegative);
    friend BigNumber operator*(const BigNumber &lhs, const BigNumber &rhs);
    friend BigNumber operator/(const BigNumber &lhs, const BigNumber &rhs);
    friend bool operator==(const BigNumber &lhs, const BigNumber &rhs);
    friend bool operator!=(const BigNumber &lhs, const BigNumber &rhs);
    friend ostream &operator<<(ostream &, const BigNumber &);
    friend istream &operator>>(istream &, BigNumber &);
public:
    // 构造函数
    BigNumber();
    BigNumber(long int x);
    BigNumber(string s);
    // 拷贝函数
    BigNumber(const BigNumber &n);
    BigNumber &operator=(const BigNumber &n);
    ~BigNumber() {}
    BigNumber abs(const BigNumber &n) const {
        BigNumber b;
        b.isNegative = false;
        b.pvec = n.pvec;
        return b;
    }
private:
    // 倒序存储
    shared_ptr<vector<int>> pvec;
    bool isNegative = false;
};

// 构造函数
BigNumber::BigNumber() : isNegative(false), pvec(make_shared<vector<int>>()) {

}

// 委托构造
BigNumber::BigNumber(long int x) : BigNumber() {
    bool flag = false;
    if (x < 0) {
        isNegative = true;
        // 最小值
        if (x == LONG_MIN) {
            x = LONG_MAX;
            flag = true;
        } else {
            x = -x;
        }
    }

    while (x) {
        pvec->push_back(x%10);
        x /= 10;
    }
    if (flag) {
        (*pvec)[0] = 8;
    }
}

BigNumber::BigNumber(string s) : pvec(make_shared<vector<int>>()) {
    if (s.empty()) {
        throw runtime_error("string is empty");
    }
    if (s[0] == '-') {
        isNegative = true;
    } else {
        isNegative = false;
    }
    for (auto begin = s.cend()-1; begin != s.cbegin(); --begin) {
        if (*begin >= '0' && *begin <= '9') {
            pvec->push_back(*begin-'0');
        } else {
            throw runtime_error("invalid number");
        }
    }
    if (s[0] >= '0' && s[0] <= '9') {
        pvec->push_back(s[0]-'0');
    }
}

// 拷贝函数
BigNumber::BigNumber(const BigNumber &n) : pvec(n.pvec), isNegative(n.isNegative) {

}

BigNumber& BigNumber::operator=(const BigNumber &n) {
    isNegative = n.isNegative;
    pvec = n.pvec;
    return *this;
}

// 友元
ostream &operator<<(ostream &os, const BigNumber &n) {
    if (n.pvec->empty()) {
        os << "null";
        return os;
    }
    if (n.isNegative) {
        os << "-";
    }
    for (auto begin = n.pvec->cend()-1; begin >= n.pvec->cbegin(); --begin) {
        os << *begin;
    }
    return os;
}

istream &operator>>(istream &is, BigNumber &n) {
    string s;
    is >> s;
    n = BigNumber(s);
    return is;
}

// 运算
bool operator==(const BigNumber &lhs, const BigNumber &rhs) {
    if ((lhs.isNegative != rhs.isNegative) ||
        (lhs.pvec->size() != rhs.pvec->size())) {
        return false;
    }
    for (int i = 0; i < lhs.pvec->size(); ++i) {
        if ((*lhs.pvec)[i] != (*rhs.pvec)[i]) {
            return false;
        }
    }
    return true;
}

bool operator!=(const BigNumber &lhs, const BigNumber &rhs) {
    return !(lhs == rhs);
}

vector<int> basicAdd(const BigNumber &lhs, const BigNumber &rhs) {
    vector<int> res;
    // 进位
    int carry = 0,
        sum = 0,
        i = 0;
    int len1 = lhs.pvec->size(),
        len2 = rhs.pvec->size();
    while (i < len1 || i < len2) {
        if (i < len1 && i < len2) {
            sum = (*lhs.pvec)[i] + (*rhs.pvec)[i] + carry;
        } else if (i < len1) {
            sum = (*lhs.pvec)[i] + carry;
        } else if (i < len2) {
            sum = (*rhs.pvec)[i] + carry;
        }
        carry = sum / 10;
        res.push_back(sum%10);
        sum = 0;
        ++i;
    }
    if (carry) {
        res.push_back(1);
    }
    return res;
}

BigNumber operator+(const BigNumber &lhs, const BigNumber &rhs) {
    if (lhs.pvec->empty()) {
        return rhs;
    }
    if (rhs.pvec->empty()) {
        return lhs;
    }
    BigNumber res;
    if (!lhs.isNegative && !rhs.isNegative) {
        res.isNegative = false;
        res.pvec = make_shared<vector<int>>(basicAdd(lhs, rhs));
    } else if (lhs.isNegative && !rhs.isNegative) {
        bool flag = false;
        auto data = basicMinus(rhs, lhs, flag);
        res.isNegative = flag;
        res.pvec = make_shared<vector<int>>(data);
    } else if (!lhs.isNegative && rhs.isNegative) {
        bool flag = false;
        auto data = basicMinus(lhs, rhs, flag);
        res.isNegative = flag;
        res.pvec = make_shared<vector<int>>(data);
    } else {
        res.isNegative = true;
        res.pvec = make_shared<vector<int>>(basicAdd(lhs, rhs));
    }
    return res;
}

vector<int> basicMinus(const BigNumber &l, const BigNumber &r, bool &isNegative) {
    BigNumber lhs, rhs;
    if (l.pvec->size() > r.pvec->size()) {
        lhs = l;
        rhs = r;
    } else if (l.pvec->size() < r.pvec->size()) {
        lhs = r;
        rhs = l;
        isNegative = true;
    } else {
        int flag = 0;
        for (int i = l.pvec->size()-1; i >= 0; --i) {
            if ((*l.pvec)[i] > (*r.pvec)[i]) {
                flag = 1;
                break;
            } else if ((*l.pvec)[i] < (*r.pvec)[i]) {
                flag = -1;
                break;
            }
        }
        if (flag == 1) {
            lhs = l;
            rhs = r;
        } else if (flag == -1) {
            lhs = r;
            rhs = l;
            isNegative = true;
        } else {
            return {0};
        }
    }
    vector<int> res;
    // 退位
    int carry = 0,
        minus = 0,
        i = 0;
    int len1 = lhs.pvec->size(),
        len2 = rhs.pvec->size();
    while (i < len1 || i < len2) {
        if (i < len1 && i < len2) {
            minus = (*lhs.pvec)[i] - (*rhs.pvec)[i] - carry;
        } else if (i < len1) {
            minus = (*lhs.pvec)[i] - carry;
        } else if (i < len2) {
            isNegative = true;
            minus = (*rhs.pvec)[i] - carry;
        }
        if (minus < 0) {
            carry = 1;
            minus += 10;
        } else {
            carry = 0;
        }
        res.push_back(minus);
        minus = 0;
        ++i;
    }
    return res;
}

BigNumber operator-(const BigNumber &lhs, const BigNumber &rhs) {
    if (lhs.pvec->empty()) {
        return rhs;
    }
    if (rhs.pvec->empty()) {
        return lhs;
    }
    BigNumber res;
    if (lhs.isNegative && rhs.isNegative) {
        bool flag = false;
        auto data = basicMinus(rhs, lhs, flag);
        res.isNegative = flag;
        res.pvec = make_shared<vector<int>>(data);
    } else if (lhs.isNegative && !rhs.isNegative) {
        auto data = basicAdd(lhs, rhs);
        res.isNegative = true;
        res.pvec = make_shared<vector<int>>(data);
    } else if (!lhs.isNegative && rhs.isNegative) {
        auto data = basicAdd(lhs, rhs);
        res.isNegative = false;
        res.pvec = make_shared<vector<int>>(data);
    } else {
        bool flag = false;
        auto data = basicMinus(lhs, rhs, flag);
        res.isNegative = flag;
        res.pvec = make_shared<vector<int>>(data);
    }
    return res;
}



int main() {
    BigNumber b1;
    cout << b1 << endl;
    BigNumber b2(LONG_MIN);
    cout << b2 << endl;
    BigNumber b3(LONG_MAX);
    cout << b3 << endl;
    BigNumber b4("-23456");
    cout << b4 << endl;
    //BigNumber b5("-t3456");
    //cout << b5 << endl;

    BigNumber b6 = b4;
    cout << b6 << endl;
    b6 = b6;
    cout << b6 << endl;

    /*BigNumber b7;
    cin >> b7;
    cout << b7 << endl;*/

    auto res = BigNumber(78) + BigNumber(561);
    cout << res << endl;
    res = BigNumber(78) - BigNumber(561);
    cout << res << endl;
    res = BigNumber(-78) + BigNumber(561);
    cout << res << endl;
    res = BigNumber(78) + BigNumber(-561);
    cout << res << endl;
    res = BigNumber(561) - BigNumber(-78);
    cout << res << endl;
    res = BigNumber(78) + BigNumber(-78);
    cout << res << endl;

    cout << (BigNumber(-78) == BigNumber(-79)) << endl;
    cout << (BigNumber(-78) == BigNumber(-78)) << endl;
}

