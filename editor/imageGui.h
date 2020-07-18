#pragma once

#include <array>
#include <imgui.h>
#include <ImGuiFileBrowser.h>
#include <stb_image.h>
#include <texture.h>

namespace editor::image
{
  namespace DGE = DragonGameEngine;
  
  bool showGeneratedImageWindow = false;
  bool showLoadedImageWindow = false;
  bool openLoadImageFileDialog = false;
  bool errorLoadingImageFile = false;
  bool generatedImage = false;
  size_t imageTextureId;
  size_t loadedTextureId;
  imgui_addons::ImGuiFileBrowser loadFileDialog;

  void drawImageMainMenu();
  void drawImageGUI();
  void drawGeneratedImageWindow();
  void generateImage();
  void drawLoadImagePopup();
  bool loadImage(const std::string&);
  void drawLoadedImageWindow();

  void drawImageMainMenu()
  {
      if(ImGui::BeginMenu("Image"))
      {
        ImGui::MenuItem("Display Generated Image", nullptr, &showGeneratedImageWindow);
        const static char* const loadImageMenuItemName = "Load Image";
        if(showLoadedImageWindow) ImGui::MenuItem(loadImageMenuItemName, nullptr, &showLoadedImageWindow);
        else if(ImGui::MenuItem(loadImageMenuItemName, nullptr)) openLoadImageFileDialog = true;
        ImGui::EndMenu();
      }
  }

  void drawImageGUI()
  {
    if(showGeneratedImageWindow) drawGeneratedImageWindow();
    drawLoadImagePopup();
    if(showLoadedImageWindow) drawLoadedImageWindow();
  }

  void drawGeneratedImageWindow()
  {
    if(!generatedImage) generateImage();
    const auto& t = DGE::Texture::getTexture(imageTextureId);

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

    imageTextureId = DGE::Texture::createTexture(width, height, data.data());
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
    loadedTextureId = DGE::Texture::createTexture(width, height, data);
    stbi_image_free(data);
    return true;
  }

  void drawLoadedImageWindow()
  {
    const auto& t = DGE::Texture::getTexture(loadedTextureId);

    if(ImGui::Begin("Loaded Image Window", &showLoadedImageWindow))
    {
      ImGui::Image((void*)(intptr_t)t.textureId, ImVec2(static_cast<float>(t.width), static_cast<float>(t.height)));
      ImGui::End();
    }
  }
}
