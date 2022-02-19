#include "GtkFileSelectorDialog.h"

#include <Log.h>
#include <gtkmm/application.h>
#include <gtkmm/filechooserdialog.h>

namespace PEANUT {

std::unique_ptr<FileSelectorDialog> CreateFileSelectorDialog()
{
    return std::make_unique<GtkFileSelectorDialog>();
}

std::optional<std::string> GtkFileSelectorDialog::SelectFile()
{
    int argc = 0;
    char** argv {};
    std::string chosenFile("");

    auto app = Gtk::Application::create("org.gtkmm.examples.base");
    Gtk::FileChooserDialog dialog("Choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.add_button("Ok", Gtk::ResponseType::RESPONSE_OK);
    dialog.add_button("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
    dialog.signal_response().connect([&](int response) {
        switch (response) {
        case Gtk::ResponseType::RESPONSE_OK:
            chosenFile = dialog.get_file().get()->get_path();
            break;

        case Gtk::ResponseType::RESPONSE_CANCEL:
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