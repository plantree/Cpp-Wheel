#include <algorithm>
#include <memory>
#include <utility>
#include <initializer_list>
#include <iostream>

using namespace std;

template <typename T>
class Vector {
    friend ostream &operator<<(ostream &, const Vector<T> &);
public:
    Vector() = default;
    Vector(const Vector &);                 // copy constructor
    Vector(Vector &&) noexcept;             // move constructor
    Vector &operator=(const Vector &);      // copy assignment
    Vector &operator=(Vector &&) noexcept;  // move assignment
    ~Vector() noexcept;

    // list assignment
    Vector &operator=(std::initializer_list<T>);

    // add elements
    void push_back(const T&);
    void push_back(T&&);
    // ???????
    template <class... Args> void emplace_back(Args&&...);

    // size and capacity
    size_t size() const {
        return first_free - elements;
    }
    size_t capacity() const {
        return cap - elements;
    }

    // element access
    T& operator[](size_t n) {
        return elements[n];
    }
    const T& operator[](size_t n) const {
        return elements[n];
    }

    // iterator interface
    T *begin() const {
        return elements;
    }
    T *end() const {
        return first_free;
    }
private:
    static std::allocator<T> alloc;     // allocates the elements
    // resize
    void chk_n_alloc() {
        if (first_free == cap) {
            reallocate();
        }
    }

    std::pair<T*, T*> alloc_n_copy(const T*, const T*);
    void free();
    void reallocate();          // get more space and copy esisting elements
    T* elements = nullptr;      // pointer to first elements in the array
    T* first_free = nullptr;    // pointer to first free elements in the array
    T* cap = nullptr;           // pointer to one past the end of the array
};

// definition for the static data member
template <typename T> std::allocator<T> Vector<T>::alloc;

template <typename T>
inline
Vector<T>::Vector(const Vector &rhs) {
    auto new_data = alloc_n_copy(rhs.begin(), rhs.end());
    elements = new_data.first;
    cap = first_free = new_data.second;
}

template <typename T>
inline
Vector<T>::Vector(Vector &&rhs) noexcept :
    elements(rhs.elements), first_free(rhs.first_free), cap(rhs.cap) {
    rhs.elements = rhs.first_free = rhs.cap = nullptr;
}

template <typename T>
inline
Vector<T> &Vector<T>::operator=(std::initializer_list<T> il) {
    // copy allocates space and copies elemetns from the given range
    auto data = alloc_n_copy(il.begin(), il.end());

    free(); // destroy the elemnts in this object and free the space
    
    elements = data.first;
    first_free = cap = data.second;

    return *this;
}

template <typename T>
inline
Vector<T> &Vector<T>::operator=(Vector &&rhs) noexcept {
    // direct test for self-assignment
    if (this != &rhs) {
        free();
        elements = rhs.elements;
        first_free = rhs.first_free;
        cap = rhs.cap;

        // leave rhs in a destructible state
        rhs.elements = rhs.first_free = rhs.cap = nullptr;
    }

    return *this;
}

template <typename T>
inline
Vector<T> &Vector<T>::operator=(const Vector &rhs) {
    auto data = alloc_n_copy(rhs.begin(), rhs.end());
    free();
    elements = data.first;
    first_free = cap = data.second;
    return *this;
}

template <typename T>
inline
Vector<T>::~Vector() noexcept {
    free();
}

template <typename T> 
inline
std::pair<T*, T*>
Vector<T>::alloc_n_copy(const T *b, const T *e) {
    // allocate space
    auto data = alloc.allocate(e-b);
    // copy
    return {data, uninitialized_copy(b, e, data)};
}

template <typename T>
inline
void Vector<T>::free() {
    // destroy the old elements in reverse order
    for (auto p = first_free; p != elements; ) {
        alloc.destroy(--p);
    }
    // deallocate cannot be called on 0 pointer
    if (elements) {
        alloc.deallocate(elements, cap-elements);
    }
} 

template <typename T>
inline
void Vector<T>::reallocate() {
    // allocate space for twice as many elements as current size
    auto newcapacity = size() ? 2 * size() : 2;

    // allocate new space
    auto first = alloc.allocate(newcapacity);
    auto dest = first;
    auto elem = elements;

    // move the elements
    for (size_t i = 0; i != size(); ++i) {
        alloc.construct(dest++, std::move(*elem++));
    }
    free();

    // update our data struction point the the new elements
    elements = first;
    first_free = dest;
    cap = elements + newcapacity;
}

template <typename T>
inline
void Vector<T>::push_back(const T& s) {
    chk_n_alloc();  // reallocate the space if necessary
    alloc.construct(first_free++, s);
}

template <typename T>
inline
void Vector<T>::push_back(T&& s) {
    chk_n_alloc();
    alloc.construct(first_free++, std::move(s));
}

template <typename T>
template <class... Args>
inline
void Vector<T>::emplace_back(Args&&... args) {
    chk_n_alloc();
    alloc.construct(first_free++, std::forward<Args>(args)...);
}

template <typename T>
ostream &operator<<(ostream &os, Vector<T> &vec) {
    for (auto c : vec) {
        os << c << " ";
    }
    return os;
}

int main() {
    Vector<int> ivec;
    ivec = {1, 2, 3};
    cout << ivec << endl;
    cout << ivec.capacity() << endl;

    Vector<int> ivec1 = ivec;
    ivec1.push_back(1);
    ivec1.push_back(2);
    cout << ivec1 << endl;
    cout << ivec1.capacity() << endl;
}



