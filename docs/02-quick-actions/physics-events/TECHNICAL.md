# Physics Events — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Exact native property targets

<!-- Media target: IMAGE | physics-events-path | docs/assets/media/physics-events-path.png -->
<figure class="doc-media" data-kind="image" data-media-id="physics-events-path">
  <img src="../../assets/media-placeholder.png" alt="Diagram of primitive collision and physics event generation paths" loading="lazy">
  <figcaption>Hit, overlap and wake notifications originate from distinct native properties and require corresponding engine conditions.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Make a three-lane diagram for Hit, Overlap, and Wake events. Each lane should show its Focus flag, the relevant collision/physics prerequisite, and the event produced by Unreal when those conditions are met.</span></figcaption>
</figure>


| Focus control | Native property | Target |
| --- | --- | --- |
| Generate Hit Events | `BodyInstance.bNotifyRigidBodyCollision` | Primitive Component |
| Generate Overlap Events | `bGenerateOverlapEvents` | Primitive Component |
| Generate Wake Events | `BodyInstance.bGenerateWakeEvents` | Primitive Component |

`FocusQuick::Write` uses `SetNotifyRigidBodyCollision` and `SetGenerateOverlapEvents` where appropriate. Wake notifications use a validated native BodyInstance property write. For all three, component target eligibility is checked; runtime notifications depend on physics and collision behavior beyond Focus.

## Scope and notification conditions

All three controls require eligible, registered Primitive Components. A valid field write does not guarantee hit/overlap/wake events: the engine's body simulation, collision response and event routing still determine runtime delivery. The controls are persisted as native component properties and use Focus's standard quick-property transaction path.

## Source references

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — property definition, eligibility and write implementation
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — state, Slate widget, transaction and Editor integrations

For the user workflow, [read Design](DESIGN.md).

**Verification:** Code reviewed; live Unreal Editor behavior must be confirmed using the [Verification Matrix](../../VERIFICATION_MATRIX.md).
