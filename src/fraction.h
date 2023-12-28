#include <iostream>
#include <math.h>

#ifndef _FRACTION
double _gcd(double a, double b) { return (long long)b == 0 ? a : _gcd(b, (long long)a % (long long)b); }
int decimalCnt(double x) {
    x = x - (long long)x;
    for (int i = 0;i < 8;i++) {
        x *= 10;
        if (x - (long long)x == 0) {
            return i;
        }
    }
    return 8;
}
class Fraction {
public:
    double numerator; // 分子
    double denominator; // 分母
    Fraction() {
        numerator = 0;
        denominator = 1;
    }
    Fraction(double n) {
        int decimal = decimalCnt(n);
        numerator = (n * std::pow(10, decimal));
        denominator = (1 * std::pow(10, decimal));
    }
    Fraction(double n, double d) {
        int decimal = decimalCnt(n);
        numerator = n * std::pow(10, decimal);
        denominator = d * std::pow(10, decimal);
    }
    Fraction operator+(Fraction x) {
        double newNumerator1 = numerator * x.denominator;
        double newNumerator2 = x.numerator * denominator;
        double newNumerator = newNumerator1 + newNumerator2;
        double newDenominator = denominator * x.denominator;
        double gcd = _gcd(newNumerator, newDenominator);
        return Fraction(newNumerator / gcd, newDenominator / gcd);
    }
    Fraction operator+(double x) {
        double newNumerator = numerator + x * denominator;
        double newDenominator = denominator;
        double gcd = _gcd(newNumerator, newDenominator);
        return Fraction(newNumerator / gcd, newDenominator / gcd);
    }
    Fraction operator-(Fraction x) {
        double newNumerator1 = numerator * x.denominator;
        double newNumerator2 = x.numerator * denominator;
        double newNumerator = newNumerator1 - newNumerator2;
        double newDenominator = denominator * x.denominator;
        double gcd = _gcd(newNumerator, newDenominator);
        return Fraction(newNumerator / gcd, newDenominator / gcd);
    }
    Fraction operator-(double x) {
        double newNumerator = numerator - x * denominator;
        double newDenominator = denominator;
        if (newNumerator == 0)newDenominator = 1;
        double gcd = _gcd(newNumerator, newDenominator);
        return Fraction(newNumerator / gcd, newDenominator / gcd);
    }
    // Fraction operator*(Fraction x) {
    //     double newNumerator = numerator * x.numerator;
    //     double newDenominator = denominator * x.denominator;
    //     double gcd = _gcd(newNumerator, newDenominator);
    //     return Fraction(newNumerator / gcd, newDenominator / gcd);
    // }
    Fraction operator*(const Fraction x) {
        double newNumerator = numerator * x.numerator;
        double newDenominator = denominator * x.denominator;
        double gcd = _gcd(newNumerator, newDenominator);
        return Fraction(newNumerator / gcd, newDenominator / gcd);
    }
    Fraction operator*(double x) {
        double newNumerator = numerator * x;
        double newDenominator = denominator;
        double gcd = _gcd(newNumerator, newDenominator);
        return Fraction(newNumerator / gcd, newDenominator / gcd);
    }
    Fraction operator/(Fraction x) {
        double newNumerator = numerator * x.denominator;
        double newDenominator = denominator * x.numerator;
        double gcd = _gcd(newNumerator, newDenominator);
        return Fraction(newNumerator / gcd, newDenominator / gcd);
    }
    Fraction operator/(double x) {
        double newNumerator = numerator;
        double newDenominator = denominator * x;
        double gcd = _gcd(newNumerator, newDenominator);
        return Fraction(newNumerator / gcd, newDenominator / gcd);
    }
    Fraction operator=(double num) {
        numerator = num;
        denominator = 1;
        return *this;
    }
    bool operator<(Fraction x) {
        double newNumerator1 = numerator * x.denominator;
        double newNumerator2 = x.numerator * denominator;
        double newDenominator = denominator * x.denominator;
        return newNumerator1 < newNumerator2;
    }

    friend std::ostream& operator<<(std::ostream& out, Fraction& x) {
        out << (double)x.numerator / x.denominator;
        return out;
    }

    operator double() {
        return (double)numerator / denominator;
    }
};
#endif

#define _FRACTION