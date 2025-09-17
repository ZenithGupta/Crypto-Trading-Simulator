#ifndef MINIMAL_LIST_HPP
#define MINIMAL_LIST_HPP

#include <stdexcept>

template <typename T>
class MinimalList {
private:
    T* data;
    int count;
    int capacity;

    void resize() {
        capacity = (capacity == 0) ? 2 : capacity * 2;
        T* newData = new T[capacity];
        for (int i = 0; i < count; ++i) {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

public:
    MinimalList() : data(nullptr), count(0), capacity(0) {}

    ~MinimalList() {
        delete[] data;
    }

    void push_back(const T& value) {
        if (count == capacity) {
            resize();
        }
        data[count++] = value;
    }

    T& at(int index) const {
        if (index >= 0 && index < count) {
            return data[index];
        }
        throw std::out_of_range("Index out of bounds");
    }

    int size() const {
        return count;
    }
};

#endif 