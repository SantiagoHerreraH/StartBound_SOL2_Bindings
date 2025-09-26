#include "LuaController.h"
#include "GameEngine.h"
#include <vector>

#include "../Minigin/Gun.h"
#include "../Minigin/Rect.h"
#include "../Minigin/Player.h"
#include "../Minigin/Follow.h"
#include "../Minigin/Enemy.h"
#include "../Minigin/MainMenu.h"
#include "../Minigin/RoundManager.h" 
#include "../Minigin/NameAssignerScene.h" 
#include "../Minigin/HighscoreScene.h"
#include "../Minigin/SceneManager.h"  

#include "../Minigin/PlayerController.h"
#include "../Minigin/Collider.h"    
#include "../Minigin/Component.h"            
#include "../Minigin/GameObject.h"
#include "../Minigin/Animation.h"
#include "../Minigin/HealthBar.h"
#include "../Minigin/Texture.h"
#include "../Minigin/ForceApplier.h"
#include "../Minigin/StatModifier.h"
#include "../Minigin/Rigidbody.h"
#include "../Minigin/EnemyAI.h"
#include "../Minigin/Movement.h"
#include "../Minigin/GameStats.h"      
#include "../Minigin/Audio.h"          
#include "../Minigin/TimerSystem.h"  
#include "../Minigin/Boundary.h"
#include "../Minigin/EventSystem.h"        
#include "../Minigin/EventTriggerCommand.h"
#include "../Minigin/InputManager.h"
#include "../Minigin/Settings.h"

namespace dae {

    void LuaController::FetchLuaFile()
    {

        sol::protected_function_result result = m_lua.safe_script_file("scripts.lua");

        if (!result.valid()) {
            sol::error err = result;
            std::cerr << "Lua load error: " << err.what() << "\n";
        }
        else {
            std::cout << "Lua script loaded successfully!\n";
        }
    }

    void LuaController::CreateBindings()
    {
        luaL_openlibs(m_lua.lua_state());
        Bind_SceneClasses(m_lua);
        Bind_Enums(m_lua);
        Bind_Plain_Structs(m_lua);
        Bind_Components(m_lua);
        Bind_GameObject(m_lua);
        Bind_Events(m_lua);
    }

	COLORREF LuaRGB(int r, int g, int b) {
		return RGB(r, g, b);
	}

	void LuaController::Bind_SceneClasses(sol::state& lua) {

        lua["g_WindowWidth"] = dae::g_WindowWidth;
        lua["g_WindowHeight"] = dae::g_WindowHeight;

        

        lua.set_function("ChangeCurrentScene", [](const std::string& sceneName) {
            SceneManager::GetInstance().ChangeCurrentScene(sceneName);
            });

		// Make RGB() accessible from Lua
        lua.set_function("RGB", &LuaRGB);


		// MainMenuData
        lua.new_usertype<MainMenuData>("MainMenuData",
			sol::constructors<MainMenuData()>(),
			"SceneName", &MainMenuData::SceneName,
			"NextSceneName", &MainMenuData::NextSceneName
		);

		// MainMenu
		lua.new_usertype<MainMenu>("MainMenu",
			sol::constructors<MainMenu(MainMenuData)>(),
			"GetName", &MainMenu::GetName
		);

		// RoundParams
		lua.new_usertype<RoundParams>("RoundParams",
			sol::constructors<RoundParams()>(),
			"SecondsBetweenSpawning", &RoundParams::SecondsBetweenSpawning,
			"InitialMaxEnemyNumber", &RoundParams::InitialMaxEnemyNumber,
			"SecondsToIncreaseMaxEnemyNumByOne", &RoundParams::SecondsToIncreaseMaxEnemyNumByOne,
			"EnemyTypes", &RoundParams::EnemyTypes,
			"AddEnemyType", &RoundParams::AddEnemyType
		);

		// RoundManager
		lua.new_usertype<RoundManager>("RoundManager",
			sol::constructors<RoundManager(const RoundParams&, const std::string&, GameObjectHandle)>(),
			"GetName", &RoundManager::GetName
		);

		// NameAssignerSceneData
		lua.new_usertype<NameAssignerSceneData>("NameAssignerSceneData",
			sol::constructors<NameAssignerSceneData()>(),
			"Player", &NameAssignerSceneData::Player,
			"SceneName", &NameAssignerSceneData::SceneName,
			"SceneNameUponCompletion", &NameAssignerSceneData::SceneNameUponCompletion
		);

		// NameAssignerScene
		lua.new_usertype<NameAssignerScene>("NameAssignerScene",
			sol::constructors<NameAssignerScene(const NameAssignerSceneData&)>()
		);

		// HighscoreScene
		lua.new_usertype<HighscoreScene>("HighscoreScene",
			sol::constructors<HighscoreScene(const std::string&, std::string, GameObjectHandle)>()
		);


        lua.set_function("BindPlayerShoot", &dae::BindPlayerShoot);

		// glm::vec2
		lua.new_usertype<glm::vec2>("vec2",
			sol::constructors<glm::vec2(), glm::vec2(float, float)>(),
			"x", &glm::vec2::x,
			"y", &glm::vec2::y
		);


		// std::vector<glm::vec2>
		lua.new_usertype<std::vector<glm::vec2>>("Vec2Array",
			sol::constructors<std::vector<glm::vec2>>(),
			sol::meta_function::index, [](std::vector<glm::vec2>& v, int i) -> glm::vec2& { return v[i - 1]; },
			sol::meta_function::length, [](std::vector<glm::vec2>& v) { return v.size(); }
		);
	}

