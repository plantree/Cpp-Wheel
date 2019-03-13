#include <iostream>
#include <memory>

using namespace std;

/* ReverseList
struct ListNode {
    int val;
    ListNode *next;
    ListNode(int x) : val(x), next(nullptr) {}
};

ListNode* Reverse(ListNode *head) {
    if (!head) {
        return head;
    }
    ListNode *pre = nullptr,
             *cur = head,
             *next = cur;
    while (cur) {
        next = cur->next;
        cur->next = pre;
        pre = cur;
        cur = next;
    }
    return pre;
}

int main() {
    ListNode *l1 = new ListNode(1);
    ListNode *l2 = new ListNode(2);
    ListNode *l3 = new ListNode(3);
    l1->next = l2;
    l2->next = l3;
    ListNode *cur = l1;
    while (cur) {
        cout << cur->val << "->";
        cur = cur->next;
    }
    cout << endl;

    ListNode *head = Reverse(l1);
    cur = head;
    while (cur) {
        cout << cur->val << "->";
        cur = cur->next;
    }
    cout << endl;

}*/

template <typename T>
class Counter;

template <typename T>
class SmartPointer {
public:
    SmartPointer() {
        ptr_counter = new Counter<T>();
    }
    SmartPointer(T* p) {
        ptr_counter = new Counter<T>(p);
    }
    SmartPointer(const SmartPointer &sp) {
        ptr_counter = sp.ptr_counter;
        ++ptr_counter->cnt;
    }
    SmartPointer& operator=(const SmartPointer &sp) {
        ++sp.ptr_counter->cnt;
        --ptr_counter->cnt;
        if (ptr_counter->cnt == 0) {
            cout << "delete" << endl;
            delete ptr_counter;
            ptr_counter = nullptr;
        }
        ptr_counter = sp.ptr_counter;
        return *this;
    }
    T& operator*() const {
        return *(ptr_counter->ptr);
    }
    T* operator->() const {
        return ptr_counter->ptr;
    }
    int use_count() const {
        return ptr_counter->cnt;
    }
    ~SmartPointer() {
        --ptr_counter->cnt;
        if (ptr_counter->cnt == 0) {
            delete ptr_counter;
        }
    }

private:
    Counter<T> *ptr_counter;
};

template <typename T>
class Counter {
    friend class SmartPointer<T>;
public:
    Counter() {
        ptr = nullptr;
        cnt = 0;
    }
    Counter(T *p) {
        ptr = p;
        cnt = 1;
    }
    ~Counter() {
        delete ptr;
    }
private:
    T* ptr;
    int cnt;
};



int main() {
    
    SmartPointer<int> sp(new int(1));

    cout << sp.use_count() << endl;
    SmartPointer<int> sp1 = sp;
    cout << sp.use_count() << " " << sp1.use_count() << endl;
    SmartPointer<int> sp2(new int(2));
    sp = sp2;
    cout << sp.use_count() << " " << sp1.use_count() << " " << sp2.use_count() << endl;
    sp1 = sp2;
    cout << sp.use_count() << " " << sp1.use_count() << " " << sp2.use_count() << endl;

    shared_ptr<int> s(new int(1));
    cout << s.use_count() << endl;
    shared_ptr<int> s1 = s;
    cout << s.use_count() << " " << s1.use_count() << endl;
    shared_ptr<int> s2(new int(2));
    s = s2;
    cout << s.use_count() << " " << s1.use_count() << " " << s2.use_count() << endl;
    s1 = s2;
    cout << s.use_count() << " " << s1.use_count() << " " << s2.use_count() << endl;
    
}

