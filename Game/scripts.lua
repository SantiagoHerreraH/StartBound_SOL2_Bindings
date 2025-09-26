

--------------------------------------------------------------------------------
-- MISC
--------------------------------------------------------------------------------

---@type integer, integer, integer, integer
local VK_LEFT, VK_UP, VK_RIGHT, VK_DOWN = 0x25, 0x26, 0x27, 0x28

---@param left integer
---@param top integer
---@param w integer
---@param h integer
---@return Rect
local function make_rect_xywh(left, top, w, h)
  local r = Rect.new()
  r.Left, r.Top, r.Width, r.Height = left, top, w, h
  return r
end

--------------------------------------------------------------------------------
-- PLAYER
--------------------------------------------------------------------------------

---@class PlayerType
---@field MaxHealth integer
---@field GunType GunType
PlayerType = {}
PlayerType.__index = PlayerType

---@param o PlayerType|nil
---@return PlayerType
function PlayerType.new(o)
  o = o or {}
  o.MaxHealth = o.MaxHealth or 1
  o.GunType   = o.GunType   or GunType.new()
  return setmetatable(o, PlayerType)
end

---@class Player
---@field m_CurrentPlayer GameObjectHandle
Player = {}
Player.__index = Player

---@param sceneNameOnPlayerDie string
---@param playerType PlayerType|nil
---@return Player

