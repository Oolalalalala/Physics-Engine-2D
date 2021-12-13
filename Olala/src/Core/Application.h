#pragma once
#include "Window.h"
#include "Event/ApplicationEvent.h"
#include "Layer/LayerStack.h"

namespace Olala {
	
	class Application
	{
	public:
		Application(const std::string& name);

		void Init();
		void Run();

		void OnEvent(Event& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		static Application& Get();
		Window& GetWindow() { return *m_Window; }

	protected:
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

	private:
		bool m_Running;
		std::string m_ApplicationName;
		Scope<Window> m_Window;

		Scope<LayerStack> m_LayerStack;

		static Application* s_Instance;
	};

	Application* CreateApplication();
}