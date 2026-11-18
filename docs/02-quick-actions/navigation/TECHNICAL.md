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

## Navigation boundary

Targets are scoped to Actor Components, Shape Components or Primitive Components as defined. `SetCanEverAffectNavigation` is invoked for the supported contribution path; other properties use native reflected writes. Rebuild/inspect the navmesh after a change; Focus itself does not calculate navigation paths.

## Source references

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — property definition, eligibility and write implementation
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — state, Slate widget, transaction and Editor integrations

For the user workflow, [read Design](DESIGN.md).

**Verification:** Code reviewed; live Unreal Editor behavior must be confirmed using the [Verification Matrix](../../VERIFICATION_MATRIX.md).
