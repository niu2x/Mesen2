#include "menu.h"
#include "../ImGuiFileDialog/ImGuiFileDialog.h"
#include "imgui.h"

void imgui_menu() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "Ctrl+O")) {
                IGFD::FileDialogConfig config;
                config.path = ".";
                ImGuiFileDialog::Instance()->OpenDialog(
                    "ChooseFileDlgKey", "Choose File", ".nes", config);
            }

            if (ImGui::MenuItem("Save", "Ctrl+S")) {
                // 处理保存操作
            }
            ImGui::Separator();

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }

    if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey")) {
        if (ImGuiFileDialog::Instance()->IsOk()) { // action if OK
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            std::string filePath = ImGuiFileDialog::Instance()->GetCurrentPath();
            // action
        }
        // close
        ImGuiFileDialog::Instance()->Close();
    }
}
