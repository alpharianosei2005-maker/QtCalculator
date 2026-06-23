#include <QtTest/QtTest>
#include "../src/CalculatorEngine.h"

/**
 * This test file ONLY includes CalculatorEngine — not MainWindow, not
 * QApplication. That's the payoff of separating logic from UI: these
 * tests run in milliseconds with no GUI window ever appearing, and
 * could run automatically in a CI pipeline (e.g. GitHub Actions) on
 * every commit.
 */
class CalculatorEngineTest : public QObject
{
    Q_OBJECT

private slots:
    void testAddition();
    void testSubtraction();
    void testMultiplication();
    void testDivision();
    void testDivisionByZeroThrows();
};

void CalculatorEngineTest::testAddition()
{
    CalculatorEngine engine;
    QCOMPARE(engine.add(2.0, 3.0), 5.0);
    QCOMPARE(engine.add(-2.0, 2.0), 0.0);
}

void CalculatorEngineTest::testSubtraction()
{
    CalculatorEngine engine;
    QCOMPARE(engine.subtract(5.0, 3.0), 2.0);
}

void CalculatorEngineTest::testMultiplication()
{
    CalculatorEngine engine;
    QCOMPARE(engine.multiply(4.0, 0.5), 2.0);
}

void CalculatorEngineTest::testDivision()
{
    CalculatorEngine engine;
    QCOMPARE(engine.divide(10.0, 2.0), 5.0);
}

void CalculatorEngineTest::testDivisionByZeroThrows()
{
    CalculatorEngine engine;
    // QVERIFY_EXCEPTION_THROWN confirms the engine fails LOUDLY and
    // predictably rather than returning "inf" silently.
    QVERIFY_EXCEPTION_THROWN(engine.divide(5.0, 0.0), std::invalid_argument);
}

QTEST_MAIN(CalculatorEngineTest)
#include "test_calculator.moc"