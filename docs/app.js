/**
 * app.js — UI Layer
 * -------------------
 * This is the JavaScript twin of MainWindow.cpp. The translation map:
 *
 *   C++ / Qt                          JavaScript / Browser
 *   ----------------------------      ----------------------------
 *   connect(button, clicked, slot)    button.addEventListener('click', fn)
 *   QObject::sender()                 event.currentTarget
 *   button->property("role")          button.dataset.role
 *   keyPressEvent(QKeyEvent*)         document.addEventListener('keydown', fn)
 *   QMessageBox::warning(...)         showError(message) — a custom function
 *                                     we write ourselves (the browser has
 *                                     no built-in equivalent we'd want to use)
 *
 * Just like MainWindow never performed math itself, this file never
 * computes anything directly — every calculation is delegated to
 * CalculatorEngine, defined in calculator-engine.js.
 */

const engine = new CalculatorEngine();
const display = document.getElementById("display");
const errorBanner = document.getElementById("error-banner");

// --- State (mirrors the private member variables in MainWindow.h) ---
let firstOperand = 0;
let pendingOperation = "add";
let waitingForSecondOperand = false;
let errorTimeout = null;

// ---------------- Shared input handlers (single source of truth) ----------------
// Exactly like in the C++ version, both button clicks AND keyboard
// presses call these same functions — one source of truth, no duplicated logic.

function inputDigit(digit) {
    if (waitingForSecondOperand) {
        display.value = digit;
        waitingForSecondOperand = false;
    } else {
        display.value = display.value === "0" ? digit : display.value + digit;
    }
}

function inputDecimalPoint() {
    if (waitingForSecondOperand) {
        display.value = "0.";
        waitingForSecondOperand = false;
        return;
    }
    if (!display.value.includes(".")) {
        display.value += ".";
    }
}

function inputOperator(op) {
    firstOperand = parseFloat(display.value);
    pendingOperation = op;
    waitingForSecondOperand = true;
}

function inputEquals() {
    const secondOperand = parseFloat(display.value);
    try {
        const result = engine.calculate(firstOperand, secondOperand, pendingOperation);
        display.value = formatResult(result);
    } catch (err) {
        showError(err.message);
    }
    waitingForSecondOperand = false;
}

function inputClear() {
    display.value = "0";
    firstOperand = 0;
    waitingForSecondOperand = false;
}

function inputUnary(role) {
    const current = parseFloat(display.value);
    try {
        let result;
        if (role === "unary_sqrt") result = engine.squareRoot(current);
        else if (role === "unary_square") result = engine.square(current);
        else if (role === "unary_log") result = engine.logarithm(current);
        else return;

        display.value = formatResult(result);
    } catch (err) {
        showError(err.message);
    }
    waitingForSecondOperand = false;
}

// Trims floating-point noise (e.g. 0.1 + 0.2 producing 0.30000000000004)
// so the display stays clean — JavaScript numbers need this; C++'s
// QString::number() handled this more gracefully by default.
function formatResult(value) {
    return parseFloat(value.toPrecision(12)).toString();
}

function showError(message) {
    errorBanner.textContent = message;
    errorBanner.classList.add("visible");
    clearTimeout(errorTimeout);
    errorTimeout = setTimeout(() => errorBanner.classList.remove("visible"), 2500);
    inputClear();
}

// ---------------- Button click wiring (mouse path) ----------------

document.querySelectorAll("button[data-role]").forEach((button) => {
    button.addEventListener("click", () => {
        const role = button.dataset.role;

        if (role === "digit") inputDigit(button.textContent);
        else if (role === "decimal") inputDecimalPoint();
        else if (role === "clear") inputClear();
        else if (role === "equals") inputEquals();
        else if (role.startsWith("op_")) {
            const map = { op_add: "add", op_sub: "subtract", op_mul: "multiply", op_div: "divide" };
            inputOperator(map[role]);
        } else if (role.startsWith("unary_")) {
            inputUnary(role);
        }
    });
});

// ---------------- Keyboard path ----------------

document.addEventListener("keydown", (event) => {
    const key = event.key;

    if (key >= "0" && key <= "9") {
        inputDigit(key);
        return;
    }

    switch (key) {
        case "+": inputOperator("add"); return;
        case "-": inputOperator("subtract"); return;
        case "*": inputOperator("multiply"); return;
        case "/": event.preventDefault(); inputOperator("divide"); return; // preventDefault stops the browser's "quick find" slash shortcut
        case ".": case ",": inputDecimalPoint(); return;
        case "Enter": case "=": inputEquals(); return;
        case "Backspace": case "Delete": case "Escape": inputClear(); return;
        default: break;
    }
});
