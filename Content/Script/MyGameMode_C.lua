--
-- DESCRIPTION
--
-- @COMPANY **
-- @AUTHOR **
-- @DATE ${date} ${time}
--

require "UnLua"

local MyGameMode_C = Class()

--function MyGameMode_C:Initialize(Initializer)
--end

--function MyGameMode_C:UserConstructionScript()
--end

function MyGameMode_C:ReceiveBeginPlay()
    print("This is LuaGameMode");
end

--function MyGameMode_C:ReceiveEndPlay()
--end

-- function MyGameMode_C:ReceiveTick(DeltaSeconds)
-- end

--function MyGameMode_C:ReceiveAnyDamage(Damage, DamageType, InstigatedBy, DamageCauser)
--end

--function MyGameMode_C:ReceiveActorBeginOverlap(OtherActor)
--end

--function MyGameMode_C:ReceiveActorEndOverlap(OtherActor)
--end

return MyGameMode_C
