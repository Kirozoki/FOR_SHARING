#include "sheet.h"

#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        
        throw InvalidPositionException("pos is out of range");
    }
    
    auto it = cells_.find(pos);

    if (it == cells_.end()) {
            cells_[pos] = std::make_unique<Cell>(*this, pos);
    }
    cells_[pos]->Set(std::move(text));
  
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("pos is out of range");
    }

    if (cells_.find(pos) == cells_.end()) {
        return nullptr;
    }
    return cells_.at(pos).get();
}
CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("pos is out of range");
    }

    if (cells_.find(pos) == cells_.end()) {
        return nullptr;
    }
    return cells_.at(pos).get();
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("pos is out of range");
    }
    
    const auto& it_cell = cells_.find(pos);
    if (it_cell == cells_.end() || !it_cell->second) {
        return;
    }
    if (it_cell->second->GetCellType() == CellType::FORMULA) {
        //Reset cashe for dependant cells 
        for (const Position& dep_pos : cells_.at(pos)->GetDependentCells()) {
            cells_.at(dep_pos)->ClearCashe();
        }
        //Delete dependency for referenced cells 
        for (const Position& ref_pos : it_cell->second->GetReferencedCells()) {
            cells_.at(ref_pos)->DelDepCell(pos);
        }
    }
    it_cell->second->Clear();
    if (!it_cell->second->IsReferenced()) {
        cells_.erase(pos);
    }
}

Size Sheet::GetPrintableSize() const {
    Size result{0, 0};
    if (cells_.empty()) {
        return result;}
    for (const auto& [pos, cell_uptr] : cells_) {
        if (cells_.at(pos)->GetCellType() != CellType::EMPTY) {
        result.rows = std::max(result.rows, pos.row + 1);
        result.cols = std::max(result.cols, pos.col + 1);
        }
    }
    return result;
}

void Sheet::PrintValues(std::ostream& output) const {
    const auto [max_rows, max_cols] = GetPrintableSize();
    for (int i = 0; i < max_rows; ++i) {
        for (int j = 0; j < max_cols; ++j) {
            if (j != 0) {
                output << '\t';
            }
            Position pos{i, j};
            const auto it = cells_.find(pos);
            if (it == cells_.end() || it->second->GetCellType() == CellType::EMPTY) {
                output << ""s;
            } else {
                std::variant<std::string, double, FormulaError> value = 
                                            cells_.at(pos)->GetValue();
                if (value.index() == 0) {
                    output << std::get<std::string>(value);
                } else if (value.index() == 1) {
                    output << std::get<double>(value);
                } else {
                    output << std::get<FormulaError>(value);
                }
            }
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    const auto [max_rows, max_cols] = GetPrintableSize();
    for (int i = 0; i < max_rows; ++i) {
        for (int j = 0; j < max_cols; ++j) {
            if (j != 0) {
                output << '\t';
            }
            Position pos{i, j};
            const auto it = cells_.find(pos);
            if (it == cells_.end() || it->second->GetCellType() == CellType::EMPTY) {
                output << ""s;
            } else {
                output << cells_.at(pos)->GetText();
            }
        }
        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}