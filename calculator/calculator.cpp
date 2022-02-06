#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <vector>
#include <stack>
#include <iostream>
#include <queue>

struct Token
{
    Token(char o) : Operator(o) {}
    Token(int n) : Number(n) {}

    char Operator = 0;

    int Number = 0;

    bool operator!=(const Token &that) const { return !operator==(that); }

    bool operator==(const Token &that) const
    {
        return Operator == that.Operator && Number == that.Number;
    }
};

std::ostream &operator<<(std::ostream &output, const Token &token)
{
    if (token.Operator)
        output << '\'' << token.Operator << '\'';
    else
        output << token.Number;
    return output;
}

using TokenVector = std::vector<Token>;

std::ostream &operator<<(std::ostream &output, const TokenVector &tokens)
{
    output << '{';
    for (const Token &token : tokens)
    {
        output << token << ',';
    }
    output << '}';
    return output;
}

TokenVector Parse(std::string_view input)
{
    TokenVector output;

    std::string number;

    for (char c : input)
    {
        switch (c)
        {
            case ' ':
                if (!number.empty())
                {
                    output.push_back(atoi(number.c_str()));
                    number.clear();
                }
                break;
            case '+':
            case '-':
            case '*':
            case '/':
            case '(':
            case ')':
                if (!number.empty())
                {
                    output.push_back(atoi(number.c_str()));
                    number.clear();
                }
                output.push_back(c);
                break;
            default:
                number += c;
        }
    }
    if (!number.empty())
    {
        output.push_back(atoi(number.c_str()));
    }

    return output;
}

int priority(const Token &token)
{
    switch (token.Operator)
    {
        case '+':
        case '-':
            return 1;
        case '*':
        case '/':
            return 2;
        default:  // Avoid warnings
            return 0;
    }
    return 0;
}

TokenVector RPN(const TokenVector &input)
{
    TokenVector output;

    std::stack<Token> operators;

    for (const auto &token : input)
    {
        if (!token.Operator)
        {
            output.push_back(token);
        }
        else
        {
            if (operators.empty() || token.Operator == '(')
            {
                operators.push(token);
            }
            else if (token.Operator == ')')
            {
                while (!operators.empty() && operators.top().Operator != '(')
                {
                    output.push_back(operators.top());
                    operators.pop();
                }
                if (!operators.empty())
                {
                    operators.pop();  // remove '('
                }
            }
            else
            {
                auto token_priority = priority(token);
                while (!operators.empty() && priority(operators.top()) > token_priority)
                {
                    output.push_back(operators.top());
                    operators.pop();
                }
                operators.push(token);
            }
        }
    }

    while (!operators.empty())
    {
        output.push_back(operators.top());
        operators.pop();
    }

    return output;
}

double Compute(double value1, double value2, char ope)
{
    switch (ope)
    {
        case '+':
            return value1 + value2;
        case '-':
            return value1 - value2;
        case '*':
            return value1 * value2;
        case '/':
            return value1 / value2;
        default:  // Avoid warnings
            return 0;
    }

    // Non reachable code
    return 0;
}

double Compute(const TokenVector &input)
{
    std::stack<double> values;

    for (const Token &token : input)
    {
        if (token.Operator)
        {
            // Get values. TODO: validate stack is not empty before calling top()
            auto value2 = values.top();
            values.pop();
            auto value1 = values.top();
            values.pop();

            values.push(Compute(value1, value2, token.Operator));
        }
        else
        {
            values.push(token.Number);
        }
    }
    return values.top();
}

TEST_CASE("Parse expressions")
{
    struct TestCase
    {
        std::string_view exp;

        TokenVector expected;
    };

    std::vector<TestCase> test_cases(
        {
            {"1 + 2",   {1,'+',2,}},
            {"1 - 2",   {1,'-',2,}},
            {"1 * 2",   {1,'*',2,}},
            {"1 / 2",   {1,'/',2,}},

            {"1 + 2 + 3",   {1,'+',2,'+',3,}},
            {"1 + 2 - 3",   {1,'+',2,'-',3,}},
            {"1 + 2 * 3",   {1,'+',2,'*',3,}},
            {"1 + 2 / 3",   {1,'+',2,'/',3,}},
            {"(1 + 2) * 3",   {'(',1,'+',2,')','*',3,}},
            {"(1 + 2) / 3",   {'(',1,'+',2,')','/',3,}},
        }
    );
    
    for (const auto &tc: test_cases) {
        auto got = Parse(tc.exp);

        REQUIRE(got.size() == tc.expected.size());
        for (size_t i = 0; i < got.size(); ++i) {
            CHECK(got[i] == tc.expected[i]);
        }
    }
}

