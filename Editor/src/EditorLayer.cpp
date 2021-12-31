#include "EditorLayer.h"


void EditorLayer::OnAttach()
{
	m_Scene = Olala::CreateRef<Olala::Scene>();
    m_AssetManager = Olala::CreateRef<Olala::AssetManager>("..\\Olala\\Asset\\Scene\\Demo");

	// Editor camera
    m_EditorCamera = m_Scene->CreateEntity("Editor Camera");
    m_EditorCamera.AddComponent<Olala::EditorCameraControllerComponent>();
	auto& cameraComponent = m_EditorCamera.AddComponent<Olala::CameraComponent>(Olala::CreateRef<Olala::OrthographicCamera>());
	Olala::FramebufferSpecs specs;
	specs.Width = 800, specs.Height = 600;
	cameraComponent.RenderTarget = Olala::Framebuffer::Create(specs);

	m_SceneViewPanel = Olala::CreateRef<SceneViewPanel>(m_EditorCamera);
	m_PropertyPanel = Olala::CreateRef<PropertyPanel>();
	m_SceneHierarchyPanel = Olala::CreateRef<SceneHierarchyPanel>(m_Scene, m_PropertyPanel);
    m_RuntimeViewPanel = Olala::CreateRef<RuntimeViewPanel>();
    m_AssetPanel = Olala::CreateRef<AssetPanel>(m_AssetManager);
    m_DebugPanel = Olala::CreateRef<DebugPanel>(&temp);

	Olala::Ref<Olala::Texture2D> exampleTextures[2];
    exampleTextures[0] = m_AssetManager->GetPool<Olala::Texture2D>().Get("Planet9.jpg");
    exampleTextures[1] = m_AssetManager->GetPool<Olala::Texture2D>().Get("Abe.png");

	// Quads
	for (int i = 0; i < 8; i++)
	{
		Olala::Entity quad = m_Scene->CreateEntity("Quad" + std::to_string(i));
		quad.AddComponent<Olala::SpriteRendererComponent>(glm::vec2{ 80.f, 45.f }, glm::vec4{ 1.0f, 1.0f, 1.0f, 1.0f }, exampleTextures[i%2]);
		quad.GetComponent<Olala::TransformComponent>().Position.x = -350.f + 100.f * i;
	}

}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float dt)
{
    m_EditorCamera.GetComponent<Olala::EditorCameraControllerComponent>().IsOn = m_SceneViewPanel->GetIsFocused();

	m_Scene->OnUpdate(dt);
}

void EditorLayer::OnImGuiRender()
{
    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;


    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->GetWorkPos());
        ImGui::SetNextWindowSize(viewport->GetWorkSize());
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }
    else
    {
        dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", (bool*)true, window_flags);
    if (!opt_padding)
        ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    // Menu bar
    DrawMenuBar();

    // Panels
    m_SceneHierarchyPanel  -> OnImGuiRender();
    m_PropertyPanel        -> OnImGuiRender();
    m_SceneViewPanel       -> OnImGuiRender();
    m_RuntimeViewPanel     -> OnImGuiRender();
    m_AssetPanel           -> OnImGuiRender();
    m_DebugPanel           -> OnImGuiRender();


    ImGui::End();

    ImGui::ShowDemoWindow();
}

void EditorLayer::DrawMenuBar()
{
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{

			}
			if (ImGui::MenuItem("Save"))
			{

			}
            ImGui::Separator();
            if (ImGui::MenuItem("Close"))
            {
                Olala::Application::Get().Close();
            }
			ImGui::EndMenu();
		}

        if (ImGui::BeginMenu("View"))
        {
            if (ImGui::MenuItem("Entities"))   m_SceneHierarchyPanel->SetOpen(true);
            if (ImGui::MenuItem("Scene"))      m_SceneViewPanel->SetOpen(true);
            if (ImGui::MenuItem("Property"))   m_PropertyPanel->SetOpen(true);
            if (ImGui::MenuItem("Asset"))      m_AssetPanel->SetOpen(true);
            if (ImGui::MenuItem("Debug"))      m_DebugPanel->SetOpen(true);

            ImGui::EndMenu();
        }
		ImGui::EndMenuBar();
	}
    
}

void EditorLayer::OnEvent(Olala::Event& e)
{
}
