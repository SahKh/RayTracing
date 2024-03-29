#include "Walnut/Application.h"
#include "Walnut/EntryPoint.h"
#include <stdint.h>

#include "Walnut/Image.h"
#include "Walnut/Timer.h"

#include "Renderer.h"
#include "Camera.h"

using namespace Walnut;

class VulkanRayTracer_Layer : public Walnut::Layer
{
public:
	VulkanRayTracer_Layer()
		: m_Camera(45.0f, 0.1f, 100.0f) {}

	virtual void OnUpdate(float ts) override {
		m_Camera.OnUpdate(ts);
	}

	virtual void OnUIRender() override
	{
		ImGui::Begin("Settings");
		ImGui::Text("Last Render %.3fms ", m_LastRenderTime);
		if (ImGui::Button("Render")) { Render(); }
		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("Viewport");

		m_ViewportWidth = ImGui::GetContentRegionAvail().x;
		m_ViewportHeight = ImGui::GetContentRegionAvail().y;

		auto image = m_Renderer.GetFinalImage();
		if (image) {																						 //flip image using inversed uv coord
			ImGui::Image(image->GetDescriptorSet(), { (float)image->GetWidth(), (float)image->GetHeight() }, ImVec2(0, 1), ImVec2(1,0) );
		}
		ImGui::End();
		ImGui::PopStyleVar();

		Render();
	}

	void Render() {
		Timer timer;
		
		m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Camera.OnResize(m_ViewportWidth, m_ViewportHeight);
		m_Renderer.Render(m_Camera);

		m_LastRenderTime = timer.ElapsedMillis();
	}
private: 
	Renderer m_Renderer;
	Camera m_Camera;
	uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
	float m_LastRenderTime = 0.0f;
};

Walnut::Application* Walnut::CreateApplication(int argc, char** argv)
{
	Walnut::ApplicationSpecification spec;
	spec.Name = "VulkanRayTracer";

	Walnut::Application* app = new Walnut::Application(spec);
	app->PushLayer<VulkanRayTracer_Layer>();
	app->SetMenubarCallback([app]()
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Exit"))
			{
				app->Close();
			}
			ImGui::EndMenu();
		}
	});
	return app;
}