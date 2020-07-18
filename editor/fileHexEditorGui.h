#pragma once

#include <imgui.h>
#include <ImGuiFileBrowser.h>
#include <imgui_memory_editor/imgui_memory_editor.h>
#include <vector>

#include "util.h"

namespace editor::fileHexEditor
{
  bool openLoadHexEditorFileDialog = false;
  bool errorLoadingHexEditorFile = false;
  bool showHexEditorFileMemoryWindow = false;
  std::vector<unsigned char> hexEditorFileMemory;
  imgui_addons::ImGuiFileBrowser loadFileDialog;
  MemoryEditor fileMemEditor;

  void drawFileHexEditorMainMenu();
  void drawFileHexEditor();
  void drawLoadHexEditorFilePopup();
  void drawLoadedHexEditorFileWindow();

  void drawFileHexEditorMainMenu()
  {
      if(ImGui::BeginMenu("File Hex Editor"))
      {
        if(ImGui::MenuItem("Open File...")) openLoadHexEditorFileDialog = true;
        ImGui::EndMenu();
      }
  }

  void drawFileHexEditor()
  {
    drawLoadHexEditorFilePopup();
    if(showHexEditorFileMemoryWindow) drawLoadedHexEditorFileWindow();
  }

  void drawLoadHexEditorFilePopup()
  {
    static const char* const loadHexEditorFilePopupName = "Load Memory File";

    if(openLoadHexEditorFileDialog)
    {
      ImGui::OpenPopup(loadHexEditorFilePopupName);
      openLoadHexEditorFileDialog = false;
    }

    if(loadFileDialog.showFileDialog(loadHexEditorFilePopupName, imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310)))
    {
      if(!editor::util::loadFile(loadFileDialog.selected_path, hexEditorFileMemory)) errorLoadingHexEditorFile = true;
      else showHexEditorFileMemoryWindow = true;
    }

    if(errorLoadingHexEditorFile)
    {
      static const char* const errorPopupName = "Memory Loading Error";
      ImGui::OpenPopup(errorPopupName);

      if (ImGui::BeginPopupModal(errorPopupName, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
      {
        ImGui::Text("Error loading file");
        
        if(ImGui::Button("OK"))
        {
          errorLoadingHexEditorFile = false;
          openLoadHexEditorFileDialog = true;
        }
      }

      ImGui::EndPopup();
    }
  }

  void drawLoadedHexEditorFileWindow()
  {
    if(ImGui::Begin("File Hex Editor", &showHexEditorFileMemoryWindow))
    {
      fileMemEditor.DrawContents(hexEditorFileMemory.data(), hexEditorFileMemory.size());
      ImGui::End();
    }
  }
}