    void LuaController::Bind_Enums(sol::state& lua) {
        lua.new_enum("ButtonState",
            "BUTTON_DOWN", ButtonState::BUTTON_DOWN,
            "BUTTON_PRESSED", ButtonState::BUTTON_PRESSED,
            "BUTTON_UP", ButtonState::BUTTON_UP
        );
        
        lua.new_enum("CollisionLayers",
            "Player", CollisionLayers::Player,
            "Enemy", CollisionLayers::Enemy,
            "EnemyBullet", CollisionLayers::EnemyBullet,
            "PlayerBullet", CollisionLayers::PlayerBullet
        );

        lua.new_enum("ForceMode2D",
            "Force", ForceMode2D::Force,
            "Impulse", ForceMode2D::Impulse
        );

        lua.new_enum("SetVelocityMode",
            "NullForces", SetVelocityMode::NullForces,
            "KeepForces", SetVelocityMode::KeepForces
        );

        lua.new_enum("StatCategory",
            "CurrentStat", StatCategory::CurrentStat,
            "MaxStat", StatCategory::MaxStat,
            "MaxLimitStat", StatCategory::MaxLimitStat
        );

        lua.new_enum("StatType",
            "Health", StatType::Health,
            "Points", StatType::Points,
            "ShotsFired", StatType::ShotsFired,
            "NumberOfHits", StatType::NumberOfHits
        );

        lua.new_enum("StatModifierType",
            "Offset", StatModifierType::Offset,
            "Scale", StatModifierType::Scale
        );
    }

