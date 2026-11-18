# Interaction — Technical

**Focus v2.1.0 · Quick Actions · Engine reference**

## Protection cycle behavior

`FFocusColumn::ProtectionState` represents unlocked, transform-locked, edit-locked, or mixed protection levels. Clicking a mixed scope normalizes it to Transform Lock. The combined protection level never silently toggles Selection Lock.

## Selection Lock routing

`UFocusEditorMode::IsSelectionDisallowed` handles supported selection attempts. It explicitly permits selection initiated with a Scene Outliner under the pointer. Cut/delete processing also checks protected selections.


## Transform Lock routing

`FFocusTransformGuard` records transaction-scoped snapshots around standard editor changes and restores protected world transforms. It does not continuously rewrite Actors every frame. The transform guard deliberately allows applicable Blueprint-driven changes rather than treating authored runtime logic as an editor drag.


## Edit Lock routing

<!-- Media target: IMAGE | interaction-native-guards | docs/assets/media/interaction-native-guards.png -->
<figure class="doc-media" data-kind="image" data-media-id="interaction-native-guards">
  <img src="../../assets/media-placeholder.png" alt="Diagram of Focus selection, transform, command and Details editing guards" loading="lazy">
  <figcaption>Focus enforces its editing protections through multiple native editor integration paths rather than a single global lock.</figcaption>
</figure>


`FFocusModule::BindView` wraps a Details view's `FIsPropertyReadOnly` and `FIsPropertyEditingEnabled` delegates, preserving the original handlers. `FFocusInteractionGuard` wraps eligible Outliner renames, component add/rename paths and structural drag/drop interactions. `FFocusCommandGuard` handles supported editor commands.


## State model

`Focus::Get(Transform)` evaluates `Flags.Transform || Flags.Edit`. `Focus::Set(Transform)` refuses to change it while Edit Lock is active. `UFocusSettings::bPersistProtection` controls session persistence for both protection levels. The level selection is not a security mechanism for Python, custom editors or arbitrary C++.

---

## Verification boundaries

Selection Lock, Transform Lock and Edit Lock are handled by distinct native-editor paths, not a universal permission check. `FFocusModule::BindView` preserves previous Details delegates, and `FFocusInteractionGuard` wraps selected native Slate interactions. Scripted editing can bypass these safeguards. See [Architecture Technical](../../04-workflows-and-implementation/architecture/TECHNICAL.md).

## Source references

- [FocusQuickProperties.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp) — property definition, eligibility and write implementation
- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp) — state, Slate widget, transaction and Editor integrations

For the user workflow, [read Design](DESIGN.md).

**Verification:** Code reviewed; live Unreal Editor behavior must be confirmed using the [Verification Matrix](../../VERIFICATION_MATRIX.md).
