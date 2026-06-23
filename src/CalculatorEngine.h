#ifndef CALCULATORENGINE_H
#define CALCULATORENGINE_H

#include <QObject>
#include <stdexcept>

/**
 * CalculatorEngine
 * -----------------
 * Business Logic layer. No knowledge of buttons, windows, or pixels.
 *
 * Two categories of operation:
 *   - Binary  (add/subtract/multiply/divide): need TWO numbers.
 *   - Unary   (squareRoot/square/logarithm):   need only ONE number,
 *             and produce a result immediately — like a scientific
 *             calculator's instant-action keys.
 */
class CalculatorEngine : public QObject
{
    Q_OBJECT

public:
    explicit CalculatorEngine(QObject *parent = nullptr);

    enum class Operation { Add, Subtract, Multiply, Divide };

    // --- Binary operations ---
    double add(double a, double b) const;
    double subtract(double a, double b) const;
    double multiply(double a, double b) const;
    double divide(double a, double b) const; // throws std::invalid_argument on /0
    double calculate(double a, double b, Operation op) const;

    // --- Unary operations ---
    double squareRoot(double a) const; // throws if a < 0 (no real result)
    double square(double a) const;
    double logarithm(double a) const;  // base-10 log; throws if a <= 0
};

#endif // CALCULATORENGINE_H