    void LuaController::Bind_Plain_Structs(sol::state& lua) {
        
        //EnemyType
        m_lua.new_usertype<EnemyType>("EnemyType",
            sol::constructors<EnemyType()>(),
            "ImageFile", &EnemyType::ImageFile,
            "FollowData", &EnemyType::FollowData,
            "GunType", &EnemyType::GunType,
            "DistanceFromTargetToShoot", &EnemyType::DistanceFromTargetToShoot,
            "BodyDamage", &EnemyType::BodyDamage,
            "BodyPushForce", &EnemyType::BodyPushForce,
            "MaxHealth", &EnemyType::MaxHealth,
            "Points", &EnemyType::Points
        );

        // KeyboardKeyData
        auto kbd = lua.new_usertype<KeyboardKeyData>("KeyboardKeyData",
            sol::constructors<KeyboardKeyData()>(),
            "ButtonState", &KeyboardKeyData::ButtonState,
            "Key", &KeyboardKeyData::Key,
            "PlayerId", &KeyboardKeyData::PlayerId,
            "OnTriggered", &KeyboardKeyData::OnTriggered
            // PlayerGameObject set/get via helper methods below
        );
        kbd.set_function("SetPlayerGameObject", sol::overload(
            [](KeyboardKeyData& d, GameObject& go) { d.PlayerGameObject = &go; },
            [](KeyboardKeyData& d, GameObject* p) { d.PlayerGameObject = p; },
            [](KeyboardKeyData& d, const std::shared_ptr<GameObject>& h) { d.PlayerGameObject = h.get(); }
        ));
        kbd.set_function("GetPlayerGameObject", [](KeyboardKeyData& d) -> GameObject* {
            return d.PlayerGameObject;
            });

        // MouseEventData
        auto ms = lua.new_usertype<MouseEventData>("MouseEventData",
            sol::constructors<MouseEventData()>(),
            "ButtonState", &MouseEventData::ButtonState,
            "IsLeft", &MouseEventData::IsLeft,
            "PlayerId", &MouseEventData::PlayerId,
            "OnTriggered", &MouseEventData::OnTriggered
        );
        ms.set_function("SetPlayerGameObject", sol::overload(
            [](MouseEventData& d, GameObject& go) { d.PlayerGameObject = &go; },
            [](MouseEventData& d, GameObject* p) { d.PlayerGameObject = p; },
            [](MouseEventData& d, const std::shared_ptr<GameObject>& h) { d.PlayerGameObject = h.get(); }
        ));
        ms.set_function("GetPlayerGameObject", [](MouseEventData& d) -> GameObject* {
            return d.PlayerGameObject;
            });
        
        
        // Rect
        lua.new_usertype<Rect>("Rect",
            sol::constructors<Rect()>(),
            "Top", &Rect::Top,
            "Left", &Rect::Left,
            "Width", &Rect::Width,
            "Height", &Rect::Height,
            "Right", &Rect::Right,
            "Bottom", &Rect::Bottom
        );
        
        lua.new_usertype<HealthBarData>("HealthBarData",
            sol::constructors<HealthBarData()>(),
            "Color", &HealthBarData::Color,
            "Rect", &HealthBarData::Rect
        );

        lua.new_usertype<ForceApplierData>("ForceApplierData",
            sol::constructors<ForceApplierData()>(),
            "PushForceOnStartCollision", &ForceApplierData::PushForceOnStartCollision,
            "PushForceOnStayCollision", &ForceApplierData::PushForceOnStayCollision,
            "PushForceOnEndCollision", &ForceApplierData::PushForceOnEndCollision
        );

        lua.new_usertype<GunType>("GunType",
            sol::constructors<GunType()>(),
            "BulletImageFile", &GunType::BulletImageFile,
            "BulletAmount", &GunType::BulletAmount,
            "BulletLifeTime", &GunType::BulletLifeTime,
            "BulletSpeed", &GunType::BulletSpeed,
            "TimeBetweenShots", &GunType::TimeBetweenShots,
            "BulletPushForce", &GunType::BulletPushForce,
            "BulletDamage", &GunType::BulletDamage
        );

        lua.new_usertype<GunData>("GunData",
            sol::constructors<GunData()>(),
            "GunType", &GunData::GunType,
            "BulletCollisionLayer", &GunData::BulletCollisionLayer,
            "BulletCollisionLayerToCollideAgainst", &GunData::BulletCollisionLayerToCollideAgainst,
            "Shooter", &GunData::Shooter
        );

        lua.new_usertype<StatModifierData>("StatModifierData",
            sol::constructors<StatModifierData()>(),
            "StatToModify", &StatModifierData::StatToModify,
            "StatModifierType", &StatModifierData::StatModifierType,
            "InitialStatValue", &StatModifierData::InitialStatValue
        );

        lua.new_usertype<FollowData>("FollowData",
            sol::constructors<FollowData()>(),
            "PossibleMovementDirections", &FollowData::PossibleMovementDirections,
            "Speed", &FollowData::Speed,
            "TimeToChangeDirection", &FollowData::TimeToChangeDirection,
            "OptimalDistanceFromTarget", &FollowData::OptimalDistanceFromTarget
        );

        lua.new_usertype<EnemyAIData>("EnemyAIData",
            sol::constructors<EnemyAIData()>(),
            "Target", &EnemyAIData::Target,
            "DistanceFromTargetToShoot", &EnemyAIData::DistanceFromTargetToShoot
        );

        // Expose GameStat (Stat<int>) for read/write in Lua (events remain opaque).
        lua.new_usertype<GameStat>("GameStat",
            "CurrentStat", &GameStat::CurrentStat,
            "MaxStat", &GameStat::MaxStat,
            "MaxLimitStat", &GameStat::MaxLimitStat
            // Events (OnResetCurrentStat, etc.) can be exposed if your Event<> is Lua-bindable.
        );

        lua.new_usertype<PlayerKeyboardKeyData>("PlayerKeyboardKeyData",
            sol::constructors<PlayerKeyboardKeyData()>(),
            "ButtonState", &PlayerKeyboardKeyData::ButtonState,   // bind ButtonState enum elsewhere for nice names
            "Key", &PlayerKeyboardKeyData::Key,           // bind KeyboardKey enum elsewhere for nice names
            "OnTriggered", &PlayerKeyboardKeyData::OnTriggered    // std::shared_ptr<ICommand>
        );

        lua.new_usertype<PlayerMouseEventData>("PlayerMouseEventData",
            sol::constructors<PlayerMouseEventData()>(),
            "ButtonState", &PlayerMouseEventData::ButtonState,
            "IsLeft", &PlayerMouseEventData::IsLeft,
            "OnTriggered", &PlayerMouseEventData::OnTriggered
        );
    }

