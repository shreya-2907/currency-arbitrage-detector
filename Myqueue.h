#ifndef MYQUEUE_H
#define MYQUEUE_H

// Custom circular queue of ints, fixed-size array.
class IntQueue {
private:
    int* data;
    int capacity;
    int head;     // pehla element yahan hai
    int count;    // kitne elements abhi queue me hain

public:
    IntQueue(int cap) {
        capacity = cap;
        data = new int[capacity];
        head = 0;
        count = 0;
    }

    ~IntQueue() {
        delete[] data;
    }

    bool empty() const {
        return count == 0;
    }

    void push(int x) {
        if (count == capacity) return;            // full: ignore
        data[(head + count) % capacity] = x;      // peeche se add
        count++;
    }

    int pop() {
        int x = data[head];
        head = (head + 1) % capacity;             // remove from the front
        count--;
        return x;
    }

private:
    IntQueue(const IntQueue&);                    // copy band (delete[] do baar na ho)
    IntQueue& operator=(const IntQueue&);
};

#endif