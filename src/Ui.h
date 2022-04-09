#ifndef GAMEFRAME_UI_H
#define GAMEFRAME_UI_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include "gfx/Window.h"

class Ui {
private:
    /// frameActive is used to prevent calling
    /// postUpdateFrame when the gui is enabled mid frame
    static bool show, frameActive;
    static Window* window;

public:
    static void InitImGui(Window* windowIn) {
        show = frameActive = false;
        window = windowIn;

        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window->handle, true);
        ImGui_ImplOpenGL3_Init("#version 430");
    }

    /**
     * Start a new ImGui frame if enabled (show)
     */
    static void preUpdateFrame() {
        if (!show) return;
        frameActive = true;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    /**
     * End the ImGui frame if one is active
     * and update the draw data
     */
    static void postUpdateFrame() {
        if (!frameActive) return;

        static float f = 0.0f;
        static int counter = 0;
        static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

        // Our window
        if (ImGui::Begin("Hello, world!", &show)) {       // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f, nullptr, ImGuiSliderFlags_AlwaysClamp);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float *) &clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
        }
        ImGui::End();
        frameActive = false;

        // Updates ImGui::DrawData
        ImGui::Render();

        if (!show)
            window->setCursorLock(!show); // If Ui is not shown lock cursor
    }

    /**
     * Render the gui if enabled (show)
     */
    static void render() {
        if (!show) return;
        // Renders ImGui::DrawData
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    static void visible(bool visible) {
        show = visible;
    }

    static bool isActive() {
        return frameActive;
    }

    static void cleanImGui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};

#endif //GAMEFRAME_UI_H
