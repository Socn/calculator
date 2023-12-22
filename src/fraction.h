#include<iostream>

#ifndef _FRACTION
double _gcd(double a, double b) { return b == 0 ? a : _gcd(b, (long long)a % (long long)b); }
class Fraction {
public:
    double numerator; // 分子
    double denominator; // 分母
    Fraction() {
        numerator = 0;
        denominator = 1;
    }
    Fraction(double n) {
        numerator = n;
        denominator = 1;
    }
    Fraction(double n, double d) {
        numerator = n;
        denominator = d;
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
        double gcd = _gcd(newNumerator, newDenominator);
        return Fraction(newNumerator / gcd, newDenominator / gcd);
    }
    Fraction operator*(Fraction x) {
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