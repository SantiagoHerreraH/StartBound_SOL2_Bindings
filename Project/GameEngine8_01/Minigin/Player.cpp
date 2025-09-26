#include "Player.h"
#include "PlayerController.h"
#include "Collider.h"
#include "Rigidbody.h"
#include "Movement.h"
#include "Texture.h"
#include "StatSystem.h"
#include "Text.h"
#include "TimerSystem.h"
#include "EventTriggerCommand.h"

#include "CollisionLayers.h"
#include "Bullet.h"
#include "Gun.h"
#include "Audio.h"
#include "ServiceLocator.h"
#include "Settings.h"
#include "Boundary.h"
#include "Animation.h"
#include "HealthBar.h"

#undef max
#undef min
#include <limits>
#include "Enemy.h"

dae::Player::Player(std::string sceneNameOnPlayerDie, const PlayerType& playerType)
{
	glm::vec2 startPos{g_WindowWidth/2.f, g_WindowHeight/2.f};

	//------- PLAYER

	dae::GameObjectHandle currentPlayer = std::make_shared<GameObject>();

	//----- COMPONENTS -----

	SpriteSheet spriteSheet{ CTextureHandle{"PlayerStar.png"}, 1, 3 };

	CAnimation animation{};
	animation.SetFramesPerSecond(6);
	animation.CanLoop(true);
	animation.AddFrame(spriteSheet.GetFrame(0, 0), 1);
	animation.AddFrame(spriteSheet.GetFrame(0, 1), 1);
	animation.AddFrame(spriteSheet.GetFrame(0, 2), 1);

	currentPlayer->AddComponent(animation);

	//------

	HealthBarData healthBarData{};
	healthBarData.Color = RGB(255, 0, 0);
	healthBarData.Rect.Top = -26;
	healthBarData.Rect.Left = -15;
	healthBarData.Rect.Width = 30;
	healthBarData.Rect.Height = 5;

	currentPlayer->AddComponent(CHealthBar{ healthBarData });

	//------

	GunData gunData{};
	gunData.GunType = playerType.GunType;
	gunData.BulletCollisionLayer = CollisionLayers::PlayerBullet;
	gunData.BulletCollisionLayerToCollideAgainst = CollisionLayers::Enemy;
	gunData.Shooter = currentPlayer;
	//gunData.ShootAudio;

	CGun gun{ gunData };

	currentPlayer->AddComponent(gun);

	//------

	Rect rect{};
	rect.Height = spriteSheet.GetScaledCellHeight();
	rect.Width  = spriteSheet.GetScaledCellWidth();

	dae::CCollider collider{ rect, (int)CollisionLayers::Player, true};
	collider.CenterRect();
	collider.RenderCollider(true, false, RGB(0,255,0));

	currentPlayer->AddComponent(collider);

	//-----

	dae::CRigidbody2D rigidBody{};
	rigidBody.SetMaxSpeed(250);
	currentPlayer->AddComponent(rigidBody);

	//-----

	dae::CMovement2D movement{};
	movement.SetMaxSpeed(30000);
	movement.SetMaxAccelForce(100000);
	currentPlayer->AddComponent(movement);


	//-----

	auto playerControllerRef = currentPlayer->AddComponent(CPlayerController{});

	//-----

	CGameStatController statController{};

	int maxHealth = playerType.MaxHealth < 1 ? 1 : playerType.MaxHealth;
	GameStat initialHealth{ maxHealth, maxHealth, maxHealth };


	constexpr int maxInt = std::numeric_limits<int>::max();

	GameStat initialPoints{ 0, maxInt, maxInt };
	GameStat initialShotsFired{ 0, maxInt, maxInt };
	GameStat initialNumberOfHits{ 0, maxInt, maxInt };

	statController.CreateStat(StatType::Health, initialHealth);

	statController.CreateStat(StatType::Points, initialPoints);
	statController.CreateStat(StatType::ShotsFired, initialShotsFired);
	statController.CreateStat(StatType::NumberOfHits, initialNumberOfHits);

	currentPlayer->AddComponent(statController);



	//----

	Rect rectBoundary{};
	rectBoundary.Top = 0;
	rectBoundary.Left = 0;
	rectBoundary.Height = g_WindowHeight;
	rectBoundary.Width = g_WindowWidth;

	currentPlayer->AddComponent(CBoundary{rectBoundary});

	//-----

	currentPlayer->Transform().SetLocalPositionX(startPos.x);
	currentPlayer->Transform().SetLocalPositionY(startPos.y);

	//----- ACTIONS -----

	// - Movement Action
	auto moveLeft = [](GameObject& gameObj) mutable
		{
			gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ -1,  0 });
		};
	auto moveRight = [](GameObject& gameObj) mutable
		{
			gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 1,  0 });
		};
	auto moveUp = [](GameObject& gameObj) mutable
		{
			gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 0,  -1 });
		};
	auto moveDown = [](GameObject& gameObj) mutable
		{
			gameObj.GetComponent<CMovement2D>()->AddSingleFrameMovementInput(glm::vec2{ 0,  1 });
		};


	// - Shooting Action

	auto shootAction = [](GameObject& self) mutable 
		{ 
			glm::vec2 delta = InputManager::GetCurrent().MousePos() - glm::ivec2(self.Transform().GetWorldTransform().Position);
			self.GetComponent<CGun>()->Shoot(delta);
		
		};

	//Events

	Event<GameObject&> shootEvent{};
	Event<GameObject&> moveLeftEvent{};
	Event<GameObject&> moveRightEvent{};
	Event<GameObject&> moveUpEvent{};
	Event<GameObject&> moveDownEvent{};

	moveLeftEvent.Subscribe(moveLeft);
	moveRightEvent.Subscribe(moveRight);
	moveUpEvent.Subscribe(moveUp);
	moveDownEvent.Subscribe(moveDown);
	shootEvent.Subscribe(shootAction);

	playerControllerRef->BindMouseEvent(dae::PlayerMouseEventData{ ButtonState::BUTTON_DOWN, true, std::make_shared<EventTriggerCommand>(shootEvent) });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, VK_LEFT,	std::make_shared<EventTriggerCommand>(moveLeftEvent) });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, VK_RIGHT,	std::make_shared<EventTriggerCommand>(moveRightEvent) });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, VK_UP,	std::make_shared<EventTriggerCommand>(moveUpEvent) });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, VK_DOWN,	std::make_shared<EventTriggerCommand>(moveDownEvent) });


	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, _T('A'),	 std::make_shared<EventTriggerCommand>(moveLeftEvent) });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, _T('D'),	 std::make_shared<EventTriggerCommand>(moveRightEvent) });

	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, _T('W'),	 std::make_shared<EventTriggerCommand>(moveUpEvent) });
	playerControllerRef->BindKey(dae::PlayerKeyboardKeyData{ ButtonState::BUTTON_PRESSED, _T('S'),	 std::make_shared<EventTriggerCommand>(moveDownEvent) });


	m_CurrentPlayer = currentPlayer;

	SubscribeOnDie([sceneNameOnPlayerDie, currentPlayer]() mutable
		{
			SceneManager::GetInstance().ChangeCurrentScene(sceneNameOnPlayerDie);
		});

}

dae::GameObjectHandle dae::Player::GetGameObjectHandle()
{
	return m_CurrentPlayer;
}

void dae::Player::SubscribeOnDie(const std::function<void()>& func)
{
	m_CurrentPlayer->GetComponent<CGameStatController>()->OnCurrentStatZeroOrLess(StatType::Health).Subscribe(func);
}

void dae::Player::AddScene(Scene& scene)
{
	scene.AddGameObjectHandle(m_CurrentPlayer);

}

void dae::BindPlayerShoot(GameObjectHandle player)
{

	auto playerControllerRef = player->GetComponent<CPlayerController>();

	auto shootAction = [](GameObject& self) mutable
		{
			glm::vec2 delta = InputManager::GetCurrent().MousePos() - glm::ivec2(self.Transform().GetWorldTransform().Position);
			self.GetComponent<CGun>()->Shoot(delta);

		};

	Event<GameObject&> shootEvent{};
	shootEvent.Subscribe(shootAction);
	playerControllerRef->BindMouseEvent(dae::PlayerMouseEventData{ ButtonState::BUTTON_DOWN, true, std::make_shared<EventTriggerCommand>(shootEvent) });

}
