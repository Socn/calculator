#include<vector>
#include "fraction.h"
#include<math.h>

double binpow(double a, long long b) {
    double res = 1;
    while (b > 0) {
        if (b & 1) res = res * a;
        a = a * a;
        b >>= 1;
    }
    return res;
}

class Operator {
public:
    char operatorChar;
    int priority;
    bool inversed;
    Fraction(*calc)(Fraction, Fraction);
    Operator(char c, int p, Fraction(*func)(Fraction, Fraction), bool i = false) {
        operatorChar = c;
        priority = p;
        calc = func;
        inversed = i;
    }
    Operator() {
        operatorChar = '\0';
        priority = -1;
        calc = nullptr;
    }
    Operator(const Operator* Op, int p) {
        operatorChar = Op->operatorChar;
        priority = p;
        inversed = Op->inversed;
        calc = Op->calc;
    }
};

const Operator* Plus = new Operator('+', 1, [](Fraction a, Fraction b) {return a + b;});
const Operator* Minus = new Operator('-', 1, [](Fraction a, Fraction b) {return a - b;});
const Operator* Multiply = new Operator('*', 2, [](Fraction a, Fraction b) {return a * b;});
const Operator* Divide = new Operator('/', 2, [](Fraction a, Fraction b) {return a / b;});
const Operator* Power = new Operator('^', 3, [](Fraction a, Fraction b) {return Fraction(pow((double)a.numerator, (double)b), pow((double)a.denominator, (double)b));}, true);

const Operator* _Operators[] = { Plus, Minus, Multiply, Divide, Power };