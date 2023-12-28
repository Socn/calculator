#include<vector>
#include "fraction.h"
#include<math.h>

#ifndef _FUNCTIONS

class Function {
public:
    wchar_t functionChar;
    std::wstring displayString;
    int priority;
    Fraction(*calc)(Fraction);

    Function(wchar_t fC, std::wstring dS, int p, Fraction(*c)(Fraction)) {
        functionChar = fC;
        displayString = dS;
        priority = p;
        calc = c;
    }
};

const Function* Sin = new Function(L'\u00C0', L"sin", 0, [](Fraction x) {return Fraction(sin((double)x));});
const Function* Cos = new Function(L'\u00C1', L"cos", 0, [](Fraction x) {return Fraction(cos((double)x));});
const Function* Tan = new Function(L'\u00C2', L"tan", 0, [](Fraction x) {return Fraction(tan((double)x));});
const Function* Sqrt = new Function(L'\u00C3', L"√", 0, [](Fraction x) {return Fraction(sqrt((double)x));});
const Function* Inverse = new Function(L'\u00C4', L"1/", 0, [](Fraction x) {
    if (x.numerator == 0)return Fraction(0);
    return Fraction(x.denominator, x.numerator);
    });
const Function* Ln = new Function(L'\u00C5', L"ln", 0, [](Fraction x) {return Fraction(log((double)x));});

const Function* _Functions[] = { Sin, Cos, Tan, Sqrt, Inverse, Ln };

#endif
#define _FUNCTIONS