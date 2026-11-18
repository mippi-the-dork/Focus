# Network — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Scope and native fields

| Focus control | Native property | Scope |
| --- | --- | --- |
| Net Load on Client | `bNetLoadOnClient` | Actor |
| Replicate Physics to Autonomous Proxy | `bReplicatePhysicsToAutonomousProxy` | Primitive Component |
| Static Mesh Replicate Movement | `bStaticMeshReplicateMovement` | Actor |

Focus does not create NetDrivers, configure role/ownership automatically, or implement replication. Eligibility depends on an actual editable native property on the given Actor or Component.

## Exact network property targets

| Label | Scope | Native property |
| --- | --- | --- |
| Net Load on Client | Actor | `bNetLoadOnClient` |
| Replicate Physics to Autonomous Proxy | Primitive | `bReplicatePhysicsToAutonomousProxy` |
| Static Mesh Replicate Movement | Actor | `bStaticMeshReplicateMovement` |

The last property is exposed through the Actor-level scope and is subject to that Actor's property availability; other classes do not gain a new replication system from Focus.

## Shared implementation contracts

For the eight native-property categories, `FocusQuick::GetTargets` finds valid, registered and instance-editable Actor/Component targets. `FocusQuick::FindProperty` respects native `CanEditChange`, EditConst and EditCondition restrictions. Writes are wrapped in Unreal transactions and dispatch native setters or property-change notifications; Edit Lock skips Actor targets. Visual and Interaction use `Focus::Get/Set`, including distinct persistence options.

Refer to [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) and [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp).

For steps and expected behavior, [read Design](DESIGN.md).
