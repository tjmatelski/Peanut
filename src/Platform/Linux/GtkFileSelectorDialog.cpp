#include "GtkFileSelectorDialog.h"

#include <gtkmm/main.h>
#include <gtkmm/window.h>
#include <gtkmm/application.h>
#include <gtkmm/filechooserdialog.h>

namespace PEANUT
{

std::unique_ptr<FileSelectorDialog> CreateFileSelectorDialog()
{
    return std::make_unique<GtkFileSelectorDialog>();
}

std::optional<std::string> GtkFileSelectorDialog::SelectFile()
{
    std::string m_selectedFile("");
    int argc = 0;
    char **argv;
    Gtk::Main kit(argc, argv);
    Gtk::Window client;
    {
      Gtk::FileChooserDialog dialog("Choose a file", Gtk::FILE_CHOOSER_ACTION_OPEN);
      dialog.add_button("Ok", Gtk::ResponseType::RESPONSE_OK);
      dialog.add_button("Cancel", Gtk::ResponseType::RESPONSE_CANCEL);
      dialog.signal_response().connect([&](int response) {
        switch (response)
        {
        case Gtk::ResponseType::RESPONSE_OK:
          m_selectedFile = dialog.get_file().get()->get_path();
          break;

        case Gtk::ResponseType::RESPONSE_CANCEL:
          break;

        default:
          break;
        }
      });
      dialog.run();
    }
    
    client.signal_show().connect([&] {
      client.close();
    });
    Gtk::Main::run(client);

    if (m_selectedFile.empty())
    {
      return {};
    }
    return m_selectedFile;
  }
}