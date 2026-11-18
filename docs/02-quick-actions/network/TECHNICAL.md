# Network — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Scope and native fields

| Focus control | Native property | Scope |
| --- | --- | --- |
| Net Load on Client | `bNetLoadOnClient` | Actor |
| Replicate Physics to Autonomous Proxy | `bReplicatePhysicsToAutonomousProxy` | Primitive Component |
| Static Mesh Replicate Movement | `bStaticMeshReplicateMovement` | Actor |

Focus does not create NetDrivers, configure role/ownership automatically, or implement replication. Eligibility depends on an actual editable native property on the given Actor or Component.

## Networking boundary

`GetTargets` selects either the Actor or registered eligible Primitive Components according to each definition. A native property being enabled does not configure net roles, ownership, drivers or the correct physics replication setup. Test the observable effect in multi-client PIE; reading a boolean alone cannot verify replication.

## Source references

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — property definition, eligibility and write implementation
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — state, Slate widget, transaction and Editor integrations

For the user workflow, [read Design](DESIGN.md).

**Verification:** Code reviewed; live Unreal Editor behavior must be confirmed using the [Verification Matrix](../../VERIFICATION_MATRIX.md).
