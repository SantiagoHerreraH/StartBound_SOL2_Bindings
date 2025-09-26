---@diagnostic disable: undefined-global
--------------------------------------------------------------------------------
-- EmmyLua annotations for better IntelliSense/hover/type-checks
--------------------------------------------------------------------------------

---@class vec2
---@field x number
---@field y number

---@class vec3
---@field x number
---@field y number
---@field z number

---@class Rect
---@field Top integer
---@field Left integer
---@field Width integer
---@field Height integer
---@field Right fun(self:Rect):integer
---@field Bottom fun(self:Rect):integer

---@class GameObject
---@field Transform fun(self:GameObject): Transform
---@field TransformConst fun(self:GameObject): Transform
---@field AddComponent fun(self:GameObject, component:any): any
---@field GetComponent fun(self:GameObject, component:any): any

---@alias GameObjectHandle userdata  -- std::shared_ptr<GameObject> in C++

---@class Transform
---@field SetLocalPosition fun(self:Transform, pos:vec3)
---@field SetLocalPositionX fun(self:Transform, x:number)
---@field SetLocalPositionY fun(self:Transform, y:number)
---@field GetWorldTransform fun(self:Transform): TransformData

---@class TransformData
---@field Position vec3
---@field Rotation vec3
---@field Scale    vec3

---@class HealthBarData
---@field Color integer
---@field Rect Rect

---@class GunType
---@field BulletImageFile string
---@field BulletAmount integer
---@field BulletLifeTime number
---@field BulletSpeed number
---@field TimeBetweenShots number
---@field BulletPushForce number
---@field BulletDamage integer

---@class GunData
---@field GunType GunType
---@field BulletCollisionLayer CollisionLayers
---@field BulletCollisionLayerToCollideAgainst CollisionLayers
---@field Shooter GameObjectHandle

---@class Event_GameObjectRef
---@field Subscribe fun(self:Event_GameObjectRef, cb:fun(go:GameObject)):integer
---@field Unsubscribe fun(self:Event_GameObjectRef, key:integer):boolean
---@field Invoke fun(self:Event_GameObjectRef, go:GameObject)

---@class GameStat
---@field CurrentStat integer
---@field MaxStat integer
---@field MaxLimitStat integer

---@class CGameStatController
---@field CreateStat fun(self:CGameStatController, key:StatType, stat:GameStat):boolean
---@field SubscribeOnCurrentStatZeroOrLess fun(self:CGameStatController, key:StatType, cb:fun())

---@class CAnimation
---@field SetFramesPerSecond fun(self:CAnimation, fps:integer)
---@field CanLoop fun(self:CAnimation, can:boolean)
---@field AddFrame fun(self:CAnimation, frame:CTextureHandle, frames:integer)

---@class CTextureHandle
---@class SpriteSheet
---@field GetScaledCellWidth fun(self:SpriteSheet):number
---@field GetScaledCellHeight fun(self:SpriteSheet):number
---@field GetFrame fun(self:SpriteSheet, row:integer, col:integer):CTextureHandle

---@class CHealthBar
---@class CCollider
---@field CenterRect fun(self:CCollider)
---@field RenderCollider fun(self:CCollider, render:boolean, fill:boolean, color:integer)

---@class CRigidbody2D
---@field SetMaxSpeed fun(self:CRigidbody2D, speed:number)

---@class CMovement2D
---@field SetMaxSpeed fun(self:CMovement2D, speed:number)
---@field SetMaxAccelForce fun(self:CMovement2D, f:number)
---@field AddSingleFrameMovementInput fun(self:CMovement2D, dir:vec2)

---@class CGun
---@field Shoot fun(self:CGun, dir:vec2)

---@class CPlayerController
---@field BindKey fun(self:CPlayerController, data:PlayerKeyboardKeyData)
---@field BindMouseEvent fun(self:CPlayerController, data:PlayerMouseEventData)

---@class PlayerKeyboardKeyData
---@field ButtonState ButtonState
---@field Key integer
---@field OnTriggered any  -- std::shared_ptr<ICommand>

---@class PlayerMouseEventData
---@field ButtonState ButtonState
---@field IsLeft boolean
---@field OnTriggered any  -- std::shared_ptr<ICommand>

---@class CBoundary
---@class RoundParams
---@field AddEnemyType fun(self:RoundParams, t:EnemyType)

---@class EnemyType
---@field ImageFile string
---@field FollowData { Speed:number, TimeToChangeDirection:number, OptimalDistanceFromTarget:number }
---@field GunType GunType
---@field DistanceFromTargetToShoot number
---@field BodyDamage integer
---@field BodyPushForce number
---@field MaxHealth integer
---@field Points integer

---@class MainMenuData
---@field SceneName string
---@field NextSceneName string

---@class NameAssignerSceneData
---@field Player GameObjectHandle
---@field SceneName string
---@field SceneNameUponCompletion string

---@enum ButtonState
-- BUTTON_DOWN | BUTTON_PRESSED | BUTTON_UP

---@enum CollisionLayers
-- Player | Enemy | EnemyBullet | PlayerBullet

---@enum StatType
-- Health | Points | ShotsFired | NumberOfHits

-- Declare globals *as types* without executing code:
---@class _G
---@field g_WindowWidth number
---@field g_WindowHeight number