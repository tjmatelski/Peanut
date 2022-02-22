#pragma once

#include <FileSelectorDialog.h>
#include <iostream>

namespace PEANUT {

class GtkFileSelectorDialog : public FileSelectorDialog {
public:
    virtual ~GtkFileSelectorDialog() { }
    virtual std::optional<std::string> SelectFile() override;
};

}