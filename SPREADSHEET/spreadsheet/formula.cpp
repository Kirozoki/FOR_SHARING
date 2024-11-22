#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

FormulaError::FormulaError(Category category) 
    : category_(category) {}

FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

std::string_view FormulaError::ToString() const {
    switch (category_) {
        case Category::Ref:
            return "#REF!";
        case Category::Value:
            return "#VALUE!";
        case Category::Arithmetic:
            return "#ARITHM!";
        }
    return "";
}

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {
class Formula : public FormulaInterface {
public:
// Реализуйте следующие методы:
    explicit Formula(std::string expression) try :
        ast_(ParseFormulaAST(expression)) {
        } catch (const std::exception& exc) {
            throw FormulaException(exc.what());
        }
    Value Evaluate(const SheetInterface& sheet) const override {
        const std::function<double(Position)> args = 
            [&sheet](const Position& pos)->double{
                if (!pos.IsValid()) {
                    throw FormulaError(FormulaError::Category::Ref);
                }
                auto cell_ptr = sheet.GetCell(pos);
                if (!cell_ptr) {
                    return 0.0;
                }
                CellInterface::Value result = cell_ptr->GetValue();
                if (result.index() == 0) {
                    std::string text = std::get<std::string>(result);
                    if (text.empty()) {
                        return 0.0;
                    }
                    try {
                        double double_val = std::stod(text);
                        std::ostringstream out;
                        out << std::noshowpoint << double_val;
                        if (out.str().size() == text.size()) {
                        return double_val;}
                        
                    } catch (...) {
                        throw FormulaError(FormulaError::Category::Value);
                    }
                    throw FormulaError(FormulaError::Category::Value);
                } else if (result.index() == 1) {
                    return std::get<double>(result);
                }
                throw FormulaError(std::get<FormulaError>(result));
            };

        try
        {
            return ast_.Execute(args);
        }
        catch(FormulaError& e)
        {
            return e;
        }
        
    }
    std::string GetExpression() const override {
        std::ostringstream out;
        ast_.PrintFormula(out);
        return out.str();
    }

    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> result = ast_.GetReferencedCells();
        return result;
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
   try {
        return std::make_unique<Formula>(std::move(expression));
    }
    catch (...) {
        throw FormulaException("Formula parsing error");
    } 
}