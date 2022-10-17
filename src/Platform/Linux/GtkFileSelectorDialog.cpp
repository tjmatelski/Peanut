#include "GtkFileSelectorDialog.h"

#include <Log.h>
#include <gtkmm/application.h>
#include <gtkmm/filechooser.h>
#include <gtkmm/filechooserdialog.h>
#include <optional>
#include <string>

namespace {
std::optional<std::string> SelectFile(Gtk::FileChooser::Action action)
{
    int argc = 0;
    char** argv {};
    std::string chosenFile("");

    auto app = Gtk::Application::create("org.gtkmm.examples.base");
    int i = app->make_window_and_run<Gtk::FileChooserDialog>(argc, argv, "Choose a file", action);
    Gtk::FileChooserDialog* dialog = dynamic_cast<Gtk::FileChooserDialog*>(app->get_run_window());
    dialog->add_button("Ok", GTK_RESPONSE_OK);
    dialog->add_button("Cancel", GTK_RESPONSE_CANCEL);
    dialog->signal_response().connect([&](int response) {
        switch (response) {
        case GTK_RESPONSE_OK:
            chosenFile = dialog->get_file().get()->get_path();
            break;

        default:
            break;
        }
        app->get_active_window()->hide();
    });
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
    return SelectFile(Gtk::FileChooser::Action::OPEN);
}

std::optional<std::string> GtkFileSelectorDialog::SaveFile()
{
    return SelectFile(Gtk::FileChooser::Action::SAVE);
}

}