#include "MainWindow.h"

#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QMessageBox>
#include <QKeyEvent>
#include <QFont>
#include <QVector>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_engine(new CalculatorEngine(this))
{
    setWindowTitle("Calculator");

    // setMinimumSize (not setFixedSize) gives the OS a floor to respect,
    // while still allowing the user to maximize or freely resize the
    // window. This was the #1 thing blocking maximize before.
    setMinimumSize(320, 480);
    resize(360, 540);

    buildUi();
    applyStyle();
}

void MainWindow::buildUi()
{
    auto *central = new QWidget(this);
    central->setObjectName("central");
    setCentralWidget(central);

    auto *layout = new QGridLayout(central);
    layout->setSpacing(10);
    layout->setContentsMargins(16, 16, 16, 16);

    // --- Display ---
    m_display = new QLineEdit("0", central);
    m_display->setObjectName("display");
    m_display->setAlignment(Qt::AlignRight);
    m_display->setReadOnly(true);
    QFont displayFont = m_display->font();
    displayFont.setPointSize(26);
    m_display->setFont(displayFont);
    m_display->setMinimumHeight(64);
    layout->addWidget(m_display, 0, 0, 1, 4);

    // Each button carries a "role" property — a stable internal ID that
    // never changes even if you later restyle the label (e.g. swap "÷"
    // for a different symbol). Logic is driven by role, not by the
    // text the user sees.
    struct ButtonSpec { QString label; QString role; int row; int col; };
    const QVector<ButtonSpec> buttons = {
        {"\u221A",  "unary_sqrt",   1, 0}, // √
        {"x\u00B2", "unary_square", 1, 1}, // x²
        {"log",     "unary_log",    1, 2},
        {"C",       "clear",        1, 3},

        {"7", "digit", 2, 0}, {"8", "digit", 2, 1}, {"9", "digit", 2, 2},
        {"\u00F7", "op_div", 2, 3}, // ÷

        {"4", "digit", 3, 0}, {"5", "digit", 3, 1}, {"6", "digit", 3, 2},
        {"\u00D7", "op_mul", 3, 3}, // ×

        {"1", "digit", 4, 0}, {"2", "digit", 4, 1}, {"3", "digit", 4, 2},
        {"-", "op_sub", 4, 3},

        {"0", "digit", 5, 0}, {".", "decimal", 5, 1}, {"=", "equals", 5, 2},
        {"+", "op_add", 5, 3},
    };

    for (const auto &spec : buttons) {
        auto *button = new QPushButton(spec.label, central);
        button->setMinimumHeight(52);
        // Expanding size policy is what lets buttons grow proportionally
        // when the window is maximized, instead of staying a fixed size
        // with empty space appearing around them.
        button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        button->setProperty("role", spec.role);
        if (spec.role == "equals") {
            button->setObjectName("equalsButton");
        }
        layout->addWidget(button, spec.row, spec.col);

        if (spec.role == "digit")        connect(button, &QPushButton::clicked, this, &MainWindow::onDigitClicked);
        else if (spec.role == "decimal") connect(button, &QPushButton::clicked, this, &MainWindow::onDecimalClicked);
        else if (spec.role == "clear")   connect(button, &QPushButton::clicked, this, &MainWindow::onClearClicked);
        else if (spec.role == "equals")  connect(button, &QPushButton::clicked, this, &MainWindow::onEqualsClicked);
        else if (spec.role.startsWith("op_"))    connect(button, &QPushButton::clicked, this, &MainWindow::onOperatorClicked);
        else if (spec.role.startsWith("unary_")) connect(button, &QPushButton::clicked, this, &MainWindow::onUnaryClicked);
    }

    // All rows/columns share space equally as the window resizes.
    for (int r = 0; r <= 5; ++r) layout->setRowStretch(r, r == 0 ? 1 : 2);
    for (int c = 0; c <= 3; ++c) layout->setColumnStretch(c, 1);
}

void MainWindow::applyStyle()
{
    // Palette: black background, pale-cream buttons, black text on
    // buttons. The :pressed state is what makes clicks feel tactile —
    // a darker shade plus a tiny padding shift simulates a "press in."
    setStyleSheet(R"(
        QWidget#central {
            background-color: #0A0A0A;
        }
        QLineEdit#display {
            background-color: #000000;
            color: #F5F0E6;
            border: 2px solid #F5F0E6;
            border-radius: 10px;
            padding: 6px 12px;
        }
        QPushButton {
            background-color: #F5F0E6;
            color: #000000;
            border: none;
            border-radius: 10px;
            font-size: 18px;
            font-weight: 600;
        }
        QPushButton:hover {
            background-color: #E6DFCC;
        }
        QPushButton:pressed {
            background-color: #C9BFA8;
            padding-top: 4px;
            padding-left: 2px;
        }
        QPushButton#equalsButton {
            background-color: #D9CBA3;
        }
        QPushButton#equalsButton:pressed {
            background-color: #B8A87C;
        }
    )");
}

