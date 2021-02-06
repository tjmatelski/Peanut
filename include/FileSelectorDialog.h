#pragma once

#include <memory>
#include <optional>
#include <string>

namespace PEANUT
{

class FileSelectorDialog
{
public:
    virtual std::optional<std::string> SelectFile() = 0;
};

std::unique_ptr<FileSelectorDialog> CreateFileSelectorDialog();

}