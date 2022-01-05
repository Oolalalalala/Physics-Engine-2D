#include "PropertyPanel.h"

PropertyPanel::PropertyPanel()
	: Panel("Property")
{
}

PropertyPanel::~PropertyPanel()
{
}

void PropertyPanel::OnUpdate()
{
}

void PropertyPanel::OnImGuiRender()
{
	if (m_IsOpen)
	{
		ImGui::Begin(m_Name.c_str(), &m_IsOpen);
		if (m_DisplayedEntity)
			DrawContext();

		m_IsFocused = ImGui::IsWindowFocused();
		ImGui::End();
	}
}

template<typename T>
static void DrawComponent(const std::string& name, Olala::Entity& entity, std::function<void(T&)> uiFunction)
{
	constexpr ImGuiTreeNodeFlags componentFlags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth;

	if (entity.HasComponent<T>())
	{
		bool opened = ImGui::CollapsingHeader(name.c_str(), componentFlags);
		bool shouldRemove = false;

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::Selectable("Remove")) shouldRemove = true;
			ImGui::EndPopup();
		}

		if (opened) uiFunction(entity.GetComponent<T>());
		if (shouldRemove) entity.RemoveComponent<T>();
	}
}

void PropertyPanel::DrawContext()
{
	auto& entity = m_DisplayedEntity;

	ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
	constexpr float addButtonWidth = 50.f;

	// Entity Name
	if (entity.HasComponent<Olala::TagComponent>())
	{
		auto& tag = entity.GetComponent<Olala::TagComponent>();
		constexpr size_t maxLength = 30;
		char name[maxLength];
		strcpy_s(name, tag.Tag.c_str());
		ImGui::PushItemWidth(contentRegionAvailable.x - addButtonWidth - 10.f);
		ImGui::InputText("", name, maxLength);
		ImGui::PopItemWidth();
		if (strlen(name))
			tag.Tag = name;
	}
	
	// Add component
	{
		ImGui::SameLine(0.f, 10.f);
		if (ImGui::Button("Add", { addButtonWidth, 20.f }))
		{
			ImGui::OpenPopup("Add component");
		}

		constexpr char* componentList[] = { "Camera", "Sprite Renderer 2D", "Rigidbody 2D", "Box Collider 2D", "Circle Collider 2D"};
		if (ImGui::BeginPopup("Add component"))
		{
			for (int i = 0; i < 5; i++)
			{
				if (ImGui::Selectable(componentList[i]))
				{
					switch (i)
					{
						case 0:
						{
							if (!entity.HasComponent<Olala::CameraComponent>())
							{
								auto& camera = entity.AddComponent<Olala::CameraComponent>();
								camera.Camera = Olala::CreateRef<Olala::PerspectiveCamera>();
							}
							break;
						}
						case 1:
						{
							if (!entity.HasComponent<Olala::SpriteRendererComponent>())
							{
								auto& sprite = entity.AddComponent<Olala::SpriteRendererComponent>();
							}
							break;
						}
						case 2:
						{
							if (!entity.HasComponent<Olala::Rigidbody2DComponent>())
							{
								auto& rigidbody2d = entity.AddComponent<Olala::Rigidbody2DComponent>();
								auto physicsWorld = entity.GetPhysicsWorld();
								rigidbody2d.PhysicsHandle = physicsWorld->CreatePhysicsBody();
								auto& physicsBody = physicsWorld->GetPhysicsBody(rigidbody2d.PhysicsHandle);
								auto& transform = entity.GetComponent<Olala::TransformComponent>();
								physicsBody.Position = (glm::vec2)transform.Position;
								physicsBody.Rotation = transform.Rotation.y;
							}
							break;
						}
						case 3:
						{
							if (!entity.HasAnyComponent<Olala::BoxCollider2DComponent, Olala::CircleCollider2DComponent>())
							{
								if (entity.HasComponent<Olala::Rigidbody2DComponent>())
								{
									auto& rigidbody2d = entity.GetComponent<Olala::Rigidbody2DComponent>();
									auto physicsWorld = entity.GetPhysicsWorld();
									physicsWorld->GetPhysicsBody(rigidbody2d.PhysicsHandle).SetColliderType(Olala::ColliderType::BoundingBox);
								}
								else
								{
									auto& rigidbody2d = entity.AddComponent<Olala::Rigidbody2DComponent>();
									auto physicsWorld = entity.GetPhysicsWorld();
									rigidbody2d.PhysicsHandle = physicsWorld->CreatePhysicsBody(Olala::ColliderType::BoundingBox);
								}
								entity.AddComponent<Olala::BoxCollider2DComponent>();
							}
							break;
						}
						case 4:
						{
							if (!entity.HasAnyComponent<Olala::BoxCollider2DComponent, Olala::CircleCollider2DComponent>())
							{
								if (entity.HasComponent<Olala::Rigidbody2DComponent>())
								{
									auto& rigidbody2d = entity.GetComponent<Olala::Rigidbody2DComponent>();
									auto physicsWorld = entity.GetPhysicsWorld();
									physicsWorld->GetPhysicsBody(rigidbody2d.PhysicsHandle).SetColliderType(Olala::ColliderType::BoundingCircle);
								}
								else
								{
									auto& rigidbody2d = entity.AddComponent<Olala::Rigidbody2DComponent>();
									auto physicsWorld = entity.GetPhysicsWorld();
									rigidbody2d.PhysicsHandle = physicsWorld->CreatePhysicsBody(Olala::ColliderType::BoundingCircle);
								}
								entity.AddComponent<Olala::CircleCollider2DComponent>();
							}
							break;
						}
					}
				}
			}
			ImGui::EndPopup();
		}
	}

	ImGui::Separator();

	// Transform Component
	DrawComponent<Olala::TransformComponent>("Transform", entity, [](auto& transform)
	{
		ImGui::DragFloat3("Position", (float*)&transform.Position, 2);
		ImGui::DragFloat3("Rotation", (float*)&transform.Rotation, 2);
		ImGui::DragFloat3("Scale", (float*)&transform.Scale, 2);
	});

	// Camera Component
	DrawComponent<Olala::CameraComponent>("Camera", entity, [](auto& camera)
	{
		constexpr const char* projectionTypes[] = { "Perspective", "Orthographic" };
		int selectedProjectionType = (int)camera.Camera->GetProjectionType();

		ImGui::Checkbox("On", &camera.IsOn);
		ImGui::Checkbox("Main", &camera.IsMain);
		if (ImGui::BeginCombo("Projection type", projectionTypes[selectedProjectionType]))
		{
			for (int i = 0; i < 2; i++)
			{
				bool isSelected = selectedProjectionType == i;
				if (ImGui::Selectable(projectionTypes[i], &isSelected))
				{
					// Change the camera type
					if (i == (int)Olala::CameraProjectionType::Perspective)
					{
						camera.Camera = Olala::CreateRef<Olala::PerspectiveCamera>();
					}
					else
					{
						camera.Camera = Olala::CreateRef<Olala::OrthographicCamera>();
					}
				}
			}
			ImGui::EndCombo();
		}

		if (camera.Camera->GetProjectionType() == Olala::CameraProjectionType::Perspective)
		{
			auto& specs = std::static_pointer_cast<Olala::PerspectiveCamera>(camera.Camera)->GetSpecs();
			auto temp = specs;
			ImGui::InputFloat("FOV", &temp.Fov);
			ImGui::InputFloat("Aspect Ratio", &temp.AspectRatio);
			ImGui::InputFloat("Near Plane", &temp.NearPlane);
			ImGui::InputFloat("Far Plane", &temp.FarPlane);
			if (temp != specs)
			{
				specs = temp;
				camera.Camera->RecalculateProjectionMatrix();
			}
		}
		else
		{
			auto& specs = std::static_pointer_cast<Olala::OrthographicCamera>(camera.Camera)->GetSpecs();
			auto temp = specs;
			ImGui::InputFloat("Width", &temp.Width);
			ImGui::InputFloat("Height", &temp.Height);
			ImGui::InputFloat("Near Plane", &temp.NearPlane);
			ImGui::InputFloat("Far Plane", &temp.FarPlane);
			if (temp != specs)
			{
				specs = temp;
				camera.Camera->RecalculateProjectionMatrix();
			}
		}
	});
	
	// Sprite Renderer Component
	DrawComponent<Olala::SpriteRendererComponent>("Sprite Renderer", entity, [](auto& sprite)
	{
		ImGui::DragFloat2("Size", (float*)&sprite.Size);
		ImGui::ColorEdit4("Color", (float*)&sprite.Color);
	});

	// Rigidbody 2D
	DrawComponent<Olala::Rigidbody2DComponent>("Rigidbody 2D", entity, [](auto& rigidbody2d)
	{
		ImGui::DragFloat("Mass", &rigidbody2d.Mass);
		ImGui::Checkbox("Gravity", &rigidbody2d.ApplyGravity);
	});

	// Box Collider 2D
	DrawComponent<Olala::BoxCollider2DComponent>("Box Collider 2D", entity, [](auto& boxCollider2d)
	{
		ImGui::DragFloat2("Center", (float*)&boxCollider2d.Center);
		ImGui::DragFloat2("Size", (float*)&boxCollider2d.Size);

	});

	// Circle Collider 2D
	DrawComponent<Olala::CircleCollider2DComponent>("Circle Collider 2D", entity, [](auto& circleCollider2d)
	{
		ImGui::DragFloat2("Center", (float*)&circleCollider2d.Center);
		ImGui::DragFloat("Radius", &circleCollider2d.Radius);
	});
}

void PropertyPanel::SetDisplayedEntity(const Olala::Entity& entity)
{
	m_DisplayedEntity = entity;
}
