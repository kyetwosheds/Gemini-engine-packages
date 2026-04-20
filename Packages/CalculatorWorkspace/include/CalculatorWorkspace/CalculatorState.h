#pragma once

#include "../../HostInterfaces.h"
#include <string>
#include <vector>

namespace CalculatorWorkspace {

struct CalculatorState {
    std::string display = "0";
    double memory = 0.0;
};

class ICalculatorManager {
public:
    virtual ~ICalculatorManager() = default;
    virtual const CalculatorState& GetState() const = 0;
    virtual void InputDigit(int digit) = 0;
    virtual void InputOperator(char op) = 0;
    virtual void Calculate() = 0;
    virtual void Clear() = 0;
};

}
