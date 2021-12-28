#pragma once

#include <Olala.h>
#include "Panel/SceneViewPanel.h"
#include "Panel/PropertyPanel.h"
#include "Panel/SceneHierarchyPanel.h"
#include "Panel/RuntimeViewPanel.h"
#include "Panel/AssetPanel.h"

class EditorLayer : public Olala::Layer
{
public:
	EditorLayer() = default;
	~EditorLayer() = default;

	void OnAttach() override;
	void OnDetach() override;
	void OnUpdate(float dt) override;
	void OnEvent(Olala::Event& e) override;
	void OnImGuiRender() override;

private:
	void DrawMenuBar();

private:
	Olala::Ref<Olala::Scene> m_Scene;
	Olala::Ref<Olala::AssetManager> m_AssetManager;

	// Panels
	Olala::Ref<SceneViewPanel> m_SceneViewPanel;
	Olala::Ref<RuntimeViewPanel> m_RuntimeViewPanel;
	Olala::Ref<SceneHierarchyPanel> m_SceneHierarchyPanel;
	Olala::Ref<PropertyPanel> m_PropertyPanel;
	Olala::Ref<AssetPanel> m_AssetPanel;
};

