#pragma once

#include <array>
#include <chip8.h>
#include <imgui.h>
#include <init.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <ImGuiFileBrowser.h>
#include <imgui_memory_editor/imgui_memory_editor.h>
#include <stb_image.h>
#include <texture.h>
#include <memory>
#include <unordered_map>

#include "util.h"

namespace editor
{
  namespace DGE = DragonGameEngine;

  static char* const generatedImageKey = "generated";
  static char* const loadedImageKey = "loaded";

  // Emulator
  bool showEmulator = false;
  const char* systemSelected = nullptr;
  bool openROMImageFileDialog = false;
  std::vector<unsigned char> emulatorROM;
  bool errorLoadingEmulatorROM = false;
  bool showEmulatorROMWindow = false;
  fs::path emulatorROMFileName;
  std::unique_ptr<CodexMachina::ChipEmulator> ce; 

  bool showGeneratedImageWindow = false;
  bool showLoadedImageWindow = false;
  bool openLoadImageFileDialog = false;
  bool errorLoadingImageFile = false;

  bool openLoadHexEditorFileDialog = false;
  bool errorLoadingHexEditorFile = false;
  bool showHexEditorFileMemoryWindow = false;
  std::vector<unsigned char> hexEditorFileMemory;

  std::unordered_map<char*, size_t> textures;
  imgui_addons::ImGuiFileBrowser loadFileDialog;
  MemoryEditor emulatorMemEditor;
  MemoryEditor fileMemEditor;

  void drawMainMenu();
  void drawEmulator();
  void drawGeneratedImageWindow();
  void generateImage();
  void drawLoadImagePopup();
  bool loadImage(const std::string& absoluteFilePath);
  void drawLoadedImageWindow();
  void drawLoadHexEditorFilePopup();
  void drawLoadedHexEditorFileWindow();

