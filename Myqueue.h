#ifndef MYQUEUE_H
#define MYQUEUE_H

// Apni circular queue (int ki), fixed size array ke saath.
// SPFA me ek node ek waqt me sirf ek baar queue me hota hai (inQueue flag),
// isliye capacity = n + 1 kaafi hai.
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
        data[(head + count) % capacity] = x;      // peeche se daalo
        count++;
    }

    int pop() {
        int x = data[head];
        head = (head + 1) % capacity;             // aage se nikalo
        count--;
        return x;
    }

private:
    IntQueue(const IntQueue&);                    // copy band (delete[] do baar na ho)
    IntQueue& operator=(const IntQueue&);
};

#endif