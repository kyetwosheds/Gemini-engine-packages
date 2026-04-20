#include "CalculatorManager.h"

namespace CalculatorWorkspace {

void CalculatorManager::InputDigit(int digit) {
    if (m_state.display == "0") m_state.display = std::to_string(digit);
    else m_state.display += std::to_string(digit);
}

void CalculatorManager::InputOperator(char op) {
    m_currentValue = std::stod(m_state.display);
    m_lastOp = op;
    m_state.display = "0";
}

void CalculatorManager::Calculate() {
    double val = std::stod(m_state.display);
    switch (m_lastOp) {
        case '+': m_currentValue += val; break;
        case '-': m_currentValue -= val; break;
        case '*': m_currentValue *= val; break;
        case '/': if (val != 0) m_currentValue /= val; break;
    }
    m_state.display = std::to_string(m_currentValue);
}

void CalculatorManager::Clear() {
    m_state.display = "0";
    m_currentValue = 0.0;
    m_lastOp = 0;
}

}
