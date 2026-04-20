#pragma once
#include "CalculatorState.h"

namespace CalculatorWorkspace {

class CalculatorManager : public ICalculatorManager {
public:
    const CalculatorState& GetState() const override { return m_state; }
    void InputDigit(int digit) override;
    void InputOperator(char op) override;
    void Calculate() override;
    void Clear() override;

private:
    CalculatorState m_state;
    double m_currentValue = 0.0;
    char m_lastOp = 0;
};

}
