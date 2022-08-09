struct edeque { // дек на циклическом массиве
    int* D;
    int n = 1, head = 0, tail = 0, _size = 0;

    inline void resize(size_t len) {
        D = new int[len + 2];
        for (int i = 0; i < len; i++) {
            D[i] = 0;
        }
        _size = len;
        n = len + 2;
        head = 0;
        tail = len;
    }
    inline const int size() const {
        return _size;
    }

private:
    inline int inc(int index)const {
        index++;
        if (index >= n) {
            index -= n;
        }
        return index;
    }
    inline void incf(int& index)const {
        index++;
        if (index >= n) {
            index -= n;
        }
    }
    inline int dec(int index)const {
        index--;
        if (index < 0) {
            index += n;
        }
        return index;
    }
    inline void decf(int& index)const {
        index--;
        if (index < 0) {
            index += n;
        }
    }
public:

    inline bool empty() const {
        return _size == 0;
    }

    inline void push_back(int x) {
        if (head == inc(tail)) {
            int* newD = new int[n * 2];
            for (int i = 0; i < n - 1; i++) {
                newD[i] = D[head];
                incf(head);
            }
            D = newD;
            head = 0;
            tail = n - 1;
            n *= 2;
        }
        D[tail] = x;
        incf(tail);
        _size++;
    }
    inline void pop_back() {
        decf(tail);
        _size--;
    }
    inline void push_front(int x) {
        if (head == inc(tail)) {
            int* newD = new int[n * 2];
            for (int i = 0; i < n - 1; i++) {
                newD[i] = D[head];
                incf(head);
            }
            D = newD;
            head = 0;
            tail = n - 1;
            n *= 2;
        }
        decf(head);
        D[head] = x;
        _size++;
    }
    inline void pop_front() {
        incf(head);
        _size--;
    }

    inline int& back() const {
        return D[dec(tail)];
    }
    inline int& front()const {
        return D[head];
    }

    inline int& operator [](size_t index) const {
        index += head;
        if (index >= n) {
            index -= n;
        }
        return D[index];
    }
};

// debug
/*std::ostream& operator << (std::ostream& output, const edeque& A) {
    for(int i = 0; i < A.size(); i++){
        output << A[i] << " ";
    }
    output << "   " << A.size() << "    " << A.front() <<  " " << A.back() << "\n";
    return output;
}*/