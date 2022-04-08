#ifndef GAMEFRAME_UI_H
#define GAMEFRAME_UI_H

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>

class Ui {
private:
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    bool show = false, demoShow = false;

public:
    static void InitImGui(GLFWwindow* window) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();

        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 430");
    }

    void updateFrame(Window* window) {
        if (!show) return;

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        static float f = 0.0f;
        static int counter = 0;

        // Demo window
        if (demoShow)
            ImGui::ShowDemoWindow(&demoShow);

        // Our window
        if (ImGui::Begin("Hello, world!", &show)) {       // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &demoShow);           // Edit bools storing our window open/close state

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float *) &clear_color); // Edit 3 floats representing a color

            if (ImGui::Button(
                    "Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
        }
        ImGui::End();

        // Updates ImGui::DrawData
        ImGui::Render();

        if (!show)
            window->setCursorLock(!show); // If Ui is not shown lock cursor
    }

    void render() {
        if (!show) return;
        // Renders ImGui::DrawData
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    void visible(bool visible) {
        this->show = visible;
    }

    static void cleanImGui() {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }
};

#endif //GAMEFRAME_UI_H
