#pragma once

#include <Peanut.h>

#include <filesystem>

namespace PEANUT
{

class FileSelectionPanel
{
public:
    FileSelectionPanel() : m_currentPath(std::getenv("PWD")) {}

    inline bool SelectedFile() const { return m_fileSelected; }
    inline std::string GetSelectedFile() const { return m_selectedFile; }

    template <typename Func>
    void Show(Func selectionFunc)
    {
        ImGui::Begin("Select a file");
        if (ImGui::Button("..") && m_currentPath.has_parent_path())
        {
            m_currentPath = m_currentPath.parent_path();
        }

        if (ImGui::Button("Ok") && m_selectedFile.has_filename())
        {
            selectionFunc(m_selectedFile);
        }

        for (const auto& file : std::filesystem::directory_iterator(m_currentPath))
        {
            if (ImGui::Selectable(file.path().c_str(), file.path() == m_selectedFile))
            {
                if (file.is_directory())
                {
                    m_currentPath = file.path();
                    break;
                }

                if (file.is_regular_file())
                {
                    m_selectedFile = file.path();
                    break;
                }
            }
        }
        ImGui::End();
    }

private:
    std::filesystem::path m_currentPath;
    std::filesystem::path m_selectedFile;
    bool m_fileSelected = false;
};

}