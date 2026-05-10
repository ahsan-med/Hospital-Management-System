#pragma once
#include <iostream>
using namespace std;

// Storage<T> Generic fixed-capacity container/ template functions
// Internally uses a static array of T* pointers
// T must expose a getID() method returning int.


template <typename T>
class Storage {
private:
    static const int CAPACITY = 100;
    T* data[CAPACITY];   // array of pointers to heap allocated T objects
    int count;

public:
    // Constructor / Destructor 
    Storage() : count(0) {
        for (int i = 0; i < CAPACITY; i++)
            data[i] = nullptr;
    }
    ~Storage() {
        clear();
    }

    // Disable copy, owns raw pointers
    Storage(const Storage&) = delete;
    Storage& operator=(const Storage&) = delete;

    //Core operations

    // Add a heap allocated object. Returns false if full
    bool add(T* obj) {
        if (count >= CAPACITY) return false;
        data[count++] = obj;
        return true;
    }

    // Remove by ID. Deletes the object, shifts array left, Returns true if found and removed
    bool removeByID(int id) {
        for (int i = 0; i < count; i++) {
            if (data[i] && data[i]->getID() == id) {
                delete data[i];
                // Shift left
                for (int j = i; j < count - 1; j++)
                    data[j] = data[j + 1];
                data[--count] = nullptr;
                return true;
            }
        }
        return false;
    }

    // Find by ID. Returns pointer to object or nullptr.
    T* findByID(int id) const {
        for (int i = 0; i < count; i++)
            if (data[i] && data[i]->getID() == id)
                return data[i];
        return nullptr;
    }

    // Get object at index (for iteration). Returns nullptr if out of range.
    T* get(int index) const {
        if (index < 0 || index >= count) return nullptr;
        return data[index];
    }

    // Pass raw pointer array to functions
    T** getAll() { return data; }
    int size() const { return count; }
    bool isFull() const { return count >= CAPACITY; }
    bool isEmpty() const { return count == 0; }

    // Delete all objects and reset
    void clear() {
        for (int i = 0; i < count; i++) {
            delete data[i];
            data[i] = nullptr;
        }
        count = 0;
    }

    //Utility
    // Find the maximum ID currently stored, for auto-increment
    int maxID() const {
        int mx = 0;
        for (int i = 0; i < count; i++)
            if (data[i] && data[i]->getID() > mx)
                mx = data[i]->getID();
        return mx;
    }

    T* getAt(int index) const {
        if (index < 0 || index >= count) return nullptr;
        return data[index];
    }
};
