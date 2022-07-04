//
//  main.cpp
//  interpreter-pattern
//
//

#include <iostream>
#include <string>

class Expression {
public:
    virtual int evaluate() = 0;
};

class OperationExpression : public Expression {
    std::string operatorSymbol;
    Expression* leftHandSide;
    Expression* rightHandSide;

public:
    OperationExpression(const std::string& operatorSymbol, Expression* lhs, Expression* rhs) : operatorSymbol(operatorSymbol), leftHandSide(lhs), rightHandSide(rhs) {}
    int evaluate() override {
        if (operatorSymbol == "+") {
            return leftHandSide->evaluate() + rightHandSide->evaluate();
        }
        else if (operatorSymbol == "-") {
            return leftHandSide->evaluate() - rightHandSide->evaluate();
        }
        else {
            std::cout << "Unrecognized operator: " << operatorSymbol;
            return 0;
        }
    }
};

class NumberExpression : public Expression {
    std::string numberString;
public:
    NumberExpression(const std::string& numberString) : numberString(numberString) {}
    int evaluate() override {
        return std::stoi(numberString);
    }
};


int main(int argc, const char * argv[]) {
    NumberExpression* five = new NumberExpression("5");
    NumberExpression* seven = new NumberExpression("7");
    OperationExpression* fivePlusSeven = new OperationExpression("+", five, seven);

    NumberExpression* thirteen = new NumberExpression("13");
    OperationExpression* complexOp = new OperationExpression("-", thirteen, fivePlusSeven);
    std::cout << "Five plus seven is: " << fivePlusSeven->evaluate() << std::endl;
    std::cout << "13 - (5 + 7) is " << complexOp->evaluate() << std::endl;
    delete five;
    delete seven;
    delete fivePlusSeven;
    return 0;
}
