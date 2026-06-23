#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "CalculatorEngine.h"

QT_BEGIN_NAMESPACE
class QLineEdit;
class QKeyEvent;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    // Overriding this lets us capture keyboard input (digits, operators,
    // Enter, Backspace) at the window level, without attaching a separate
    // QShortcut for every single key.
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    // These remain thin "adapters" that figure out which button/key fired,
    // then delegate to the input* helpers below — so mouse clicks and
    // keyboard presses always go through the exact same logic.
    void onDigitClicked();
    void onDecimalClicked();
    void onOperatorClicked();
    void onEqualsClicked();
    void onClearClicked();
    void onUnaryClicked();

private:
    void buildUi();
    void applyStyle();
    void showError(const QString &message);

    // --- Shared input handlers (single source of truth) ---
    void inputDigit(const QString &digit);
    void inputDecimalPoint();
    void inputOperator(CalculatorEngine::Operation op);
    void inputEquals();
    void inputClear();
    void inputUnary(const QString &role);

    CalculatorEngine *m_engine;
    QLineEdit *m_display;

    double m_firstOperand = 0.0;
    CalculatorEngine::Operation m_pendingOperation = CalculatorEngine::Operation::Add;
    bool m_waitingForSecondOperand = false;
};

#endif // MAINWINDOW_H