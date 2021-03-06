#pragma once

#include "PhysicsBody.h"

#include <utility>
#include "glm/glm.hpp"

namespace Olala {

	struct CollisionData
	{
		float Depth;
		glm::vec2 Normal; // Normal pointing from A to B
	};

	class Collision
	{
	public:
		template<typename A, typename B>
		static bool TestCollision(const PhysicsBody& a, const PhysicsBody& b, CollisionData* data);
	};

}