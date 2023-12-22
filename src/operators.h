#include<vector>
#include "fraction.h"
#include<math.h>

class Operator {
public:
    char operatorChar;
    int priority;
    Fraction(*calc)(Fraction, Fraction);
    Operator(char c, int p, Fraction(*func)(Fraction, Fraction)) {
        operatorChar = c;
        priority = p;
        calc = func;
    }
    Operator() {
        operatorChar = '\0';
        priority = -1;
        calc = nullptr;
    }
};

const Operator* Plus = new Operator('+', 1, [](Fraction a, Fraction b) {return a + b;});
const Operator* Minus = new Operator('-', 1, [](Fraction a, Fraction b) {return a - b;});
const Operator* Multiply = new Operator('*', 2, [](Fraction a, Fraction b) {return a * b;});
const Operator* Divide = new Operator('/', 2, [](Fraction a, Fraction b) {return a / b;});
const Operator* Power = new Operator('^', 3, [](Fraction a, Fraction b) {return Fraction(pow((double)a.numerator, (double)b), pow((double)a.denominator, (double)b));});

const Operator* _Operators[] = { Plus, Minus, Multiply, Divide, Power };