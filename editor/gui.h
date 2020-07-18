#pragma once

#include <imgui.h>
#include <init.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include "emulatorGui.h"
#include "fileHexEditorGui.h"
#include "imageGui.h"

namespace editor
{
  namespace DGE = DragonGameEngine;

  void drawGui();
  void drawMainMenu();

  void drawGui()
  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    drawMainMenu();
    emulator::drawEmulator();
    image::drawImageGUI();
    fileHexEditor::drawFileHexEditor();

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

      emulator::drawEmulatorMainMenu();
      image::drawImageMainMenu();
      fileHexEditor::drawFileHexEditorMainMenu();
      ImGui::EndMenuBar();
    }
  }
}