    void LuaController::Bind_Components(sol::state& lua)
    {
        lua.new_usertype<CBoundary>("CBoundary",
            sol::constructors<CBoundary(const Rect&)>(),
            "Update", &CBoundary::Update
        );

        lua.new_usertype<CPlayerController>("CPlayerController",
            sol::constructors<CPlayerController()>(),
            "Start", &CPlayerController::Start,
            "SetActive", &CPlayerController::SetActive,
            "BindKey", &CPlayerController::BindKey,
            "BindMouseEvent", &CPlayerController::BindMouseEvent
        );

        lua.new_usertype<CTextureHandle>("CTextureHandle",
            sol::constructors<CTextureHandle(), CTextureHandle(const std::string&)>(),
            "Render", &CTextureHandle::Render,
            "IsValid", &CTextureHandle::IsValid,
            "SetTexture", &CTextureHandle::SetTexture,
            "SetTextureView", &CTextureHandle::SetTextureView,
            "GetTextureView", &CTextureHandle::GetTextureView,
            "Data", &CTextureHandle::Data,
            "Center", &CTextureHandle::Center,
            "SetTextureTransform", &CTextureHandle::SetTextureTransform,
            "GetTextureTransform", &CTextureHandle::GetTextureTransform
        );

        lua.new_usertype<SpriteSheet>("SpriteSheet",
            sol::constructors<
            SpriteSheet(const CTextureHandle&, unsigned int, unsigned int, unsigned int, unsigned int),
            SpriteSheet(const CTextureHandle&, unsigned int, unsigned int)
            >(),
            "GetFrame", &SpriteSheet::GetFrame,
            "TextureHandle", &SpriteSheet::TextureHandle,
            "GetOriginalCellWidth", &SpriteSheet::GetOriginalCellWidth,
            "GetOriginalCellHeight", &SpriteSheet::GetOriginalCellHeight,
            "GetScaledCellWidth", &SpriteSheet::GetScaledCellWidth,
            "GetScaledCellHeight", &SpriteSheet::GetScaledCellHeight
        );

        lua.new_usertype<CAnimation>("CAnimation",
            sol::constructors<CAnimation()>(),
            "Update", &CAnimation::Update,
            "Render", &CAnimation::Render,
            "End", &CAnimation::End,
            "CanLoop", &CAnimation::CanLoop,
            "SetFramesPerSecond", &CAnimation::SetFramesPerSecond,
            "AddFrame", &CAnimation::AddFrame,
            "ClearFrames", &CAnimation::ClearFrames,
            "LoopNumber", &CAnimation::LoopNumber,
            "Reset", &CAnimation::Reset,
            "Play", &CAnimation::Play,
            "Pause", &CAnimation::Pause,
            "GetConstTextureRef", &CAnimation::GetConstTextureRef,
            "GetCurrentFrame", &CAnimation::GetCurrentFrame,
            // This one already accepts std::function<void()>, perfect for Lua:
            "SubscribeOnEnd", &CAnimation::SubscribeOnEnd
        );

        lua.new_usertype<CHealthBar>("CHealthBar",
            sol::constructors<CHealthBar(const HealthBarData&)>(),
            "Start", &CHealthBar::Start,
            "Render", &CHealthBar::Render
        );

        lua.new_usertype<CForceApplier>("CForceApplier",
            sol::constructors<CForceApplier(const ForceApplierData&)>(),
            "Start", &CForceApplier::Start
        );

        lua.new_usertype<CGun>("CGun",
            sol::constructors<CGun(const GunData&)>(),
            "Start", &CGun::Start,
            "Shoot", &CGun::Shoot
            // Internals like audio/timers/bullets are intentionally not exposed.
        );

        lua.new_usertype<CStatModifier>("CStatModifier",
            sol::constructors<CStatModifier(const StatModifierData&)>(),
            "Start", &CStatModifier::Start,
            "Stat", &CStatModifier::Stat  // returns GameStat&
        );

        lua.new_usertype<dae::CCollider>("CCollider",
            sol::constructors<CCollider(const Rect&, const CollisionTag&, bool)>(),
            "Start", &CCollider::Start,
            "FixedUpdate", &CCollider::FixedUpdate,
            "Render", &CCollider::Render,
            "SetActive", &CCollider::SetActive,

            "SetRectOffset", &CCollider::SetRectOffset,
            "CenterRect", &CCollider::CenterRect,
            "SetOriginalRect", &CCollider::SetOriginalRect,
            "GetOriginalRect", &CCollider::GetOriginalRect,

            "GetCollisionTag", &CCollider::GetCollisionTag,
            "AddCollisionTagToCollideWith", &CCollider::AddCollisionTagToCollideWith,

            "RenderCollider", &CCollider::RenderCollider
        );

        lua.new_usertype<CRigidbody2D>("CRigidbody2D",
            sol::constructors<CRigidbody2D()>(),
            "FixedUpdate", &CRigidbody2D::FixedUpdate,

            "SetNetTorque", &CRigidbody2D::SetNetTorque,
            "SetNetAngularAcceleration", &CRigidbody2D::SetNetAngularAcceleration,
            "SetNetForce", &CRigidbody2D::SetNetForce,
            "SetNetAcceleration", &CRigidbody2D::SetNetAcceleration,

            "AddTorque", &CRigidbody2D::AddTorque,
            "AddForce", &CRigidbody2D::AddForce,
            "AddAcceleration", &CRigidbody2D::AddAcceleration,
            "AddVelocity", &CRigidbody2D::AddVelocity,
            "AddAngularVelocity", &CRigidbody2D::AddAngularVelocity,

            "GetNetTorque", &CRigidbody2D::GetNetTorque,
            "GetNetAngularAcceleration", &CRigidbody2D::GetNetAngularAcceleration,
            "GetNetAcceleration", &CRigidbody2D::GetNetAcceleration,
            "GetNetForce", &CRigidbody2D::GetNetForce,

            "GetMass", &CRigidbody2D::GetMass,
            "SetMass", &CRigidbody2D::SetMass,

            "SetMaxSpeed", &CRigidbody2D::SetMaxSpeed,
            "ClampVelocity", &CRigidbody2D::ClampVelocity,
            "SetVelocityX", &CRigidbody2D::SetVelocityX,
            "SetVelocityY", &CRigidbody2D::SetVelocityY,
            "SetVelocity", &CRigidbody2D::SetVelocity,

            "GetVelocityX", &CRigidbody2D::GetVelocityX,
            "GetVelocityY", &CRigidbody2D::GetVelocityY,
            "GetVelocity", &CRigidbody2D::GetVelocity,

            "GetAngularVelocityX", &CRigidbody2D::GetAngularVelocityX,
            "GetAngularVelocityY", &CRigidbody2D::GetAngularVelocityY,
            "GetAngularVelocityZ", &CRigidbody2D::GetAngularVelocityZ,
            "GetAngularVelocity", &CRigidbody2D::GetAngularVelocity
        );

        lua.new_usertype<CMovement2D>("CMovement2D",
            sol::constructors<CMovement2D()>(),
            "Start", &CMovement2D::Start,
            "FixedUpdate", &CMovement2D::FixedUpdate,
            "SetMaxSpeed", &CMovement2D::SetMaxSpeed,
            "SetMaxAccelForce", &CMovement2D::SetMaxAccelForce,
            "AddSingleFrameMovementInput", &CMovement2D::AddSingleFrameMovementInput
        );

        lua.new_usertype<CFollow>("CFollow",
            sol::constructors<CFollow(GameObjectHandle, const FollowData&)>(),
            "Start", &CFollow::Start,
            "Update", &CFollow::Update
        );

        lua.new_usertype<CEnemyAI>("CEnemyAI",
            sol::constructors<CEnemyAI(const EnemyAIData&)>(),
            "Start", &CEnemyAI::Start,
            "Update", &CEnemyAI::Update,
            "Render", &CEnemyAI::Render
        );

        using Controller = CGameStatController;

        using EventVoid = dae::Event<>;           // your zero-arg Event

        auto sub_zero = [](Controller& c, StatType key, sol::function fn) -> EventListenerKey {
            auto& ev = c.OnCurrentStatZeroOrLess(key);
            return ev.Subscribe([fn]() { if (fn.valid()) fn(); });
            };
        auto unsub_zero = [](Controller& c, StatType key, EventListenerKey k) -> bool {
            auto& ev = c.OnCurrentStatZeroOrLess(key);
            return ev.Unsubscribe(k);
            };

        auto create_stat_wrapper = [](Controller& c, StatType key, const GameStat& stat) {
            bool changed = false;
            c.CreateStat(key, stat, &changed);
            return changed; 
            };

        lua.new_usertype<Controller>("CGameStatController",
            sol::constructors<Controller()>(),
            "CreateStat", create_stat_wrapper,

            // The rest as before:
            "RemoveStat", & Controller::RemoveStat,
            "ResetCurrentStat", & Controller::ResetCurrentStat,
            "HasStat", & Controller::HasStat,
            "GetStatStruct", & Controller::GetStatStruct,
            "GetStat", & Controller::GetStat,
            "SetStat", & Controller::SetStat,
            "OffsetStat", & Controller::OffsetStat,
            "ScaleStat", & Controller::ScaleStat,
            "OnResetCurrentStatEvent", & Controller::OnResetCurrentStatEvent,
            "OnCurrentStatZeroOrLess", & Controller::OnCurrentStatZeroOrLess,
            "OnCurrentStatChange", & Controller::OnCurrentStatChange,

            "SubscribeOnCurrentStatZeroOrLess", sub_zero,
            "UnsubscribeOnCurrentStatZeroOrLess", unsub_zero
        );

    }