// ---------------- Shared input handlers ----------------

void MainWindow::inputDigit(const QString &digit)
{
    if (m_waitingForSecondOperand) {
        m_display->setText(digit);
        m_waitingForSecondOperand = false;
    } else {
        QString currentText = (m_display->text() == "0") ? "" : m_display->text();
        m_display->setText(currentText + digit);
    }
}

void MainWindow::inputDecimalPoint()
{
    if (m_waitingForSecondOperand) {
        m_display->setText("0.");
        m_waitingForSecondOperand = false;
        return;
    }
    if (!m_display->text().contains('.')) {
        m_display->setText(m_display->text() + ".");
    }
}

void MainWindow::inputOperator(CalculatorEngine::Operation op)
{
    m_firstOperand = m_display->text().toDouble();
    m_pendingOperation = op;
    m_waitingForSecondOperand = true;
}

void MainWindow::inputEquals()
{
    const double secondOperand = m_display->text().toDouble();
    try {
        const double result = m_engine->calculate(m_firstOperand, secondOperand, m_pendingOperation);
        m_display->setText(QString::number(result));
    } catch (const std::invalid_argument &e) {
        showError(QString::fromStdString(e.what()));
    }
    m_waitingForSecondOperand = false;
}

void MainWindow::inputClear()
{
    m_display->setText("0");
    m_firstOperand = 0.0;
    m_waitingForSecondOperand = false;
}

void MainWindow::inputUnary(const QString &role)
{
    const double current = m_display->text().toDouble();
    try {
        double result = 0.0;
        if (role == "unary_sqrt")        result = m_engine->squareRoot(current);
        else if (role == "unary_square") result = m_engine->square(current);
        else if (role == "unary_log")    result = m_engine->logarithm(current);
        else return;

        m_display->setText(QString::number(result));
    } catch (const std::invalid_argument &e) {
        showError(QString::fromStdString(e.what()));
    }
    m_waitingForSecondOperand = false;
}

// ---------------- Button slots (mouse path) ----------------

void MainWindow::onDigitClicked()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    if (button) inputDigit(button->text());
}

void MainWindow::onDecimalClicked()
{
    inputDecimalPoint();
}

void MainWindow::onOperatorClicked()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    if (!button) return;

    const QString role = button->property("role").toString();
    if (role == "op_add")      inputOperator(CalculatorEngine::Operation::Add);
    else if (role == "op_sub") inputOperator(CalculatorEngine::Operation::Subtract);
    else if (role == "op_mul") inputOperator(CalculatorEngine::Operation::Multiply);
    else if (role == "op_div") inputOperator(CalculatorEngine::Operation::Divide);
}

void MainWindow::onEqualsClicked()
{
    inputEquals();
}

void MainWindow::onClearClicked()
{
    inputClear();
}

void MainWindow::onUnaryClicked()
{
    auto *button = qobject_cast<QPushButton *>(sender());
    if (button) inputUnary(button->property("role").toString());
}

// ---------------- Keyboard path ----------------

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();

    // Digits 0-9 (works for both the main keyboard and numpad).
    if (key >= Qt::Key_0 && key <= Qt::Key_9) {
        inputDigit(QString::number(key - Qt::Key_0));
        return;
    }

    switch (key) {
        case Qt::Key_Plus:
            inputOperator(CalculatorEngine::Operation::Add);
            return;
        case Qt::Key_Minus:
            inputOperator(CalculatorEngine::Operation::Subtract);
            return;
        case Qt::Key_Asterisk:
            inputOperator(CalculatorEngine::Operation::Multiply);
            return;
        case Qt::Key_Slash:
            inputOperator(CalculatorEngine::Operation::Divide);
            return;
        case Qt::Key_Period:
        case Qt::Key_Comma:
            inputDecimalPoint();
            return;
        case Qt::Key_Enter:
        case Qt::Key_Return:
            inputEquals();
            return;
        case Qt::Key_Backspace:
        case Qt::Key_Delete:
        case Qt::Key_Escape:
            // Per your request: Backspace/Delete (and Escape, kept as a
            // bonus) reset the calculator to 0, same as clicking "C".
            inputClear();
            return;
        default:
            break;
    }

    // Anything we don't handle gets passed up to Qt's default behavior.
    QMainWindow::keyPressEvent(event);
}

void MainWindow::showError(const QString &message)
{
    QMessageBox::warning(this, "Calculation Error", message);
    inputClear();
}