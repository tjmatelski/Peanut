#include "GtkFileSelectorDialog.h"

#include <Log.h>
#include <gtkmm/application.h>
#include <gtkmm/filechooser.h>
#include <gtkmm/filechooserdialog.h>
#include <optional>
#include <string>

namespace {
std::optional<std::string> SelectFile(Gtk::FileChooserAction action)
{
    int argc = 0;
    char** argv {};
    std::string chosenFile("");

    auto app = Gtk::Application::create("org.gtkmm.examples.base");
    Gtk::FileChooserDialog dialog("Choose a file", action);
    dialog.add_button("Ok", Gtk::ResponseType::RESPONSE_OK);
    dialog.add_button("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
    dialog.signal_response().connect([&](int response) {
        switch (response) {
        case Gtk::ResponseType::RESPONSE_OK:
            chosenFile = dialog.get_file().get()->get_path();
            break;

        default:
            break;
        }
        app->get_active_window()->hide();
    });
    int i = app->run(dialog, argc, argv);
    if (i != 0) {
        LOG_ERROR("Error Selecting file");
    }
    app->quit();

    if (chosenFile.empty()) {
        return {};
    }
    return chosenFile;
}
}
namespace PEANUT {

std::unique_ptr<FileSelectorDialog> CreateFileSelectorDialog()
{
    return std::make_unique<GtkFileSelectorDialog>();
}

std::optional<std::string> GtkFileSelectorDialog::OpenFile()
{
    return SelectFile(Gtk::FILE_CHOOSER_ACTION_OPEN);
}

std::optional<std::string> GtkFileSelectorDialog::SaveFile()
{
    return SelectFile(Gtk::FILE_CHOOSER_ACTION_SAVE);
}

std::optional<std::string> GtkFileSelectorDialog::OpenDirectory()
{
    return SelectFile(Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
}

}