#pragma once

#include <FileSelectorDialog.h>

namespace PEANUT
{

class GtkFileSelectorDialog : public FileSelectorDialog
{
public:
    virtual std::optional<std::string> SelectFile() override;
};

}