    void LuaController::Bind_GameObject(sol::state& lua) {
        lua["GameObjectHandle"] = lua.create_table();

        auto GameObject_type = lua.new_usertype<GameObject>("GameObject",
            sol::constructors<GameObject(), GameObject(const std::string&)>(),
            "SetName", &GameObject::SetName,
            "GetName", &GameObject::GetName,
            "AlreadyStarted", &GameObject::AlreadyStarted,
            "Start", &GameObject::Start,
            "Update", &GameObject::Update,
            "FixedUpdate", &GameObject::FixedUpdate,
            "Render", &GameObject::Render,
            "End", &GameObject::End,
            "IsActive", &GameObject::IsActive,
            "SetActive", &GameObject::SetActive,
            "RenderTransform", &GameObject::RenderTransform,
            "Transform", &GameObject::Transform,   
            "TransformConst", &GameObject::TransformConst  
        );

        // ---- AddComponent: ALL overloads in ONE sol::overload ----
        GameObject_type.set_function("AddComponent", sol::overload(

            [](GameObject& go, const CAnimation& c) { return go.AddComponent(c); },
            [](GameObject& go, const CHealthBar& c) { return go.AddComponent(c); },
            [](GameObject& go, const CTextureHandle& c) { return go.AddComponent(c); },
            [](GameObject& go, const CForceApplier& c) { return go.AddComponent(c); },
            [](GameObject& go, const CGun& c) { return go.AddComponent(c); },
            [](GameObject& go, const CStatModifier& c) { return go.AddComponent(c); },
            [](GameObject& go, const CCollider& c) { return go.AddComponent(c); },
            [](GameObject& go, const CRigidbody2D& c) { return go.AddComponent(c); },
            [](GameObject& go, const CMovement2D& c) { return go.AddComponent(c); },
            [](GameObject& go, const CFollow& c) { return go.AddComponent(c); },
            [](GameObject& go, const CEnemyAI& c) { return go.AddComponent(c); },
            [](GameObject& go, const CGameStatController& c) { return go.AddComponent(c); },
            [](GameObject& go, const CPlayerController& c) { return go.AddComponent(c); }, 
            [](GameObject& go, const CBoundary& c) { return go.AddComponent(c); },

            [](GameObject& go, const HealthBarData& d) { return go.AddComponent(CHealthBar(d)); },
            [](GameObject& go, const ForceApplierData& d) { return go.AddComponent(CForceApplier(d)); },
            [](GameObject& go, const GunData& d) { return go.AddComponent(CGun(d)); },
            [](GameObject& go, const Rect& r, const CollisionTag& tag, bool solid) {
                return go.AddComponent(CCollider(r, tag, solid));
            }
            // (Do NOT add zero-arg emplace overloads; they collide.)
        ));

        // ---- GetComponent: ALL overloads in ONE sol::overload ----
        GameObject_type.set_function("GetComponent", sol::overload(
            [](GameObject& go, const CAnimation&) { return go.GetComponentHandle<CAnimation>(); },
            [](GameObject& go, const CHealthBar&) { return go.GetComponentHandle<CHealthBar>(); },
            [](GameObject& go, const CTextureHandle&) { return go.GetComponentHandle<CTextureHandle>(); },
            [](GameObject& go, const CForceApplier&) { return go.GetComponentHandle<CForceApplier>(); },
            [](GameObject& go, const CGun&) { return go.GetComponentHandle<CGun>(); },
            [](GameObject& go, const CStatModifier&) { return go.GetComponentHandle<CStatModifier>(); },
            [](GameObject& go, const CCollider&) { return go.GetComponentHandle<CCollider>(); },
            [](GameObject& go, const CRigidbody2D&) { return go.GetComponentHandle<CRigidbody2D>(); },
            [](GameObject& go, const CMovement2D&) { return go.GetComponentHandle<CMovement2D>(); },
            [](GameObject& go, const CFollow&) { return go.GetComponentHandle<CFollow>(); },
            [](GameObject& go, const CEnemyAI&) { return go.GetComponentHandle<CEnemyAI>(); },
            [](GameObject& go, const CGameStatController&) { return go.GetComponentHandle<CGameStatController>(); },
            [](GameObject& go, const CPlayerController&) { return go.GetComponentHandle<CPlayerController>(); },
            [](GameObject& go, const CBoundary&) { return go.GetComponentHandle<CBoundary>(); }
        ));

        lua.set_function("MakeGameObject", sol::overload(
            []() -> GameObjectHandle {
                return std::make_shared<GameObject>();
            },
            [](const std::string& name) -> GameObjectHandle {
                return std::make_shared<GameObject>(name);
            }
        ));

        // vec3 (you already have vec2)
        lua.new_usertype<glm::vec3>("vec3",
            sol::constructors<glm::vec3(), glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z
        );

        // TransformData (read-only is fine; Lua often just reads Position/Rotation/Scale)
        lua.new_usertype<TransformData>("TransformData",
            sol::no_constructor,
            "Position", &TransformData::Position,
            "Rotation", &TransformData::Rotation,
            "Scale", &TransformData::Scale
        );

        // Transform (no public ctor from Lua)
        lua.new_usertype<Transform>("Transform", sol::no_constructor,
            // parenting
            "HasParent", &Transform::HasParent,
            "HasChildren", &Transform::HasChildren,
            "GetParent", &Transform::GetParent,
            "GetChildren", &Transform::GetChildren,
            "MakeRootNode", &Transform::MakeRootNode,

            // set local scale / pos / rot
            "SetLocalScaleX", &Transform::SetLocalScaleX,
            "SetLocalScaleY", &Transform::SetLocalScaleY,
            "SetLocalScaleZ", &Transform::SetLocalScaleZ,
            "SetLocalScale", &Transform::SetLocalScale,

            "SetLocalPositionX", &Transform::SetLocalPositionX,
            "SetLocalPositionY", &Transform::SetLocalPositionY,
            "SetLocalPositionZ", &Transform::SetLocalPositionZ,
            "SetLocalPosition", &Transform::SetLocalPosition,

            "MoveLocalPositionX", &Transform::MoveLocalPositionX,
            "MoveLocalPositionY", &Transform::MoveLocalPositionY,
            "MoveLocalPositionZ", &Transform::MoveLocalPositionZ,
            "MoveLocalPosition", &Transform::MoveLocalPosition,

            "SetLocalRotationX", &Transform::SetLocalRotationX,
            "SetLocalRotationY", &Transform::SetLocalRotationY,
            "SetLocalRotationZ", &Transform::SetLocalRotationZ,
            "SetLocalRotation", &Transform::SetLocalRotation,

            "MoveLocalRotationX", &Transform::MoveLocalRotationX,
            "MoveLocalRotationY", &Transform::MoveLocalRotationY,
            "MoveLocalRotationZ", &Transform::MoveLocalRotationZ,
            "MoveLocalRotation", &Transform::MoveLocalRotation,

            // matrices / transforms
            "GetWorldTransform", &Transform::GetWorldTransform,
            "GetLocalTransform", &Transform::GetLocalTransform,
            "GetWorldMatrix", &Transform::GetWorldMatrix,

            // vectors & conversions
            "GetForwardVector", &Transform::GetForwardVector,
            "GetUpVector", &Transform::GetUpVector,
            "GetRightVector", &Transform::GetRightVector,
            "LocalToWorldVec", sol::overload(
                static_cast<glm::vec3(Transform::*)(const glm::vec3&) const>(&Transform::LocalToWorldVec),
                static_cast<glm::vec2(Transform::*)(const glm::vec2&) const>(&Transform::LocalToWorldVec)
            ),
            "WorldToLocalVec", sol::overload(
                static_cast<glm::vec3(Transform::*)(const glm::vec3&) const>(&Transform::WorldToLocalVec),
                static_cast<glm::vec2(Transform::*)(const glm::vec2&) const>(&Transform::WorldToLocalVec)
            )
        );
    }


