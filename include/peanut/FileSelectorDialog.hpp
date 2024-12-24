#pragma once

// stl
#include <memory>
#include <optional>
#include <string>

namespace PEANUT {

class FileSelectorDialog {
public:
    std::optional<std::string> OpenFile();
    std::optional<std::string> SaveFile();
    std::optional<std::string> OpenDirectory();
};

std::unique_ptr<FileSelectorDialog> CreateFileSelectorDialog();

}