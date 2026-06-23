/**
 * CalculatorEngine
 * -----------------
 * This is the JavaScript twin of your C++ CalculatorEngine class.
 * Notice the architecture is IDENTICAL: pure math, zero knowledge of
 * buttons or the DOM. This file could be reused in a Node.js backend,
 * a different web page, anywhere — exactly like the C++ version could
 * be reused in a CLI tool.
 *
 * In C++, invalid operations throw std::invalid_argument.
 * In JavaScript, the equivalent is throwing a standard Error object.
 * The calling code (app.js) catches it with try/catch — same pattern,
 * different syntax.
 */
class CalculatorEngine {
    add(a, b) {
        return a + b;
    }

    subtract(a, b) {
        return a - b;
    }

    multiply(a, b) {
        return a * b;
    }

    divide(a, b) {
        if (b === 0) {
            throw new Error("Division by zero is undefined.");
        }
        return a / b;
    }

    // The 'op' parameter is a string ("add", "subtract", etc.) instead
    // of a C++ enum class — JavaScript doesn't have true enums, so a
    // string is the idiomatic equivalent here.
    calculate(a, b, op) {
        switch (op) {
            case "add": return this.add(a, b);
            case "subtract": return this.subtract(a, b);
            case "multiply": return this.multiply(a, b);
            case "divide": return this.divide(a, b);
            default: throw new Error("Unknown operation.");
        }
    }

    squareRoot(a) {
        if (a < 0) {
            throw new Error("Cannot take the square root of a negative number.");
        }
        return Math.sqrt(a);
    }

    square(a) {
        return a * a;
    }

    logarithm(a) {
        if (a <= 0) {
            throw new Error("Logarithm is undefined for zero or negative numbers.");
        }
        return Math.log10(a);
    }
}
