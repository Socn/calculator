#include<vector>
#include "fraction.h"
#include<math.h>


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

const Function* Sin = new Function('\u0021', L"sin", 0, [](Fraction x) {return Fraction(tan(x));});

const Function* _Functionss[] = { Sin };