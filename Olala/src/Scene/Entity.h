#pragma once

#include "entt/entt.hpp"
#include "Scene.h"

namespace Olala {

	class Entity
	{
	public:
		Entity() = default;

		Entity(entt::entity entityID, Scene* scene)
			: m_EntityID(entityID), m_Scene(scene)
		{
		}

		template<typename T, typename ... Args>
		T& AddComponent(Args&& ... args)
		{
			return m_Scene->m_Registry.emplace<T>(m_EntityID, std::forward<Args>(args)...);
		}

		template<typename ... Args>
		void RemoveComponent()
		{
			m_Scene->m_Registry.remove_if_exists<Args...>(m_EntityID);
		}

		template<typename ... Args>
		decltype(auto) GetComponent()
		{
			return m_Scene->m_Registry.get<Args...>(m_EntityID);
		}

		template<typename ... Args>
		bool HasComponent()
		{
			return m_Scene->m_Registry.has<Args...>(m_EntityID);
		}

	private:
		entt::entity m_EntityID;
		Scene* m_Scene;
	};

}