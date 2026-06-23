#include "CalculatorEngine.h"
#include <cmath>

CalculatorEngine::CalculatorEngine(QObject *parent)
    : QObject(parent)
{
}

double CalculatorEngine::add(double a, double b) const { return a + b; }
double CalculatorEngine::subtract(double a, double b) const { return a - b; }
double CalculatorEngine::multiply(double a, double b) const { return a * b; }

double CalculatorEngine::divide(double a, double b) const
{
    if (b == 0.0) {
        throw std::invalid_argument("Division by zero is undefined.");
    }
    return a / b;
}

double CalculatorEngine::calculate(double a, double b, Operation op) const
{
    switch (op) {
        case Operation::Add:      return add(a, b);
        case Operation::Subtract: return subtract(a, b);
        case Operation::Multiply: return multiply(a, b);
        case Operation::Divide:   return divide(a, b);
    }
    throw std::logic_error("Unknown operation.");
}

double CalculatorEngine::squareRoot(double a) const
{
    if (a < 0.0) {
        // Real numbers only — no complex number support, so we fail
        // loudly rather than returning NaN silently.
        throw std::invalid_argument("Cannot take the square root of a negative number.");
    }
    return std::sqrt(a);
}

double CalculatorEngine::square(double a) const
{
    return a * a;
}

double CalculatorEngine::logarithm(double a) const
{
    if (a <= 0.0) {
        throw std::invalid_argument("Logarithm is undefined for zero or negative numbers.");
    }
    return std::log10(a);
}