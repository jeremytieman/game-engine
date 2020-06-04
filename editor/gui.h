#pragma once

#include <array>
#include <imgui.h>
#include <init.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>
#include <ImGuiFileBrowser.h>
#include <imgui_memory_editor/imgui_memory_editor.h>
#include <stb_image.h>
#include <texture.h>
#include <unordered_map>

#include "util.h"

namespace editor
{
  namespace DGE = DragonGameEngine;

  static char* const generatedImageKey = "generated";
  static char* const loadedImageKey = "loaded";

  bool showEmulator = false;
  unsigned int systemSelected = 0;
  bool openROMImageFileDialog = false;
  std::vector<char> emulatorROM;
  bool errorLoadingEmulatorROM = false;
  bool showEmulatorROMWindow = false;
  std::string emulatorROMFileName;

  bool showGeneratedImageWindow = false;
  bool showLoadedImageWindow = false;
  bool openLoadImageFileDialog = false;
  bool openLoadMemoryFileDialog = false;
  bool errorLoadingImageFile = false;
  bool errorLoadingMemoryFile = false;
  bool showLoadedMemoryWindow = false;

  std::unordered_map<char*, size_t> textures;
  std::vector<char> v;
  imgui_addons::ImGuiFileBrowser loadFileDialog;
  MemoryEditor memEditor;

  void drawMainMenu();
  void drawEmulator();
  void drawGeneratedImageWindow();
  void generateImage();
  void drawLoadImagePopup();
  bool loadImage(const std::string& absoluteFilePath);
  void drawLoadedImageWindow();
  void drawLoadMemoryPopup();
  void drawLoadedMemoryWindow();

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
    drawLoadMemoryPopup();
    if(showLoadedMemoryWindow) drawLoadedMemoryWindow();

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

      if(ImGui::BeginMenu("Memory"))
      {
        if(ImGui::MenuItem("Open File...")) openLoadMemoryFileDialog = true;
        ImGui::EndMenu();
      }

      ImGui::EndMenuBar();
    }
  }

  void drawEmulator()
  {
    if(ImGui::Begin("Emulator", 0, ImGuiWindowFlags_MenuBar))
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
          emulatorROMFileName = loadFileDialog.selected_path;
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

      const char* systems[] = { "CHIP8", "SCHIP8" };

      if(ImGui::BeginCombo("Systems", systems[systemSelected]))
      {
        for (int i = 0; i < IM_ARRAYSIZE(systems); ++i)
        {
            const bool selected = (systemSelected == i);
            if (ImGui::Selectable(systems[i], selected)) systemSelected = i;
            if (selected) ImGui::SetItemDefaultFocus();
        }

        ImGui::EndCombo();
      }

      ImGui::LabelText("ROM File Name", emulatorROMFileName.c_str());
      ImGui::End();
    }

    if(!emulatorROM.empty())
    {
      if(ImGui::Begin("Memory Editor"))
      {
        memEditor.DrawContents(emulatorROM.data(), emulatorROM.size());
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

  void drawLoadMemoryPopup()
  {
    static const char* const loadMemoryFilePopupName = "Load Memory File";

    if(openLoadMemoryFileDialog)
    {
      ImGui::OpenPopup(loadMemoryFilePopupName);
      openLoadMemoryFileDialog = false;
    }

    if(loadFileDialog.showFileDialog(loadMemoryFilePopupName, imgui_addons::ImGuiFileBrowser::DialogMode::OPEN, ImVec2(700, 310)))
    {
      if(!editor::loadFile(loadFileDialog.selected_path, v)) errorLoadingMemoryFile = true;
      else showLoadedMemoryWindow = true;
    }

    if(errorLoadingMemoryFile)
    {
      static const char* const errorPopupName = "Memory Loading Error";
      ImGui::OpenPopup(errorPopupName);

      if (ImGui::BeginPopupModal(errorPopupName, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
      {
        ImGui::Text("Error loading file");
        
        if(ImGui::Button("OK"))
        {
          errorLoadingMemoryFile = false;
          openLoadMemoryFileDialog = true;
        }
      }

      ImGui::EndPopup();
    }
  }

  void drawLoadedMemoryWindow()
  {
    if(ImGui::Begin("Memory Editor", &showLoadedMemoryWindow))
    {
      memEditor.DrawContents(v.data(), v.size());
      ImGui::End();
    }
  }
}