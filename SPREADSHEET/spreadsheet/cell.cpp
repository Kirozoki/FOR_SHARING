#include "cell.h"

using namespace std::literals; 

// Реализуйте следующие методы
Cell::Cell(SheetInterface& sheet, Position pos) : sheet_(sheet), pos_(pos) 
{
}

Cell::~Cell() = default;

void Cell::Set(std::string text) {
    
    if (impl_ && impl_->GetText() == text) {
        return;
    }
    std::string prev_text = impl_? impl_->GetText(): ""s;
    if (impl_) {
    for (Position old_dep_pos : impl_->GetReferencedCells()) {
        Cell* dep_cell_ptr = const_cast<Cell*>(dynamic_cast<const Cell*>(sheet_.GetCell(old_dep_pos)));
        dep_cell_ptr->DelDepCell(pos_);
    }
    }

    if (text.empty()) {    
        impl_ = std::make_unique<EmptyImpl>();
        type_ = CellType::EMPTY;
    } else if (text[0] == FORMULA_SIGN && text.size() > 1) {
        impl_ = std::make_unique<FormulaImpl>(std::move(text));
        type_ = CellType::FORMULA;
        if (CyclicDependencies(this, impl_->GetReferencedCells())) {
            Set(prev_text);
            throw CircularDependencyException("Cycle connections");
        }
        for (Position ref_pos : impl_->GetReferencedCells()) {
            CellInterface* ref_pos_ptr = sheet_.GetCell(ref_pos);
            if (!ref_pos_ptr) {
                sheet_.SetCell(ref_pos, ""s);
            }
            Cell* cell_ref_ptr = const_cast<Cell*>(dynamic_cast<const Cell*>(sheet_.GetCell(ref_pos)));
            cell_ref_ptr->SetDepCell(pos_);
        }
        ClearCashe();

    } else {
        impl_ = std::make_unique<TextImpl>(std::move(text));
        type_ = CellType::TEXT;
    }
}

void Cell::Clear() {
    Set(""s);
}

bool Cell::HasCashe() const {
    return cashe_.has_value();
}
void Cell::SetCashe(double value) const {
    cashe_ = value;
}
void Cell::ClearCashe() const {
    cashe_.reset();

    if (!dependant_cells_.empty()) {
        for (const auto dep_pos : dependant_cells_) {
            reinterpret_cast<const Cell*>(sheet_.GetCell(dep_pos))->ClearCashe();
        }
    }
}

double Cell::GetCashe() const {
    if (HasCashe()) {
        const auto result = cashe_.value();
        if (result.index() == 0) {
            return std::get<double>(result);
        }
    }
    return 0.0;
}

CellType Cell::GetCellType() const {
    return type_;
}

CellInterface::Value Cell::GetValue() const {
    if (type_ != CellType::FORMULA) {
        return impl_->GetValue(sheet_);
    }
    
    if (HasCashe()) {
        return GetCashe();
    }

    const CellInterface::Value value = impl_->GetValue(sheet_);
    if (value.index() == 1) {
        SetCashe(std::get<1>(value));
        return GetCashe();
    }
    return std::get<FormulaError>(value);
}

std::string Cell::GetText() const {
    return impl_->GetText();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

Cell::FormulaImpl::FormulaImpl(std::string formula_txt) 
{
    std::string formula_exp = formula_txt.substr(1);
    formula_ = ParseFormula(formula_exp);
}

void Cell::SetDepCell(Position pos) {
    dependant_cells_.insert(pos);
}
void Cell::DelDepCell(Position pos) {
    dependant_cells_.erase(pos);
}

std::set<Position> Cell::GetDependentCells() {
    return dependant_cells_;
}

CellInterface::Value Cell::FormulaImpl::GetValue(const SheetInterface& sheet) const { 
    
    std::variant<double, FormulaError> value = formula_->Evaluate(sheet); // std::variant<double, FormulaError>;
    if (value.index() == 0) {
        return std::get<double>(value);
    }
    return std::get<FormulaError>(value);
}

std::string Cell::FormulaImpl::GetText() const {
    return std::string(FORMULA_SIGN + formula_->GetExpression());
}

std::vector<Position> Cell::FormulaImpl::GetReferencedCells() const {
    return formula_->GetReferencedCells();
}

bool Cell::IsReferenced() const {
    return !dependant_cells_.empty();
}

bool Cell::CyclicDependencies(Cell* head_cell_ptr, std::vector<Position> ref_cells) const { 
    if (ref_cells.empty()) {
        return false;
    }
    for (const Position& pos : ref_cells) {

        CellInterface* pos_cell_ptr = sheet_.GetCell(pos);
        if (pos_cell_ptr && (pos_cell_ptr == head_cell_ptr 
            || CyclicDependencies(head_cell_ptr, pos_cell_ptr->GetReferencedCells()))) {
            return true;
        }
    }
    return false;
}


