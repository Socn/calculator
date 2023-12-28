#include<vector>
#include "fraction.h"
#include<math.h>

#ifndef _CONSTANTS

class Constant {
public:
    wchar_t constantChar;
    Fraction value;

    Constant(wchar_t cC, Fraction v) {
        constantChar = cC;
        value = v;
    }
};

const Constant* Pi = new Constant(L'π', Fraction(M_PI));
const Constant* E = new Constant(L'e', Fraction(M_E));

const Constant* _Constants[] = { Pi, E };

#endif
#define _CONSTANTS