TEST_CASE("RPN tokens")
{
    struct TestCase
    {
        TokenVector tokens;

        TokenVector expected;
    };
    std::vector<TestCase> test_cases(
        {
            { {1,'+',2,},                {1,2,'+',}       },
            { {1,'+',2,'*',3,},          {1,2,3,'*','+',} },
            { {'(',1,'+',2,')','*',3,},  {1,2,'+',3,'*',} },
        }
    );
        
    for (const auto &tc : test_cases) {
        auto got = RPN(tc.tokens);
        INFO("got " << got << '\n');
        REQUIRE(got.size() == tc.expected.size());
        for (size_t i = 0; i < got.size(); ++i) {
            CHECK(got[i] == tc.expected[i]);
        }
    }
}

TEST_CASE("Compute RPNs")
{
    struct TestCase
    {
        TokenVector rpn;

        double expected;
    };

    std::vector<TestCase> test_cases(
        {
            { {1,2,'+',},  3.0, },
            { {1,2,'-',}, -1.0, },
            { {1,2,'*',},  2.0, },
            { {1,2,'/',},  0.5, },
            
            { {1,2,'+',3,'+',},  6.0, },
            { {1,2,'+',3,'-',},  0.0, },
            { {1,2,'+',3,'*',},  9.0, },
            { {1,2,'+',3,'/',},  1.0, },
            
            { {1,2,3,'*','+',},  7.0, },
        }
    );        

    for (const auto &tc : test_cases) {
        INFO(tc.rpn);
        REQUIRE(Compute(tc.rpn) == tc.expected);
    }
}

TEST_CASE("end to end")
{
    struct TestCase
    {
        std::string_view exp;

        double expected;
    };

    std::vector<TestCase> test_cases(
        {
            {"1 + 2",   3},
            {"1 - 2",   -1},
            {"1 * 2",   2},
            {"1 / 2",   0.5},

            {"1 + 2 + 3",   6},
            {"1 + 2 - 3",   0},
            {"1 + 2 * 3",   7},
            {"1 + 2 / 4",   1.5},
            {"1 + (2 * 3)",   7},
            {"1 + (2 / 4)",   1.5},
            {"(1 + 2) * 3",   9},
            {"(1 + 2) / 3",   1},
        }
    );
    
    for (const auto &tc: test_cases) {
        auto got = Compute(RPN(Parse(tc.exp)));
        INFO(tc.exp);
        REQUIRE(got == tc.expected);
    }
}

struct ICharDataSource
{
    virtual bool HasNext() = 0;
    virtual char GetNext() = 0;
};

struct StringExpression : ICharDataSource
{
    StringExpression(std::string_view input) : _input(input), _it(_input.begin()) { ; }
    
    bool HasNext() override { return _it != _input.end(); }

    char GetNext() override { return *_it++; }
    
private:
    std::string_view _input;
    std::string_view::const_iterator _it;   
};

TEST_CASE("StringExpression")
{
    // GIVEN an expression with "123"
    StringExpression se{"123"};
    
    // THEN data '1' is available 
    REQUIRE(se.HasNext());
    REQUIRE(se.GetNext() == '1');

    // AND data '2' is available 
    REQUIRE(se.HasNext());
    REQUIRE(se.GetNext() == '2');

    // AND data '3' is available 
    REQUIRE(se.HasNext());
    REQUIRE(se.GetNext() == '3');

    // AND no more data is available
    REQUIRE(!se.HasNext());
}

class Expression2RPN
{
  public:
    Expression2RPN(ICharDataSource &exp) : _exp(exp)
    { 
        readTokenFromCharDataSource();
        readTokenFromExpQueue();
    }

    bool HasNext() { return !_rpnQueue.empty(); }
    
    Token GetNext() 
    { 
        auto token = _rpnQueue.front();
        _rpnQueue.pop();
        readTokenFromExpQueue();
        return token; 
    }

  private:

    bool hasExpNext() { return !_expQueue.empty(); }
    