function Player.new(sceneNameOnPlayerDie, playerType)
  local self = setmetatable({}, Player)

  playerType = playerType or PlayerType.new()

  local startPos = { x = g_WindowWidth / 2.0, y = g_WindowHeight / 2.0 }

  ----------------------------------------------------------------
  -- Create player GameObject (shared_ptr handle)
  ----------------------------------------------------------------
  local currentPlayer = MakeGameObject("Player")
  self.m_CurrentPlayer = currentPlayer

  ----------------------------------------------------------------
  -- Components
  ----------------------------------------------------------------

  -- SpriteSheet + Animation
  local sheet = SpriteSheet.new(CTextureHandle.new("PlayerStar.png"), 1, 3)
  local anim  = CAnimation.new()
  anim:SetFramesPerSecond(6)
  anim:CanLoop(true)
  anim:AddFrame(sheet:GetFrame(0, 0), 1)
  anim:AddFrame(sheet:GetFrame(0, 1), 1)
  anim:AddFrame(sheet:GetFrame(0, 2), 1)
  currentPlayer:AddComponent(anim)

  -- HealthBar
  local hbData = HealthBarData.new()
  hbData.Color    = RGB(255, 0, 0)
  hbData.Rect.Top = -26
  hbData.Rect.Left = -15
  hbData.Rect.Width = 30
  hbData.Rect.Height = 5
  currentPlayer:AddComponent(CHealthBar.new(hbData))

  -- Gun
  local gunData = GunData.new()
  gunData.GunType                              = playerType.GunType
  gunData.BulletCollisionLayer                 = CollisionLayers.PlayerBullet
  gunData.BulletCollisionLayerToCollideAgainst = CollisionLayers.Enemy
  gunData.Shooter                              = currentPlayer
  currentPlayer:AddComponent(CGun.new(gunData))

  -- Collider sized to sprite
  local cw, ch = sheet:GetScaledCellWidth(), sheet:GetScaledCellHeight()
  local rect   = make_rect_xywh(0, 0, math.floor(cw), math.floor(ch))
  local col    = CCollider.new(rect, CollisionLayers.Player, true)
  if col.CenterRect then col:CenterRect() end
  currentPlayer:AddComponent(col)

  -- Rigidbody2D
  local rb = CRigidbody2D.new()
  rb:SetMaxSpeed(250)
  currentPlayer:AddComponent(rb)

  -- Movement2D
  local mv = CMovement2D.new()
  mv:SetMaxSpeed(30000)
  mv:SetMaxAccelForce(100000)
  currentPlayer:AddComponent(mv)

  -- PlayerController
  local playerController = currentPlayer:AddComponent(CPlayerController.new())

  -- Stat Controller
  local statCtrl = CGameStatController.new()

  local maxHealth = playerType.MaxHealth
  if maxHealth < 1 then maxHealth = 1 end

  local initialHealth = GameStat.new()
  initialHealth.CurrentStat  = maxHealth
  initialHealth.MaxStat      = maxHealth
  initialHealth.MaxLimitStat = maxHealth

  local maxInt = 2147483647
  local function make_stat(cur)
    local s = GameStat.new()
    s.CurrentStat  = cur
    s.MaxStat      = maxInt
    s.MaxLimitStat = maxInt
    return s
  end

  statCtrl:CreateStat(StatType.Health,       initialHealth)
  statCtrl:CreateStat(StatType.Points,       make_stat(0))
  statCtrl:CreateStat(StatType.ShotsFired,   make_stat(0))
  statCtrl:CreateStat(StatType.NumberOfHits, make_stat(0))

  currentPlayer:AddComponent(statCtrl)

  -- Boundary
    local rectBoundary = Rect.new()
    rectBoundary.Top = 0
    rectBoundary.Left = 0
    rectBoundary.Height = g_WindowHeight
    rectBoundary.Width  = g_WindowWidth
    currentPlayer:AddComponent(CBoundary.new(rectBoundary))

  -- Optional: set transform if exposed
  local tr = currentPlayer:Transform()
    tr:SetLocalPositionX(startPos.x)
    tr:SetLocalPositionY(startPos.y)

  ----------------------------------------------------------------
  -- Actions (movement + shooting)
  ----------------------------------------------------------------
  local function with_mv(go, dx, dy)
    local m = go:GetComponent(CMovement2D.new())
    if m then m:AddSingleFrameMovementInput(vec2.new(dx, dy)) end
  end

  local moveLeft  = function(go) with_mv(go, -1,  0) end
  local moveRight = function(go) with_mv(go,  1,  0) end
  local moveUp    = function(go) with_mv(go,  0, -1) end
  local moveDown  = function(go) with_mv(go,  0,  1) end

  -- Events
  local moveLeftEvent = Event_GameObjectRef.new()
  local moveRightEvent= Event_GameObjectRef.new()
  local moveUpEvent   = Event_GameObjectRef.new()
  local moveDownEvent = Event_GameObjectRef.new()

  moveLeftEvent:Subscribe(moveLeft)
  moveRightEvent:Subscribe(moveRight)
  moveUpEvent:Subscribe(moveUp)
  moveDownEvent:Subscribe(moveDown)

  -- Input binding helpers (use bound KeyboardKeyData / MouseEventData)

    local function bindKey(state, key, ev)
      local d = PlayerKeyboardKeyData.new()
      d.ButtonState = state
      d.Key         = key
      d.OnTriggered = MakeCommand(function(go) ev:Invoke(go) end)
      playerController:BindKey(d)
    end

  -- Bind inputs using ButtonState enum
  BindPlayerShoot(currentPlayer)

  bindKey(ButtonState.BUTTON_PRESSED, VK_LEFT,  moveLeftEvent)
  bindKey(ButtonState.BUTTON_PRESSED, VK_RIGHT, moveRightEvent)
  bindKey(ButtonState.BUTTON_PRESSED, VK_UP,    moveUpEvent)
  bindKey(ButtonState.BUTTON_PRESSED, VK_DOWN,  moveDownEvent)

  -- WASD (letters -> ASCII codes; KeyboardKey is int)
  bindKey(ButtonState.BUTTON_PRESSED, string.byte('A'), moveLeftEvent)
  bindKey(ButtonState.BUTTON_PRESSED, string.byte('D'), moveRightEvent)
  bindKey(ButtonState.BUTTON_PRESSED, string.byte('W'), moveUpEvent)
  bindKey(ButtonState.BUTTON_PRESSED, string.byte('S'), moveDownEvent)

  -- Subscribe to die => change scene
  if sceneNameOnPlayerDie and sceneNameOnPlayerDie ~= "" then
  self:SubscribeOnDie(function()
    ChangeCurrentScene(sceneNameOnPlayerDie)   -- simpler & bound directly
  end)
  end

  return self
end

function Player:GetGameObject()
  -- In Lua, GameObjectHandle and GameObject behave the same for method calls.
  return self.m_CurrentPlayer
