#pragma once

#include <chip8.h>
#include <emulator.h>
#include <filesystem>
#include <imgui.h>
#include <ImGuiFileBrowser.h>
#include <imgui_memory_editor/imgui_memory_editor.h>
#include <texture.h>

#include "util.h"

namespace editor::emulator
{
  namespace fs = std::filesystem;
  namespace DGE = DragonGameEngine;

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
  MemoryEditor emulatorMemEditor;
  MemoryEditor emulatorDisplayMemEditor;
  size_t emulatorTextureId;
  size_t displayZoom = 1;
  imgui_addons::ImGuiFileBrowser loadFileDialog;

  void drawEmulatorMainMenu();
  void drawEmulator();
  void drawEmulatorWindows();
  void generateChip8Texture(const size_t, const size_t, const std::vector<unsigned char>&);

  void drawEmulatorMainMenu()
  {
      if(ImGui::BeginMenu("Emulate"))
      {
        ImGui::MenuItem("Show Emulator", nullptr, &showEmulator);
        ImGui::EndMenu();
      }
  }

  void drawEmulator()
  {
    if(showEmulator) drawEmulatorWindows();
  }

  void drawEmulatorWindows()
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
        if(!editor::util::loadFile(loadFileDialog.selected_path, emulatorROM)) errorLoadingEmulatorROM = true;
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
            ce = std::make_unique<CodexMachina::Chip8::Chip8>();
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
      const auto& t = DGE::Texture::getTexture(emulatorTextureId);
      ImGui::SetNextWindowPos(ImVec2(mainViewport->GetWorkPos().x + 420, mainViewport->GetWorkPos().y + 10));
      ImGui::SetNextWindowSize(ImVec2(400, 400));

      if(ImGui::Begin("Emulator Display", &showEmulatorImageWindow, ImGuiWindowFlags_MenuBar))
      {
        if(ImGui::BeginMenuBar())
        {
          if(ImGui::BeginMenu("Zoom"))
          {
            if(ImGui::MenuItem("1:1", nullptr, displayZoom == 1)) displayZoom = 1;
            if(ImGui::MenuItem("2:1", nullptr, displayZoom == 2)) displayZoom = 2;
            if(ImGui::MenuItem("4:1", nullptr, displayZoom == 4)) displayZoom = 4;
            if(ImGui::MenuItem("8:1", nullptr, displayZoom == 8)) displayZoom = 8;
            ImGui::EndMenu();
          }

          ImGui::EndMenuBar();
        }
        ImGui::Image((void*)(intptr_t)t.textureId, ImVec2(static_cast<float>(t.width), static_cast<float>(t.height)));
        ImGui::End();
      }

      ImGui::SetNextWindowPos(ImVec2(mainViewport->GetWorkSize().x - 420, mainViewport->GetWorkPos().y + 10));
      ImGui::SetNextWindowSize(ImVec2(200, 400));

      if(ImGui::Begin("Stack", nullptr, ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoResize))
      {
        const auto& stack = ce->getStack();

        for(const auto& stackValue : stack)
        {
          std::ostringstream sSS;
          sSS << "0x" << std::uppercase << std::hex << std::setfill('0') << std::setw(2) << stackValue;
          ImGui::Text(sSS.str().c_str());
        }
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
    static constexpr size_t channels = 4;
    static constexpr auto alphaOffset = channels - 1;
    const auto zoomWidth = channels * displayZoom;
    std::vector<unsigned char> data(display.size() * zoomWidth * displayZoom);
    const auto outputRowSize = width * zoomWidth;

    for(size_t h = 0; h < height; ++h)
    {
      const auto inputHeightOffset = h * width;

      for(size_t w = 0; w < width; ++w)
      {
        const auto inputOffset = inputHeightOffset + w;
        const auto pixel = display[inputOffset];
        const auto value = (pixel == 1) ? 255 : 0;
        
        for (size_t zh = 0; zh < displayZoom; ++zh)
        {
          const auto outputRowOffset = ((h * displayZoom) + zh) * outputRowSize;

          for (size_t zw = 0; zw < displayZoom; ++zw)
          {
            const auto outputOffset = outputRowOffset + ((w * displayZoom) + zw) * channels;
            for(size_t c = 0; c < (channels - 1); ++c) data[outputOffset + c] = value;
            data[outputOffset + alphaOffset] = 255;
          }
        }
      }
    }

    emulatorTextureId = DGE::Texture::createTexture(width * displayZoom, height * displayZoom, data.data());
  }
}