    Token getExpNext() 
    { 
        auto token = _expQueue.front();
        _expQueue.pop();
        readTokenFromCharDataSource();
        return token; 
    }
    
    ICharDataSource &_exp;
    
    void readTokenFromCharDataSource()
    {
        std::string number;
        
        while (_expQueue.empty() && _exp.HasNext())
        {
            auto c = _exp.GetNext();
            switch (c)
            {
                case ' ':
                    if (!number.empty())
                    {
                        _expQueue.push(Token(atoi(number.c_str())));
                        return;
                    }
                    break;
                case '+':
                case '-':
                case '*':
                case '/':
                case '(':
                case ')':
                    if (!number.empty())
                    {
                        _expQueue.push(Token(atoi(number.c_str())));
                    }
                    _expQueue.push(Token(c));
                    return;
                default:
                    number += c;
            }           
        }

        if (!number.empty())
        {
            _expQueue.push(Token(atoi(number.c_str())));
        }
    }

    void readTokenFromExpQueue()
    {
        while (_rpnQueue.empty() && hasExpNext())
        {
            auto token = getExpNext();
            if (!token.Operator)
            {
                _rpnQueue.push(token);
                return;
            }
            else
            {
                if (operators.empty() || token.Operator == '(')
                {
                    operators.push(token);
                }
                else if (token.Operator == ')')
                {
                    while (!operators.empty() && operators.top().Operator != '(')
                    {
                        _rpnQueue.push(operators.top());
                        operators.pop();
                    }
                    if (!operators.empty())
                    {
                        operators.pop();  // remove '('
                    }
                    if (!_rpnQueue.empty())
                        return;
                }
                else
                {
                    auto token_priority = priority(token);
                    while (!operators.empty() && priority(operators.top()) > token_priority)
                    {
                        _rpnQueue.push(operators.top());
                        operators.pop();
                    }
                    operators.push(token);
                    if (!_rpnQueue.empty())
                        return;
                }
            }
        }

        if (_rpnQueue.empty() && !hasExpNext() && !operators.empty())
        {
            _rpnQueue.push(operators.top());
            operators.pop();
        }
    }
    
    std::queue<Token> _expQueue;
    std::queue<Token> _rpnQueue;

    std::stack<Token> operators;    
};

TEST_CASE("Expression2RPN")
{
    StringExpression exp{" (123 + 456) + 789 "};
    
    Expression2RPN parser{exp};
    
    REQUIRE(parser.HasNext());
    REQUIRE(parser.GetNext() == Token{123});

    REQUIRE(parser.HasNext());
    REQUIRE(parser.GetNext() == Token{456});

    REQUIRE(parser.HasNext());
    REQUIRE(parser.GetNext() == Token{'+'});

    REQUIRE(parser.HasNext());
    REQUIRE(parser.GetNext() == Token{789});

    REQUIRE(parser.HasNext());
    REQUIRE(parser.GetNext() == Token{'+'});

    REQUIRE(!parser.HasNext());
}

double Compute(Expression2RPN &input)
{
    std::stack<double> values;

    while (input.HasNext())
    {
        auto token = input.GetNext();

        if (token.Operator)
        {
            // Get values. TODO: validate stack is not empty before calling top()
            auto value2 = values.top();
            values.pop();
            auto value1 = values.top();
            values.pop();

            values.push(Compute(value1, value2, token.Operator));
        }
        else
        {
            values.push(token.Number);
        }
    }
    return values.top();
}

TEST_CASE("end to end Expression2RPN")
{
    struct TestCase
    {
        std::string_view exp;

        double expected;
    };

    std::vector<TestCase> test_cases(
        {
            {"1 + 2",   3},
            {"1 - 2",   -1},
            {"1 * 2",   2},
            {"1 / 2",   0.5},

            {"1 + 2 + 3",   6},
            {"1 + 2 - 3",   0},
            {"1 + 2 * 3",   7},
            {"1 + 2 / 4",   1.5},
            {"1 + (2 * 3)",   7},
            {"1 + (2 / 4)",   1.5},
            {"(1 + 2) * 3",   9},
            {"(1 + 2) / 3",   1},
        }
    );
    
    for (const auto &tc: test_cases) {
        StringExpression exp{tc.exp};       
        Expression2RPN expRPN{exp};

        auto got = Compute(expRPN);
        INFO(tc.exp);
        REQUIRE(got == tc.expected);
    }
}