    struct LuaCommand final : public ICommand {
        sol::function fn;
        explicit LuaCommand(sol::function f) : fn(std::move(f)) {}
        void Execute(GameObject& go) override { if (fn.valid()) fn(go); }
    };

    void LuaController::Bind_Events(sol::state& lua)
    {
        lua.new_usertype<ICommand>("ICommand", sol::no_constructor,
            "Execute", &ICommand::Execute
        );

        lua.set_function("MakeCommand", [](sol::function f) {
            return std::shared_ptr<ICommand>(std::make_shared<LuaCommand>(std::move(f)));
            });


        using Event_GameObjectRef = Event<GameObject&>;

        lua.new_usertype<Event_GameObjectRef>("Event_GameObjectRef",
            sol::constructors<Event_GameObjectRef()>(),
            "Size", &Event_GameObjectRef::Size,
            "Subscribe", &Event_GameObjectRef::Subscribe,       
            "Unsubscribe", &Event_GameObjectRef::Unsubscribe,
            "UnsubscribeAll", &Event_GameObjectRef::UnsubscribeAll,
            "Invoke", &Event_GameObjectRef::Invoke
        );


        lua.new_usertype<EventTriggerCommand>("EventTriggerCommand",
            sol::base_classes, sol::bases<ICommand>(),
            sol::call_constructor, sol::factories(
                [](const Event_GameObjectRef& ev) {
                    return std::make_shared<EventTriggerCommand>(ev);
                }
            )
        );
    }


