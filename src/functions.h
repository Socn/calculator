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

const Function* Sin = new Function(L'\u00C0', L"sin", 0, [](Fraction x) {return Fraction(sin(x));});
const Function* Cos = new Function(L'\u00C1', L"cos", 0, [](Fraction x) {return Fraction(cos(x));});
const Function* Tan = new Function(L'\u00C2', L"tan", 0, [](Fraction x) {return Fraction(tan(x));});

const Function* _Functions[] = { Sin, Cos, Tan };

#endif
#define _FUNCTIONS