#pragma once

#include <FileSelectorDialog.h>
#include <iostream>
#include <optional>
#include <string>

namespace PEANUT {

class GtkFileSelectorDialog : public FileSelectorDialog {
public:
    virtual ~GtkFileSelectorDialog() = default;
    std::optional<std::string> OpenFile() override;
    std::optional<std::string> SaveFile() override;
    std::optional<std::string> OpenDirectory() override;
};

}