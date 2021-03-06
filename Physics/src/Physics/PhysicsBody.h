#pragma once

#include "Base.h"
#include "Collider.h"

#include "glm/glm.hpp"

namespace Olala {

	// Position is set to the center of mass
	struct PhysicsBody
	{
		glm::vec2 Position = glm::vec2(0.f);
		glm::vec2 Velocity = glm::vec2(0.f);
		float Rotation = 0.f;
		float AngularVelocity = 0.f;
		float InvMass = 1.f; // Value of zero means static
		float InvInertia = 1.f;
		float Restitution = 1.f;
		Ref<Collider> Collider;

		bool ApplyGravity = true;

		void SetColliderType(ColliderType type)
		{
			switch (type)
			{
			case ColliderType::None:            Collider = nullptr;                      break;
			case ColliderType::BoundingBox:     Collider = CreateRef<BoundingBox>();     break;
			case ColliderType::BoundingCircle:  Collider = CreateRef<BoundingCircle>();  break;
			case ColliderType::BoundingPolygon: Collider = CreateRef<BoundingPolygon>(); break;
			default: break; //error
			}
		}
	};

}