	void LuaController::CreateGame()
	{
		this->CreateBindings();
		this->FetchLuaFile();


		sol::table globals = m_lua.globals();
		std::vector<std::string> strings{};

		for (auto& kvp : globals) {
			sol::object key = kvp.first;
			sol::object value = kvp.second;

			if (key.is<std::string>() && value.is<sol::function>()) {

				strings.push_back(key.as<std::string>());
			}
		}

		auto luaSetup = m_lua["CreateGame"];
		if (luaSetup.valid())
		{
			auto setupResult = luaSetup(); //actually calls the function
			if (!setupResult.valid())
			{
				const sol::error err = setupResult;
				throw std::exception(err.what());
			}

		}
	}

	void LuaController::CreateGameExample()
	{
		//---- SCENE NAMES ----

		const std::string mainMenu_SceneName{"MainMenu"};
		const std::string nameAssigner_SceneName{"NameAssigner"};
		const std::string roundManager_SceneName{"GameRound"};
		const std::string highscore_SceneName{"Highscore"};

		//---- ENEMIES ----
		EnemyType enemyTypeOne{};

		//enemyTypeOne.FollowData.PossibleMovementDirections;// if empty, all are possible
		enemyTypeOne.FollowData.Speed = 300;
		enemyTypeOne.FollowData.TimeToChangeDirection = 2.f;
		enemyTypeOne.FollowData.OptimalDistanceFromTarget = 10.f;

		enemyTypeOne.GunType.BulletAmount = 10;
		enemyTypeOne.GunType.BulletLifeTime = 1.5f;
		enemyTypeOne.GunType.BulletSpeed = 500 ;
		enemyTypeOne.GunType.TimeBetweenShots = 0.3f ;
		enemyTypeOne.GunType.BulletPushForce = 300.f ;
		enemyTypeOne.GunType.BulletDamage = 2;

		enemyTypeOne.DistanceFromTargetToShoot = 200;

		enemyTypeOne.BodyDamage = 1;
		enemyTypeOne.BodyPushForce = 600;

		enemyTypeOne.MaxHealth = 3;
		enemyTypeOne.Points = 3;

		//---- PLAYER ----
		PlayerType playerType{};
		playerType.GunType.BulletAmount = 10;
		playerType.GunType.BulletLifeTime = 1.5f;
		playerType.GunType.BulletSpeed = 500;
		playerType.GunType.TimeBetweenShots = 0.3f;
		playerType.GunType.BulletPushForce = 300.f;
		playerType.GunType.BulletDamage = 2;


		Player player{ highscore_SceneName , playerType };

		//---- ROUND PARAMS ----
		RoundParams roundParams{};
		roundParams.EnemyTypes.push_back(enemyTypeOne);
		roundParams.InitialMaxEnemyNumber = 10;
		roundParams.SecondsBetweenSpawning = 10;
		roundParams.SecondsToIncreaseMaxEnemyNumByOne = 15;

		//---- GAME LOOP ----
		MainMenuData mainMenuData{};
		mainMenuData.SceneName = mainMenu_SceneName;
		mainMenuData.NextSceneName = nameAssigner_SceneName;
		MainMenu mainMenu{ mainMenuData };

		NameAssignerSceneData nameAssignerSceneData{};
		nameAssignerSceneData.Player = player.GetGameObjectHandle();
		nameAssignerSceneData.SceneName = nameAssigner_SceneName;
		nameAssignerSceneData.SceneNameUponCompletion = roundManager_SceneName;
		NameAssignerScene nameAssignerScene{ nameAssignerSceneData };

		RoundManager roundManager{ roundParams, roundManager_SceneName, player.GetGameObjectHandle()};

		HighscoreScene highscoreScene{ highscore_SceneName , mainMenuData.SceneName, player.GetGameObjectHandle()};

	}

}





