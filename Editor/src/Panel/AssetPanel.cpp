#include "AssetPanel.h"

AssetPanel::AssetPanel(Olala::Ref<Olala::AssetManager> assetManager)
	: Panel("Asset")
{
	m_AssetManager = assetManager;
}

AssetPanel::~AssetPanel()
{
}

void AssetPanel::OnImGuiRender()
{
	if (m_IsOpen)
	{
		ImGui::Begin(m_Name.c_str(), &m_IsOpen);

		switch (m_CurrentDirectory)
		{
			case AssetDirectory::Root:
			{
				if (ImGui::Button("Texture")) m_CurrentDirectory = AssetDirectory::Texture;
				break;
			}
			case AssetDirectory::Texture:
			{
				if (ImGui::Button("Back")) m_CurrentDirectory = AssetDirectory::Root;

				auto& textures = m_AssetManager->GetPool<Olala::Texture2D>().GetAll();
				for (auto& texture : textures)
				{
					ImGui::Button(texture.first.c_str());
				}

				break;
			}
		}

		m_IsFocused = ImGui::IsWindowFocused();

		ImGui::End();
	}
}

void AssetPanel::SetAssetManager(Olala::Ref<Olala::AssetManager> assetManager)
{
	m_AssetManager = assetManager;
}