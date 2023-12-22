#include<bitset>
template<int SIZE>
class myBitset {
public:
    myBitset(std::bitset<SIZE> b) {
        bitset = b;
    }
    myBitset() {
        bitset = 0;
    }
    std::bitset<SIZE> bitset;
    bool operator[](int pos) {
        return bitset[pos];
    }
    friend bool operator<(const myBitset a, const myBitset b) {
        for (int i = 0;i < SIZE;i++) {
            if (a.bitset[i] < b.bitset[i]) {
                return true;
            }
            if (a.bitset[i] > b.bitset[i]) {
                return false;
            }
        }
        return false;
    }
    myBitset operator<<(int x) {
        return myBitset(bitset << 1);
    }
    myBitset operator|(int x) {
        return myBitset(bitset | std::bitset<SIZE>(x));
    }
};