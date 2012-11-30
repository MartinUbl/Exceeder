#ifndef EXCDR_EXPRESSION_PARSER_H
#define EXCDR_EXPRESSION_PARSER_H

enum MathOperation
{
    OP_ADD      = 0,    // addition (includes subtracting as addition of negative value)
    OP_MULTIPLY = 1,    // multiplication
    OP_DIVIDE   = 2,    // division
    OP_MODULO   = 3,    // modulo
    OP_MAX
};

enum ValueType
{
    VT_EXPRESSION = 0,  // another expression inside
    VT_INTEGER    = 1,  // integer type (cast as long)
    VT_FLOAT      = 2,  // floating point value (cast as double)
    VT_STRING     = 3,  // string value (cast as char* )
    VT_MAX
};

// not derived from Parser class since it does not work with files, only with strings

class ExprVector;
typedef ExprVector ExpressionVector;

struct ExprTreeElem;
typedef ExprTreeElem ExpressionTreeElement;

struct ExprTreeElem
{
    union valueUnion
    {
        double    asDouble;
        long      asLong;
        wchar_t*  asString;
    };

    ExprTreeElem()
    {
        polarity = true;
        valueType = VT_EXPRESSION;
    }
    ExprTreeElem(bool p_polarity, ValueType p_valueType, valueUnion p_val)
    {
        polarity = p_polarity;
        valueType = p_valueType;
        switch (valueType)
        {
            case VT_INTEGER:
                value.asLong = p_val.asLong;
                break;
            case VT_FLOAT:
                value.asDouble = p_val.asDouble;
                break;
            case VT_STRING:
                value.asString = p_val.asString;
                break;
            case VT_EXPRESSION:
            default:
                break;
        }
    }

    bool polarity; // positive (true) / negative (false)

    ValueType valueType;
    union valueUnion value;
    std::vector<ExpressionTreeElement*> items;

    // in case of expression type, there has to be an mathematic operation defined
    MathOperation operation;
};

class ExpressionParser
{
    public:
        enum Entity
        {
            EN_INVALID   = 0, // start/end of expression
            EN_VALUE     = 1,
            EN_PLUS      = 2,
            EN_MINUS     = 3,
            EN_MULT      = 4,
            EN_DIV       = 5,
            EN_MODULO    = 6,
            EN_LEFT_PAR  = 7,
            EN_RIGHT_PAR = 8,
            EN_UNDEFINED
        };
        enum EntityCathegory
        {
            EC_VALUE       = 0,
            EC_OPERATOR    = 1,
            EC_PARENTHESIS = 2,
            EC_MAX
        };

        static Entity charEntity(wchar_t input)
        {
            switch (input)
            {
                case L'+': return EN_PLUS;
                case L'-': return EN_MINUS;
                case L'*': return EN_MULT;
                case L'/': return EN_DIV;
                case L'%': return EN_MODULO;
                case L'(': return EN_LEFT_PAR;
                case L')': return EN_RIGHT_PAR;
                default:
                    return EN_UNDEFINED;
            }
        }
        static wchar_t entityChar(Entity input)
        {
            switch (input)
            {
                case EN_PLUS:      return L'+';
                case EN_MINUS:     return L'-';
                case EN_MULT:      return L'*';
                case EN_DIV:       return L'/';
                case EN_MODULO:    return L'%';
                case EN_LEFT_PAR:  return L'(';
                case EN_RIGHT_PAR: return L')';
                default:
                    return 0;
            }
        }
        static EntityCathegory entityCathegory(Entity input)
        {
            switch (input)
            {
                case EN_VALUE:
                    return EC_VALUE;
                case EN_PLUS:
                case EN_MINUS:
                case EN_MULT:
                case EN_DIV:
                case EN_MODULO:
                    return EC_OPERATOR;
                case EN_LEFT_PAR:
                case EN_RIGHT_PAR:
                    return EC_PARENTHESIS;
            }
            return EC_VALUE;
        }

        static ExpressionVector* Parse(const wchar_t* input);
        static bool CheckSyntax(ExpressionVector* input);
        static ExpressionTreeElement* BuildTree(ExpressionVector* input, uint32 start, uint32 count);

        static ExpressionTreeElement* BuildValueElement(const wchar_t* input);
};

class ExprVector: public std::vector<ExpressionParser::Entity>
{
    public:
        ExprVector(): std::vector<ExpressionParser::Entity>()
        {
            pos = 0;
        }

        ExpressionParser::Entity prev()
        {
            if (pos == 0)
                return ExpressionParser::EN_INVALID;

            return (*this)[--pos];
        }

        ExpressionParser::Entity actual()
        {
            if (size() == 0)
                return ExpressionParser::EN_INVALID;

            return (*this)[pos];
        }

        ExpressionParser::Entity next()
        {
            if (pos >= size())
                return ExpressionParser::EN_INVALID;

            return (*this)[++pos];
        }

        void push(ExpressionParser::Entity ent)
        {
            push_back(ent);
            m_valueVector.push_back(NULL);
        }

        void push(ExpressionParser::Entity ent, const wchar_t* val)
        {
            push_back(ent);
            m_valueVector.push_back(new std::wstring(val));
        }

        void rewind()
        {
            pos = 0;
        }

        const wchar_t* getValue()
        {
            if (size() == 0 || pos >= m_valueVector.size() || (*this)[pos] != ExpressionParser::EN_VALUE)
                return NULL;

            return m_valueVector[pos]->c_str();
        }
        const wchar_t* getPrevValue()
        {
            if (pos == 0 || size() == 0 || pos >= m_valueVector.size() || (*this)[pos-1] != ExpressionParser::EN_VALUE)
                return NULL;

            return m_valueVector[pos-1]->c_str();
        }
        const wchar_t* getNextValue()
        {
            if (size() == 0 || pos+1 >= size() || pos+1 >= m_valueVector.size() || (*this)[pos+1] != ExpressionParser::EN_VALUE)
                return NULL;

            return m_valueVector[pos+1]->c_str();
        }

        const wchar_t* getValue(uint32 offset)
        {
            if (offset >= size() || offset >= m_valueVector.size() || (*this)[offset] != ExpressionParser::EN_VALUE)
                return NULL;

            return m_valueVector[offset]->c_str();
        }

    private:
        uint32 pos;
        std::vector<std::wstring*> m_valueVector;
};

#endif
