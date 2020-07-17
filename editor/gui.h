#pragma once

#include <algorithm>
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
#include <sstream>
#include <unordered_map>
#include <vector>

#include "util.h"

namespace editor
{
  namespace DGE = DragonGameEngine;

  enum class ImageKeys {
    Emulator,
    Generated,
    Loaded
  };

  // Emulator
  bool showEmulator = false;
  const char* systemSelected = nullptr;
  bool openROMImageFileDialog = false;
  std::vector<unsigned char> emulatorROM;
  bool errorLoadingEmulatorROM = false;
  bool showEmulatorROMWindow = false;
  fs::path emulatorROMFileName;
  bool showEmulatorImageWindow = false;
  std::unique_ptr<CodexMachina::ChipEmulator> ce;
  bool runEmulator = false;

  bool showGeneratedImageWindow = false;
  bool showLoadedImageWindow = false;
  bool openLoadImageFileDialog = false;
  bool errorLoadingImageFile = false;

  bool openLoadHexEditorFileDialog = false;
  bool errorLoadingHexEditorFile = false;
  bool showHexEditorFileMemoryWindow = false;
  std::vector<unsigned char> hexEditorFileMemory;

  std::unordered_map<ImageKeys, size_t, EnumClassHash> textures;
  imgui_addons::ImGuiFileBrowser loadFileDialog;
  MemoryEditor emulatorMemEditor;
  MemoryEditor emulatorDisplayMemEditor;
  MemoryEditor fileMemEditor;

  void drawMainMenu();
  void drawEmulator();
  void generateChip8Texture(const size_t width, const size_t height, const std::vector<unsigned char>& display);
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
        std::ostringstream iSS;
        iSS << "0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(4) << ce->getI();
        ImGui::LabelText("I", iSS.str().c_str());
        std::ostringstream pcSS;
        pcSS << "0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(3) << ce->getPC();
        ImGui::LabelText("Program Counter", pcSS.str().c_str());
        ImGui::LabelText("Timing in Hertz", std::to_string(ce->getTimingHertz()).c_str());
        ImGui::LabelText("Next Op Code", ce->getNextOpcode().c_str());
        ImGui::LabelText("Next Op Code Description", ce->getNextOpcodeDesc().c_str());
        if(ImGui::Button("Run")) runEmulator = true;
        if(ImGui::Button("Step")) ce->emulateCycle();
        if(ImGui::Button("Stop")) runEmulator = false;
        ImGui::End();
      }

      ImGui::SetNextWindowPos(ImVec2(mainViewport->GetWorkPos().x + 10, mainViewport->GetWorkPos().y + 530));
      ImGui::SetNextWindowSize(ImVec2(400, 400));

      if(ImGui::Begin("Emulator Memory Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
      {
        emulatorMemEditor.DrawContents(ce->getMemory().data(), ce->getMemory().size());
        ImGui::End();
      }

      if(runEmulator) ce->emulateCycle();
      size_t x;
      size_t y;
      std::tie(x, y) = ce->getDisplaySize();
      auto& displayMemory = ce->getDisplay();
      generateChip8Texture(x, y, displayMemory);
      const auto& t = DGE::Texture::getTexture(textures[ImageKeys::Emulator]);
      ImGui::SetNextWindowPos(ImVec2(mainViewport->GetWorkPos().x + 420, mainViewport->GetWorkPos().y + 10));
      ImGui::SetNextWindowSize(ImVec2(400, 400));

      if(ImGui::Begin("Emulator Display", &showEmulatorImageWindow, ImGuiWindowFlags_MenuBar))
      {
        ImGui::Image((void*)(intptr_t)t.textureId, ImVec2(static_cast<float>(t.width), static_cast<float>(t.height)));
        ImGui::End();
      }

      ImGui::SetNextWindowPos(ImVec2(mainViewport->GetWorkSize().x - 210, mainViewport->GetWorkPos().y + 10));
      ImGui::SetNextWindowSize(ImVec2(200, 400));

      if(ImGui::Begin("Registers", nullptr, ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoResize))
      {
        std::vector<const char*> registerNames{ ce->getRegisterNames() };
        
        std::sort(registerNames.begin(), registerNames.end(), [](const std::string& lhs, const std::string& rhs) {
          return lhs < rhs;
        });
        
        const auto& registers = ce->getRegisterValues();

        for(const auto& registerName : registerNames)
        {
          std::ostringstream rSS;
          rSS << "0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << registers.at(registerName);
          ImGui::LabelText(registerName, rSS.str().c_str());
        }
        
        ImGui::End();
      }

      ImGui::SetNextWindowPos(ImVec2(mainViewport->GetWorkSize().x - 410, mainViewport->GetWorkPos().y + 420));
      ImGui::SetNextWindowSize(ImVec2(400, 400));

      if(ImGui::Begin("Emulator Display Memory Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
      {
        emulatorDisplayMemEditor.DrawContents(displayMemory.data(), displayMemory.size());
        ImGui::End();
      }
    }
  }

  void generateChip8Texture(const size_t width, const size_t height, const std::vector<unsigned char>& display)
  {
    static constexpr unsigned int channels = 4;
    std::vector<unsigned char> data;
    data.reserve(channels * display.size());

    for(const auto& pixel : display)
    {
      const auto value = (pixel == 1) ? 255 : 0;
      data.push_back(value);
      data.push_back(value);
      data.push_back(value);
      data.push_back(255);
    }

    textures[ImageKeys::Emulator] = DGE::Texture::createTexture(width, height, data.data());
  }

  void drawGeneratedImageWindow()
  {
    if(!textures.count(ImageKeys::Generated)) generateImage();
    const auto& t = DGE::Texture::getTexture(textures[ImageKeys::Generated]);

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

    textures[ImageKeys::Generated] = DGE::Texture::createTexture(width, height, data.data());
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
    textures[ImageKeys::Loaded] = DGE::Texture::createTexture(width, height, data);
    stbi_image_free(data);
    return true;
  }

  void drawLoadedImageWindow()
  {
    const auto& t = DGE::Texture::getTexture(textures[ImageKeys::Loaded]);

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