  void drawGui()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    drawMainMenu();
    if(showEmulator) drawEmulator();
    if(showGeneratedImageWindow) drawGeneratedImageWindow();
    drawLoadImagePopup();
    if(showLoadedImageWindow) drawLoadedImageWindow();
    drawLoadHexEditorFilePopup();
    if(showHexEditorFileMemoryWindow) drawLoadedHexEditorFileWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    const auto& io = ImGui::GetIO();

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* const backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
  }

  void drawMainMenu()
  {
    if(ImGui::BeginMainMenuBar())
    {
      if(ImGui::BeginMenu("File"))
      {
        if(ImGui::MenuItem("Exit")) DGE::setWindowShouldClose();
        ImGui::EndMenu();
      }

      if(ImGui::BeginMenu("Emulate"))
      {
        ImGui::MenuItem("Show Emulator", nullptr, &showEmulator);
        ImGui::EndMenu();
      }

      if(ImGui::BeginMenu("Image"))
      {
        ImGui::MenuItem("Display Generated Image", nullptr, &showGeneratedImageWindow);
        const static char* const loadImageMenuItemName = "Load Image";
        if(showLoadedImageWindow) ImGui::MenuItem(loadImageMenuItemName, nullptr, &showLoadedImageWindow);
        else if(ImGui::MenuItem(loadImageMenuItemName, nullptr)) openLoadImageFileDialog = true;
        ImGui::EndMenu();
      }

      if(ImGui::BeginMenu("File Hex Editor"))
      {
        if(ImGui::MenuItem("Open File...")) openLoadHexEditorFileDialog = true;
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }
  }

  void drawEmulator()
  {
    const auto mainViewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(ImVec2(mainViewport->GetWorkPos().x + 10, mainViewport->GetWorkPos().y + 10));
    static const char* labelText = "ROM File Name";

    if(!emulatorROMFileName.empty())
    {
      ImGui::SetNextWindowSize(ImVec2(400, 100));
    }
    else ImGui::SetNextWindowSize(ImVec2(200, 100));

    if(ImGui::Begin("Emulator", 0, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoResize))
    {
      if(ImGui::BeginMenuBar())
      {
        if(ImGui::BeginMenu("File"))
        {
          if(ImGui::MenuItem("Open")) openROMImageFileDialog = true;
          ImGui::EndMenu();
        }

        ImGui::EndMenuBar();
      }

      const static char* const loadROMFileDialogPopupName = "Open ROM Image";

      if(openROMImageFileDialog)
      {
        ImGui::OpenPopup(loadROMFileDialogPopupName);
        openROMImageFileDialog = false;
      }

      if(loadFileDialog.showFileDialog("Open ROM Image", imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310)))
      {
        if(!editor::loadFile(loadFileDialog.selected_path, emulatorROM)) errorLoadingEmulatorROM = true;
        else
        {
          emulatorROMFileName = fs::path(loadFileDialog.selected_path);
          if(ce) ce->loadMemory(emulatorROM, 0x200);
          showEmulatorROMWindow = true;
        }
      }

      if(errorLoadingEmulatorROM)
      {
        static const char* const errorPopupName = "ROM Loading Error";
        ImGui::OpenPopup(errorPopupName);

        if (ImGui::BeginPopupModal(errorPopupName, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
        {
          ImGui::Text("Error loading ROM file");
          
          if(ImGui::Button("OK"))
          {
            errorLoadingEmulatorROM = false;
            openROMImageFileDialog = true;
          }
        }
      }

      if(ImGui::BeginCombo("Systems", systemSelected))
      {
        for (auto emulatorName : CodexMachina::emulators())
        {
          const bool selected = (emulatorName == systemSelected);
          if(ImGui::Selectable(emulatorName, selected))
          {
            systemSelected = emulatorName;
            ce = std::make_unique<CodexMachina::Chip8>();
            if(!emulatorROM.empty()) ce->loadMemory(emulatorROM, 0x200);
          }
          if(selected) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
      }

      if(!emulatorROMFileName.empty())
      {
        ImGui::LabelText(labelText, emulatorROMFileName.filename().string().c_str());
      }

      ImGui::End();
    }

    if(ce && !emulatorROM.empty())
    {
      ImGui::SetNextWindowPos(ImVec2(mainViewport->GetWorkPos().x + 10, mainViewport->GetWorkPos().y + 120));
      ImGui::SetNextWindowSize(ImVec2(400, 400));

      if(ImGui::Begin("Emulator State", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
      {
        ImGui::LabelText("Program Counter", std::to_string(ce->getPC()).c_str());
        ImGui::LabelText("Timing", std::to_string(ce->getTiming()).c_str());
        ImGui::End();
      }

      ImGui::SetNextWindowPos(ImVec2(mainViewport->GetWorkPos().x + 10, mainViewport->GetWorkPos().y + 530));
      ImGui::SetNextWindowSize(ImVec2(400, 400));

      if(ImGui::Begin("Emulator Memory Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
      {
        emulatorMemEditor.DrawContents(ce->getMemory().data(), ce->getMemory().size());
        ImGui::End();
      }
    }
  }

  void drawGeneratedImageWindow()
  {
    if(!textures.count(generatedImageKey)) generateImage();
    const auto& t = DGE::Texture::getTexture(textures[generatedImageKey]);

    if(ImGui::Begin("Generated Image Window", &showGeneratedImageWindow, ImGuiWindowFlags_MenuBar))
    {
      ImGui::Image((void*)(intptr_t)t.textureId, ImVec2(static_cast<float>(t.width), static_cast<float>(t.height)));
      ImGui::End();
    }
  }

  void generateImage()
  {
    static constexpr unsigned int width = 128;
    static constexpr unsigned int height = 128;
    static constexpr unsigned int channels = 4;
    std::array<unsigned char, width * height * channels> data;

    for (int i = 0; i < width * height; ++i)
    {
      data[(i * 4)] = 0;
      data[(i * 4) + 1] = 0;
      data[(i * 4) + 2] = 255;
      data[(i * 4) + 3] = 255;
    }

    textures[generatedImageKey] = DGE::Texture::createTexture(width, height, data.data());
  }

  void drawLoadImagePopup()
  {
    const static char* const loadFileDialogPopupName = "Load Image";

    if(openLoadImageFileDialog)
    {
      ImGui::OpenPopup(loadFileDialogPopupName);
      openLoadImageFileDialog = false;
    }

    if(loadFileDialog.showFileDialog(loadFileDialogPopupName, imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310), ".jpg,.png,.bmp,.tga,.gif"))
    {
        if(!loadImage(loadFileDialog.selected_path)) errorLoadingImageFile = true;
        else showLoadedImageWindow = true;
    }

    if(errorLoadingImageFile)
    {
      const static char* const errorPopupName = "Image Loading Error";
      ImGui::OpenPopup(errorPopupName);

      if (ImGui::BeginPopupModal(errorPopupName, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
      {
        ImGui::Text("Error loading image file");
        
        if(ImGui::Button("OK"))
        {
          errorLoadingImageFile = false;
          openLoadImageFileDialog = true;
        }
      }

      ImGui::EndPopup();
    }
  }

  bool loadImage(const std::string& absoluteFilePath)
  {
    int width = 0;
    int height = 0;
    int channels = 0;
    const auto data = stbi_load(absoluteFilePath.c_str(), &width, &height, &channels, 4);
    if(nullptr == data) return false;
    textures[loadedImageKey] = DGE::Texture::createTexture(width, height, data);
    stbi_image_free(data);
    return true;
  }

  void drawLoadedImageWindow()
  {
    const auto& t = DGE::Texture::getTexture(textures[loadedImageKey]);

    if(ImGui::Begin("Loaded Image Window", &showLoadedImageWindow))
    {
      ImGui::Image((void*)(intptr_t)t.textureId, ImVec2(static_cast<float>(t.width), static_cast<float>(t.height)));
      ImGui::End();
    }
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
      if(!editor::loadFile(loadFileDialog.selected_path, hexEditorFileMemory)) errorLoadingHexEditorFile = true;
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