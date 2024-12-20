#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <unordered_map>
#include <map>

struct PosHasher
{
    size_t operator() (Position pos) const {
        return pos.row * 43 + pos.col * 43 * 43; 
    }
};

class Sheet : public SheetInterface {
public:
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

	// Можете дополнить ваш класс нужными полями и методами

private:
	std::unordered_map<Position, std::unique_ptr<Cell>, PosHasher> cells_;
};