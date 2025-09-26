#pragma once
#include <sol/sol.hpp>
#include <vector>

namespace dae {

	class LuaController final
	{
	public:
		void CreateGame();
	private:
		void FetchLuaFile();
		void CreateBindings();

		void Bind_SceneClasses(sol::state& lua);
		void Bind_Enums(sol::state& lua);
		void Bind_Plain_Structs(sol::state& lua);
		void Bind_Components(sol::state& lua);
		void Bind_GameObject(sol::state& lua);
		void Bind_Events(sol::state& lua);

		//------
		void CreateGameExample();
		sol::state m_lua;
	};

	extern inline LuaController myLuaController{};
	extern inline LuaController* LUA_CONTROLLER{ &myLuaController };
}
