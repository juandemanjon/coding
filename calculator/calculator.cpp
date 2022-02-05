#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <vector>
#include <stack>
#include <iostream>

struct Token
{
    char Operator = 0;

    int Number = 0;
};

using RPNVector = std::vector<Token>;


RPNVector RPNTokenize(std::string_view input)
{
    RPNVector output;
    
    return output;
}

double Compute(double value1, double value2, char ope)
{
    switch (ope) {
        case '+':
            return value1 + value2;
        case '-':
            return value1 - value2;
        case '*':
            return value1 * value2;
        case '/':
            return value1 / value2;            
        default: // Avoid warnings
            return 0;
    }
    
    // Non reachable code
    return 0;
}


double Compute(const RPNVector &input)
{
    std::stack<double> values;
        
    for (const Token &token: input)
    {
        if (token.Operator) {
            auto value2 = values.top();
            values.pop();
            auto value1 = values.top();
            values.pop();
            
            values.push(Compute(value1, value2, token.Operator));
            
        } else {
            values.push(token.Number);
        }
    }
    return values.top();
}

TEST_CASE("Parse expressions")
{
    
}

struct RPN_Result
{
    RPNVector rpn;
    
    double expected;
};

TEST_CASE("Compute RPNs")
{
    std::vector<RPN_Result> test_cases(
        {
            { {{0, 1},{0, 2},{'+',0},},  3.0, },
            { {{0, 1},{0, 2},{'-',0},}, -1.0, },
            { {{0, 1},{0, 2},{'*',0},},  2.0, },
            { {{0, 1},{0, 2},{'/',0},},  0.5, },
            
            { {{0, 1},{0, 2},{'+',0},{0, 3},{'+',0},},  6.0, },
            { {{0, 1},{0, 2},{'+',0},{0, 3},{'-',0},},  0.0, },
            { {{0, 1},{0, 2},{'+',0},{0, 3},{'*',0},},  9.0, },
            { {{0, 1},{0, 2},{'+',0},{0, 3},{'/',0},},  1.0, },
        }
    );       

    for (const auto &tc : test_cases) {
        REQUIRE(Compute(tc.rpn) == tc.expected);
    }
}