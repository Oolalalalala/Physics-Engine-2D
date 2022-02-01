#include "EditorLayer.h"

namespace fs = std::filesystem;

static Olala::Entity GetEditorCamera(Olala::Ref<Olala::Scene> scene)
{
    auto cameraView = scene->GetAllEntitiesWith<Olala::TagComponent, Olala::CameraComponent>();
    for (auto e : cameraView)
    {
        if (cameraView.get<Olala::TagComponent>(e).Tag == "Editor Camera")
            return Olala::Entity(e, scene.get());
    }
    return Olala::Entity();
}

void EditorLayer::OnAttach()
{
    // Panels
	m_SceneViewPanel = Olala::CreateRef<SceneViewPanel>(m_Scene, m_EditorCamera);
	m_PropertyPanel = Olala::CreateRef<PropertyPanel>();
	m_SceneHierarchyPanel = Olala::CreateRef<SceneHierarchyPanel>(m_Scene, m_PropertyPanel);
    m_AssetPanel = Olala::CreateRef<AssetPanel>(nullptr);
    m_DebugPanel = Olala::CreateRef<DebugPanel>(m_SceneViewPanel);
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float dt)
{
    if (m_EditorCamera && m_EditorCamera.HasComponent<Olala::EditorCameraControllerComponent>())
        m_EditorCamera.GetComponent<Olala::EditorCameraControllerComponent>().IsOn = m_SceneViewPanel->GetIsFocused();

    if (m_Scene)
    {
        if (m_IsRuntime)
            m_RuntimeScene->OnRuntimeUpdate(dt);
        else
            m_Scene->OnUpdate(dt);
    }

    m_SceneHierarchyPanel   ->  OnUpdate(dt);
    m_PropertyPanel         ->  OnUpdate(dt);
    m_SceneViewPanel        ->  OnUpdate(dt);
    m_AssetPanel            ->  OnUpdate(dt);
    m_DebugPanel            ->  OnUpdate(dt);
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
            if (ImGui::MenuItem("New Scene"))
            {
                // TODO : save check here
                if (m_IsRuntime)
                {
                    OnRuntimeEnd();
                    m_IsRuntime = false;
                }

                fs::path selectedFolder = Olala::FileDialog::SelectFolder("Select Folder");
                if (!selectedFolder.empty())
                {
                    m_Scene = Olala::CreateRef<Olala::Scene>();

                    m_EditorCamera = m_Scene->CreateEntity("Editor Camera");
                    m_EditorCamera.AddComponent<Olala::CameraComponent>(Olala::CreateRef<Olala::OrthographicCamera>(), Olala::Framebuffer::Create(Olala::FramebufferSpecs()));
                    m_EditorCamera.AddComponent<Olala::EditorCameraControllerComponent>();

                    Olala::SceneSerializer::CraeteDirectory(selectedFolder, m_Scene);
                    m_SceneSerializer = Olala::CreateRef<Olala::SceneSerializer>(m_Scene, selectedFolder / m_Scene->GetName() / (m_Scene->GetName() + ".olala"));

                    m_SceneViewPanel->SetScene(m_Scene);
                    m_SceneViewPanel->SetCamera(m_EditorCamera);
                    m_SceneHierarchyPanel->SetDisplayingScene(m_Scene);
                    m_AssetPanel->SetAssetManager(m_Scene->GetAssetManager());

                    m_IsSceneLoaded = true;
                    m_IsSceneSaved = true;
                }
            }
			if (ImGui::MenuItem("Open Scene"))
			{
                // TODO : save check
                if (m_IsRuntime)
                {
                    OnRuntimeEnd();
                    m_IsRuntime = false;
                }

                fs::path filepath = Olala::FileDialog::OpenFile("Select File", { "Scene File (.olala)", "*.olala" });

                if (!filepath.empty())
                {
                    m_Scene = Olala::CreateRef<Olala::Scene>();
                    m_SceneSerializer = Olala::CreateRef<Olala::SceneSerializer>(m_Scene, filepath);
                    m_SceneSerializer->Deserialize();

                    m_EditorCamera = GetEditorCamera(m_Scene);
                    if (m_EditorCamera)
                        m_EditorCamera.AddComponent<Olala::EditorCameraControllerComponent>();

                    m_SceneViewPanel->SetScene(m_Scene);
                    m_SceneViewPanel->SetCamera(m_EditorCamera);
                    m_SceneHierarchyPanel->SetDisplayingScene(m_Scene);
                    m_PropertyPanel->DisplayEntity(Olala::Entity());
                    m_AssetPanel->SetAssetManager(m_Scene->GetAssetManager());

                    m_IsSceneLoaded = true;
                    m_IsSceneSaved = true;
                }
			}
            if (ImGui::MenuItem("Close Scene") && m_IsSceneLoaded)
            {
                // Save check
                if (!m_IsSceneSaved)
                {
                    auto result = Olala::FileDialog::MessageYesNoCancel("Message", "Save changes?");
                    if (result == Olala::FileDialog::Result::yes)
                        m_SceneSerializer->Serialize();
                    if (result != Olala::FileDialog::Result::cancel)
                    {
                        // Reset
                        m_Scene = nullptr;
                        m_SceneSerializer = nullptr;
                        m_SceneViewPanel->SetScene(nullptr);
                        m_SceneViewPanel->SetCamera(Olala::Entity());
                        m_SceneHierarchyPanel->SetDisplayingScene(nullptr);
                        m_PropertyPanel->DisplayEntity(Olala::Entity());
                        m_AssetPanel->SetAssetManager(nullptr);
                        m_EditorCamera = Olala::Entity();

                        m_IsSceneLoaded = false;
                    }
                }
                else
                {
                    // Reset
                    m_Scene = nullptr;
                    m_SceneSerializer = nullptr;
                    m_SceneViewPanel->SetScene(nullptr);
                    m_SceneViewPanel->SetCamera(Olala::Entity());
                    m_SceneHierarchyPanel->SetDisplayingScene(nullptr);
                    m_PropertyPanel->DisplayEntity(Olala::Entity());
                    m_AssetPanel->SetAssetManager(nullptr);
                    m_EditorCamera = Olala::Entity();

                    m_IsSceneLoaded = false;
                }

            }
            ImGui::Separator();
            if (ImGui::MenuItem("Save", "Ctrl+S") && m_IsSceneLoaded)
            {
                m_SceneSerializer->Serialize();
                m_IsSceneSaved = true;
            }
            if (ImGui::MenuItem("Save As") && m_Scene)
            {
                fs::path folderPath = Olala::FileDialog::SelectFolder("Select Folder");
                if (!folderPath.empty())
                {
                    Olala::SceneSerializer::CraeteDirectory(folderPath, m_Scene);
                    Olala::SceneSerializer::CopyAssets(m_SceneSerializer->GetDirectoryPath(), folderPath / m_Scene->GetName());
                    Olala::SceneSerializer(m_Scene, folderPath / m_Scene->GetName() / (m_Scene->GetName() + ".olala")).Serialize();
                }
			}
            ImGui::Separator();
            if (ImGui::MenuItem("Import"))
            {
                std::string filepath = Olala::FileDialog::OpenFile("Select Asset", 
                {   
                    "All Files", "*",
                    "Image (.jpg, .png, .jpeg, .bmp, .gif)", "*.jpg *.png *.jpeg *.bmp *.gif" 
                });
                if (!filepath.empty())
                {
                    m_SceneSerializer->Import<Olala::Texture2D>(filepath);
                }
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit"))
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

        if (ImGui::BeginMenu("Program"))
        {
            if (ImGui::MenuItem(m_IsRuntime ? "Stop" : "Start", "F5"))
            {
                m_IsRuntime = !m_IsRuntime;
                if (m_IsRuntime)
                    OnRuntimeBegin();
                else
                    OnRuntimeEnd();
            }
            if (ImGui::MenuItem("Pause"))
            {
                m_IsPausing = true;
            }
            ImGui::EndMenu();
        }

		ImGui::EndMenuBar();
	}
    
}

