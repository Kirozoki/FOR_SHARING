#pragma once

#include "common.h"
#include "formula.h"

#include <optional>
#include <set>

using namespace std::literals;

enum class CellType {
    EMPTY,
    TEXT,
    FORMULA
};

class Cell : public CellInterface {
public:
    explicit Cell(SheetInterface& sheet, Position pos);
    ~Cell();

    void Set(std::string text);
    void Clear();
    bool HasCashe() const;
    void SetCashe(double value) const;
    void ClearCashe() const;
    double GetCashe() const;

    CellType GetCellType() const;

    void SetDepCell(Position pos);
    void DelDepCell(Position pos);
    std::set<Position> GetDependentCells();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    bool IsReferenced() const;

private:
    class Impl
    {
    public:
        using Value = CellInterface::Value;
        Impl() = default;
        virtual ~Impl() = default; 
        virtual Value GetValue(const SheetInterface&) const = 0;
        virtual std::string GetText() const = 0;
        virtual std::vector<Position> GetReferencedCells() const {
            return {};
        }
    };

    class EmptyImpl : public Impl {
    public:
        explicit EmptyImpl() :
        Impl(){}
        Value GetValue(const SheetInterface&) const override {
            return ""s;
        }
        std::string GetText() const override {
            return ""s;
        }
    };

    class TextImpl : public Impl {
    public:
        explicit TextImpl(std::string str) :
        Impl(), str_(str) 
        {}
        Value GetValue(const SheetInterface&) const override {
            if (str_[0] == ESCAPE_SIGN) {
                return str_.substr(1);
            }
            return str_;
        }
        std::string GetText() const override {
            return str_;
        }
    private:
        std::string str_;
    };

    class FormulaImpl : public Impl {
    public:
        explicit FormulaImpl(std::string formula_txt);
        Value GetValue(const SheetInterface& sheet) const override ;
        std::string GetText() const override ;
        std::vector<Position> GetReferencedCells() const override;
    private:
        std::unique_ptr<FormulaInterface> formula_;
};


    std::unique_ptr<Impl> impl_;

    // Добавьте поля и методы для связи с таблицей, проверки циклических 
    // зависимостей, графа зависимостей и т. д.
    SheetInterface& sheet_;
    Position pos_;
    mutable std::optional<FormulaInterface::Value> cashe_;
    std::set<Position> dependant_cells_;
    CellType type_;

    bool CyclicDependencies(Cell* cell_ptr, std::vector<Position> ref_cells) const ;

}; // added, to check

