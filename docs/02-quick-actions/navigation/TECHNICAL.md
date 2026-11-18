# Navigation — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Native fields and scopes

| Focus control | Property | Scope |
| --- | --- | --- |
| Can Ever Affect Navigation | `bCanEverAffectNavigation` | Actor Component |
| Dynamic Obstacle | `bDynamicObstacle` | Shape Component |
| Fill Collision Underneath for Navmesh | `bFillCollisionUnderneathForNavmesh` | Primitive Component |
| Rasterize as Filled Convex Volume | `bRasterizeAsFilledConvexVolume` | Primitive Component |

The navigation contribution path uses `SetCanEverAffectNavigation` where applicable; other properties use validated reflected writes. Focus does not implement navmesh generation or pathfinding.

## Exact navigation property targets

| Label | Scope | Native property |
| --- | --- | --- |
| Can Ever Affect Navigation | Actor Component | `bCanEverAffectNavigation` |
| Dynamic Obstacle | Shape Component | `bDynamicObstacle` |
| Fill Collision Underneath for Navmesh | Primitive | `bFillCollisionUnderneathForNavmesh` |
| Rasterize as Filled Convex Volume | Primitive | `bRasterizeAsFilledConvexVolume` |

`FocusQuick::Write` invokes `UActorComponent::SetCanEverAffectNavigation` for that property, rather than only setting the reflected field. Remaining applicable flags go through validated property writes and normal edit-change notifications.

## Shared implementation contracts

For the eight native-property categories, `FocusQuick::GetTargets` finds valid, registered and instance-editable Actor/Component targets. `FocusQuick::FindProperty` respects native `CanEditChange`, EditConst and EditCondition restrictions. Writes are wrapped in Unreal transactions and dispatch native setters or property-change notifications; Edit Lock skips Actor targets. Visual and Interaction use `Focus::Get/Set`, including distinct persistence options.

Refer to [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) and [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp).

For steps and expected behavior, [read Design](DESIGN.md).