void EditorLayer::OnRuntimeBegin()
{
    m_RuntimeScene = Olala::Scene::Copy(m_Scene);
    m_RuntimeScene->InitializePhysics();

    m_SceneViewPanel->SetScene(m_RuntimeScene);
    m_SceneHierarchyPanel->SetDisplayingScene(m_RuntimeScene);

    auto cameraView = m_RuntimeScene->GetAllEntitiesWith<Olala::TagComponent, Olala::CameraComponent>();
    for (auto e : cameraView)
    {
        if (cameraView.get<Olala::TagComponent>(e).Tag == "Editor Camera")
        {
            m_EditorCamera = Olala::Entity(e, m_RuntimeScene.get());
            m_SceneViewPanel->SetCamera(m_EditorCamera);
            break;
        }
    }

    m_PropertyPanel->DisplayEntity(Olala::Entity());
    m_PropertyPanel->SetIsRuntime(true);
}

void EditorLayer::OnRuntimeEnd()
{
    m_SceneViewPanel->SetScene(m_Scene);
    m_SceneHierarchyPanel->SetDisplayingScene(m_Scene);

    auto cameraView = m_Scene->GetAllEntitiesWith<Olala::TagComponent, Olala::CameraComponent>();
    for (auto e : cameraView)
    {
        if (cameraView.get<Olala::TagComponent>(e).Tag == "Editor Camera")
        {
            m_EditorCamera = Olala::Entity(e, m_Scene.get());
            m_SceneViewPanel->SetCamera(m_EditorCamera);
            break;
        }
    }

    m_PropertyPanel->DisplayEntity(Olala::Entity());
    m_PropertyPanel->SetIsRuntime(false);

    m_RuntimeScene = nullptr;
}

void EditorLayer::OnEvent(Olala::Event& e)
{
}