end

function Player:GetGameObjectHandle()
  return self.m_CurrentPlayer
end

function Player:SubscribeOnDie(func)
  local go = self.m_CurrentPlayer
  if not go then return end
  local sc = go:GetComponent(CGameStatController.new())
  if not sc then return end
  sc:SubscribeOnCurrentStatZeroOrLess(StatType.Health, func)
end


-- ---- GAME CREATION ----

function CreateGame()
    -- ---- SCENE NAMES ----
    local mainMenu_SceneName       = "MainMenu"
    local nameAssigner_SceneName   = "NameAssigner"
    local roundManager_SceneName   = "GameRound"
    local highscore_SceneName      = "Highscore"

    -- ---- ENEMIES ----

     -- ---- EVIL ----
    local evil_enemyType = EnemyType.new()

    evil_enemyType.ImageFile = "EvilStar.png"

    evil_enemyType.FollowData.Speed = 2000
    evil_enemyType.FollowData.TimeToChangeDirection = 2.0
    evil_enemyType.FollowData.OptimalDistanceFromTarget = 10.0

    evil_enemyType.GunType.BulletImageFile = "EvilFlyingStar.png"
    evil_enemyType.GunType.BulletAmount = 10
    evil_enemyType.GunType.BulletLifeTime = 1.5
    evil_enemyType.GunType.BulletSpeed = 300
    evil_enemyType.GunType.TimeBetweenShots = 0.3
    evil_enemyType.GunType.BulletPushForce = 300.0
    evil_enemyType.GunType.BulletDamage = 2

    evil_enemyType.DistanceFromTargetToShoot = 200
    evil_enemyType.BodyDamage = 1
    evil_enemyType.BodyPushForce = 600
    evil_enemyType.MaxHealth = 3
    evil_enemyType.Points = 3

     -- ---- SAD ----
    local sad_enemyType = EnemyType.new()

    sad_enemyType.ImageFile = "SadStar.png"

    sad_enemyType.FollowData.Speed = 1500
    sad_enemyType.FollowData.TimeToChangeDirection = 10.0
    sad_enemyType.FollowData.OptimalDistanceFromTarget = 10.0

    sad_enemyType.GunType.BulletImageFile = "SadFlyingStar.png"
    sad_enemyType.GunType.BulletAmount = 10
    sad_enemyType.GunType.BulletLifeTime = 2
    sad_enemyType.GunType.BulletSpeed = 100
    sad_enemyType.GunType.TimeBetweenShots = 0.3
    sad_enemyType.GunType.BulletPushForce = 300.0
    sad_enemyType.GunType.BulletDamage = 3

    sad_enemyType.DistanceFromTargetToShoot = 100
    sad_enemyType.BodyDamage = 2
    sad_enemyType.BodyPushForce = 600
    sad_enemyType.MaxHealth = 2
    sad_enemyType.Points = 2

     -- ---- SLEEPY ----
    local sleepy_enemyType = EnemyType.new()

    sleepy_enemyType.ImageFile = "SleepyStar.png"

    sleepy_enemyType.FollowData.Speed = 1000
    sleepy_enemyType.FollowData.TimeToChangeDirection = 2.0
    sleepy_enemyType.FollowData.OptimalDistanceFromTarget = 10.0

    sleepy_enemyType.GunType.BulletImageFile = "SleepyFlyingStar.png"
    sleepy_enemyType.GunType.BulletAmount = 10
    sleepy_enemyType.GunType.BulletLifeTime = 5
    sleepy_enemyType.GunType.BulletSpeed = 50
    sleepy_enemyType.GunType.TimeBetweenShots = 2
    sleepy_enemyType.GunType.BulletPushForce = 300.0
    sleepy_enemyType.GunType.BulletDamage = 4

    sleepy_enemyType.DistanceFromTargetToShoot = 300
    sleepy_enemyType.BodyDamage = 0
    sleepy_enemyType.BodyPushForce = 600
    sleepy_enemyType.MaxHealth = 6
    sleepy_enemyType.Points = 6

     -- ---- SUN ----
    local sun_enemyType = EnemyType.new()

    sun_enemyType.ImageFile = "AngrySun.png"

    sun_enemyType.FollowData.Speed = 1000
    sun_enemyType.FollowData.TimeToChangeDirection = 2.0
    sun_enemyType.FollowData.OptimalDistanceFromTarget = 10.0

    sun_enemyType.GunType.BulletImageFile = "SunFlyingStar.png"
    sun_enemyType.GunType.BulletAmount = 30
    sun_enemyType.GunType.BulletLifeTime = 5
    sun_enemyType.GunType.BulletSpeed = 50
    sun_enemyType.GunType.TimeBetweenShots = 0.3
    sun_enemyType.GunType.BulletPushForce = 300.0
    sun_enemyType.GunType.BulletDamage = 2

    sun_enemyType.DistanceFromTargetToShoot = 500
    sun_enemyType.BodyDamage = 4
    sun_enemyType.BodyPushForce = 600
    sun_enemyType.MaxHealth = 10
    sun_enemyType.Points = 10

     -- ---- MOON ----
    local moon_enemyType = EnemyType.new()

    moon_enemyType.ImageFile = "DeathMoon.png"

    moon_enemyType.FollowData.Speed = 2000
    moon_enemyType.FollowData.TimeToChangeDirection = 2.0
    moon_enemyType.FollowData.OptimalDistanceFromTarget = 10.0

    moon_enemyType.GunType.BulletImageFile = "MoonFlyingStar.png"
    moon_enemyType.GunType.BulletAmount = 10
    moon_enemyType.GunType.BulletLifeTime = 1.5
    moon_enemyType.GunType.BulletSpeed = 100
    moon_enemyType.GunType.TimeBetweenShots = 0.3
    moon_enemyType.GunType.BulletPushForce = 300.0
    moon_enemyType.GunType.BulletDamage = 2

    moon_enemyType.DistanceFromTargetToShoot = 400
    moon_enemyType.BodyDamage = 0
    moon_enemyType.BodyPushForce = 0
    moon_enemyType.MaxHealth = 8
    moon_enemyType.Points = 8

    -- ---- PLAYER ----
    local playerType = PlayerType.new()

    playerType.MaxHealth = 10

    playerType.GunType.BulletImageFile = "FlyingStar.png"
    playerType.GunType.BulletAmount = 10
    playerType.GunType.BulletLifeTime = 1.5
    playerType.GunType.BulletSpeed = 500
    playerType.GunType.TimeBetweenShots = 0.3
    playerType.GunType.BulletPushForce = 300.0
    playerType.GunType.BulletDamage = 2

    local player = Player.new(highscore_SceneName, playerType)

    -- ---- ROUND PARAMS ----
    local roundParams = RoundParams.new()

    roundParams:AddEnemyType(evil_enemyType)
    roundParams:AddEnemyType(sad_enemyType)
    roundParams:AddEnemyType(sleepy_enemyType)
    roundParams:AddEnemyType(sun_enemyType)
    roundParams:AddEnemyType(moon_enemyType)

    roundParams.InitialMaxEnemyNumber = 10
    roundParams.SecondsBetweenSpawning = 10
    roundParams.SecondsToIncreaseMaxEnemyNumByOne = 15

    -- ---- GAME LOOP ----
    local mainMenuData = MainMenuData.new()
    mainMenuData.SceneName = mainMenu_SceneName
    mainMenuData.NextSceneName = nameAssigner_SceneName
    local mainMenu = MainMenu.new(mainMenuData)

    local nameAssignerSceneData = NameAssignerSceneData.new()
    nameAssignerSceneData.Player = player:GetGameObjectHandle()
    nameAssignerSceneData.SceneName = nameAssigner_SceneName
    nameAssignerSceneData.SceneNameUponCompletion = roundManager_SceneName
    local nameAssignerScene = NameAssignerScene.new(nameAssignerSceneData)

    local roundManager = RoundManager.new(roundParams, roundManager_SceneName, player:GetGameObjectHandle())

    local highscoreScene = HighscoreScene.new(highscore_SceneName, mainMenuData.SceneName, player:GetGameObjectHandle())
end
