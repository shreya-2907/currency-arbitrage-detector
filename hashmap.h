#ifndef HASHMAP_H
#define HASHMAP_H

#include <string>
using namespace std;

// Custom hash map: string (currency name) -> int (index in the graph)
class StringIntMap {
private:
    struct Node {
        string key;
        int value;
        Node* next;
    };

    Node** buckets;      // buckets[i] = i-th linked list's 1st node
    int bucketCount;
    int itemCount;

    // djb2 hash: turns string to no.
    static unsigned long hashOf(const string& s) {
        unsigned long h = 5381;
        for (size_t i = 0; i < s.size(); i++) {
            h = h * 33 + (unsigned char)s[i];
        }
        return h;
    }

    void init(int count) {
        bucketCount = count;
        itemCount = 0;
        buckets = new Node*[bucketCount];
        for (int i = 0; i < bucketCount; i++) buckets[i] = NULL;
    }

    void clear() {
        for (int i = 0; i < bucketCount; i++) {
            Node* cur = buckets[i];
            while (cur != NULL) {
                Node* nxt = cur->next;
                delete cur;
                cur = nxt;
            }
        }
        delete[] buckets;
        buckets = NULL;
    }

    void rehash() {
        int oldCount = bucketCount;
        Node** old = buckets;

        init(oldCount * 2);                      
        for (int i = 0; i < oldCount; i++) {
            Node* cur = old[i];
            while (cur != NULL) {
                Node* nxt = cur->next;
                int pos = hashOf(cur->key) % bucketCount;   
                cur->next = buckets[pos];
                buckets[pos] = cur;
                itemCount++;
                cur = nxt;
            }
        }
        delete[] old;                            
    }

public:
    StringIntMap() {
        init(16);
    }

    StringIntMap(const StringIntMap& other) {     // deep copy
        init(other.bucketCount);
        for (int i = 0; i < other.bucketCount; i++) {
            for (Node* cur = other.buckets[i]; cur != NULL; cur = cur->next) put(cur->key, cur->value);
        }
    }

    StringIntMap& operator=(const StringIntMap& other) {
        if (this == &other) return *this;
        clear();
        init(other.bucketCount);
        for (int i = 0; i < other.bucketCount; i++) {
            for (Node* cur = other.buckets[i]; cur != NULL; cur = cur->next) put(cur->key, cur->value);
        }
        return *this;
    }

    ~StringIntMap() {
        clear();
    }

    int size() const {
        return itemCount;
    }


    int get(const string& key) const {
        int pos = hashOf(key) % bucketCount;
        for (Node* cur = buckets[pos]; cur != NULL; cur = cur->next) {
            if (cur->key == key) return cur->value;
        }
        return -1;
    }


    void put(const string& key, int value) {
        int pos = hashOf(key) % bucketCount;
        for (Node* cur = buckets[pos]; cur != NULL; cur = cur->next) {
            if (cur->key == key) {
                cur->value = value;
                return;
            }
        }

        if ((itemCount + 1) * 4 > bucketCount * 3) {  
            rehash();
            pos = hashOf(key) % bucketCount;
        }

        Node* n = new Node;
        n->key = key;
        n->value = value;
        n->next = buckets[pos];                 
        buckets[pos] = n;
        itemCount++;
    }
};

#endif