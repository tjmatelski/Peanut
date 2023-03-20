#pragma once

#include <memory>
#include <optional>
#include <string>

namespace PEANUT {

class FileSelectorDialog {
public:
    virtual ~FileSelectorDialog() = default;
    virtual std::optional<std::string> OpenFile() = 0;
    virtual std::optional<std::string> SaveFile() = 0;
    virtual std::optional<std::string> OpenDirectory() = 0;
};

std::unique_ptr<FileSelectorDialog> CreateFileSelectorDialog();

}