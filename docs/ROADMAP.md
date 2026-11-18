# Focus Roadmap

**Current release:** Focus v2.1.0 · UE 5.8.x · Win64 · Editor-only  
**Historical milestones:** v1.0 and v2.0 (provided by the plugin author; milestone summaries rather than independently reconstructed changelogs)  
**Next major direction:** **v3.0 — Planned** (scope is directional; no date, completed API, or release commitment)  
**Reviewed:** November 18, 2026

## At a glance

<!-- Media target: IMAGE | roadmap-milestones | docs/assets/media/roadmap-milestones.png -->
<figure class="doc-media" data-kind="image" data-media-id="roadmap-milestones">
  <img src="assets/media-placeholder.png" alt="Timeline showing Focus v1.0, v2.0 and planned v3.0 capabilities" loading="lazy">
  <figcaption>Focus evolved from Visual and Interaction controls in v1.0 to broader native Quick Actions in v2.0, with extensibility planned for v3.0.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Create a clean horizontal timeline with v1.0 Released (Visual, Interaction), v2.0 Released (eight more Quick Action categories and session-only Focus editing), and v3.0 Planned (new categories, provider API, Widget Builder). Use a visually distinct Planned treatment.</span></figcaption>
</figure>


| Milestone | Status | What changed / is planned |
| --- | --- | --- |
| **v1.0 — The essentials** | Released · historical | The original **Visual** and **Interaction** Quick Actions: visibility, Solo, and protection workflows |
| **v2.0 — Expanded Quick Actions** | Released · historical | The other eight built-in categories and support for **session-only (non-persistent) Focus Visual/Interaction changes** |
| **v2.1.0 — Current documented release** | Released · current | Current ten-category implementation documented throughout this site; this entry is a documentation baseline, not a claim of additional specific v2.1.0 features |
| **v3.0 — Extensible Focus** | **Planned** | Additional Quick Action categories, an extension API for other plugins, and a user-facing Quick Widget Builder |

> **Important distinction:** “Non-persistence” in the v2.0 milestone refers to Focus-initiated **Visual/Interaction** changes that can be session-only. The eight native-property categories still edit Unreal properties with normal save behavior.

## History and evolution

### v1.0 — Visual and Interaction

Focus began as a compact Outliner workflow tool. The initial set consisted of **Visual** controls (Hide in Editor, Hidden in Game and Solo) and **Interaction** controls (Selection Lock and Transform/Edit protection). The goal was faster scene organization and fewer accidental edits without requiring a separate Actor or editor window.

### v2.0 — More categories and optional non-persistence

v2.0 expanded Focus beyond visibility and locking by adding **Simulation, Physics Events, Lighting, World, Network, Navigation, Mobility and LOD** Quick Actions. These are shortcuts for *native Unreal Actor/Component properties*, not new Focus gameplay systems.

v2.0 also introduced the ability to choose session-only behavior for Focus's Visual and Interaction operations. This lets users temporarily hide, Solo or protect Actors without intending those actions to replace the saved Actor state. The precise current implementation and its save/checkout limitations are documented in [Settings and Saving](03-settings-and-saving/README.md).

### v3.0 — Planned extensibility

The next major direction is to evolve Focus from a fixed list of built-in controls into a **Quick Actions platform**. The three planned pillars are:

1. **More built-in Quick Action categories.** Research and deliver useful new compact native workflows.
2. **Plugin extension API.** Other Unreal Editor plugins register Focus-compatible categories and micro-widgets without modifying Focus or the engine.
3. **Quick Widget Builder.** Users design and configure Focus-compatible Quick Actions themselves in the editor, using the same underlying extension system.

**None of these v3.0 features exists in the current v2.1.0 release.** The order of engineering work and exact v3.0 feature scope may change as the design is validated; v3.0 is a proposed major-version target, not a delivery date.

## Product direction

Focus should become a practical, native-feeling **World Outliner Quick Actions platform**: useful built-in categories, a supported way for other plugins to contribute their own categories and micro-widgets, and an accessible editor tool for users to create custom Quick Actions without writing C++.

The user experience should remain compact. Advanced extensibility should not make the default Outliner wider, slower or harder to understand.

## v3.0 planned capabilities

| Direction | Status | Goal | Dependencies |
| --- | --- | --- | --- |
| More built-in Quick Actions categories | Planned · exploratory | Expand the useful native Unreal operations that Focus exposes | Clear category selection, engine-native behavior and tests |
| Extension API for external plugins | Planned · architecture/design | Let third-party Unreal plugins register Focus-compatible categories and micro-widgets | Extensible registry, stable identifiers, native Slate lifecycle and ownership contracts |
| Quick Widget Builder | Planned · product/design | Let users create their own Focus-compatible categories and micro-widgets in the editor | The same registration/evaluation framework used by built-ins and plugin extensions |

No specific new category names, delivery date or editor UI are committed. **v3.0 is the planned major-version target**, not an already shipped release.

## v3.0 / 1. Additional built-in Quick Actions

**Goal:** Add categories that remove high-frequency Outliner/Details friction while retaining Focus's compact feel.

### Selection criteria

A new category should answer these questions before it is approved:

- Does it address a frequent, meaningful editor task rather than expose a rarely used property for its own sake?
- Is it safe and predictable on one Actor, mixed selections, folders, attachments and groups?
- Can its icon or micro-widget communicate On, Off, Mixed, Not Applicable and destructive operations without confusion?
- Does the category map to proper Unreal properties or commands with Undo/Redo and native edit restrictions?
- Does it have reasonable eligibility rules and explanatory tooltips?
- What is the performance cost in a large loaded world and in levels with many Outliner rows?
- Is the behavior an editor operation or a runtime configuration that needs extra explanation?

### Proposed delivery approach

New categories can be added in small, coherent groups of related actions. Do not predefine which categories ship before research and their core workflows are agreed. Each shipped category should receive its own **Design** and **Technical** documentation, examples and automation checks in the same PR.

**Acceptance:** A new category can be hidden, shown, operated, reverted, persisted appropriately and used safely in mixed/loaded hierarchy scopes. Unsupported target types are marked unavailable rather than silently altered.

## v3.0 / 2. Extension API for third-party plugins

**Goal:** Allow other Unreal Editor plugins to supply **their own Focus-compatible Quick Actions categories and micro-widgets** without modifying the Focus source or replacing the Outliner column.

### Current technical constraint

The existing source is not yet a plugin extension system:

- `FocusQuick::ECategory` is a fixed enum of ten categories; `CategoryName`, `CategoryVisible` and spacing/width logic depend on it.
- `FocusQuick::Definitions()` returns a hard-coded array of built-in property definitions.
- `FFocusColumn::CompactCategory` selects specialized Slate widget implementations with fixed category branches and authored dimensions.
- `FFocusColumn::ConstructRowWidget` knows how to render the built-in Visual and Interaction controls and then iterates the fixed native property categories.

Therefore, public registration must be designed **below the existing UI**, rather than exposing the static definition array as an API and assuming third-party categories will automatically work.

### Proposed API architecture (subject to design review)

<!-- Media target: IMAGE | roadmap-extension-api | docs/assets/media/roadmap-extension-api.png -->
<figure class="doc-media" data-kind="image" data-media-id="roadmap-extension-api">
  <img src="assets/media-placeholder.png" alt="Conceptual architecture of Focus categories, third-party providers and the Quick Widget Builder" loading="lazy">
  <figcaption>The planned extension registry would provide a shared foundation for built-in actions, plugin contributions and user-authored widgets.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Create a conceptual, clearly labeled Planned architecture diagram: Focus Quick Actions registry in the center; built-in providers and third-party plugin providers register categories/micro-widgets; the Quick Widget Builder supplies data-driven definitions; all feed the same Outliner rendering/eligibility/transaction system.</span></figcaption>
</figure>


| Contract | Purpose |
| --- | --- |
| **Category descriptor** | Stable namespaced ID, display name, description, default visibility, order/placement and icon/layout preferences |
| **Action/widget descriptor** | Stable namespaced ID, labels/tooltips, interaction type and category membership |
| **Target resolver** | Determines compatible Actor/Component targets and reports eligible counts and unavailable reasons |
| **State evaluator** | Produces On, Off, Mixed, Not Applicable or an appropriate typed value from current targets |
| **Execution adapter** | Applies an authorized change to the resolved eligible loaded scope and supports native transaction/undo behavior |
| **Slate widget factory / schema** | Supplies a Focus-sized custom widget, or asks Focus to render a supported standardized control |
| **Registration lifetime** | Registers/unregisters per module lifecycle; handles unloaded modules, stale Slate widgets and dependency order |
| **Persistence policy** | Explicitly declares whether the value is native/saved, temporary editor state or a custom plugin-owned asset/config value |

**Preferred division:** Focus owns Outliner integration, target scope, state display, widget placement, settings, tooltip conventions and transactional boundaries. Provider plugins own domain-specific eligibility, values and actions. Do not force every provider into the existing three `EKind` variants.

### Required guardrails

- **No Unreal Engine source modifications.** Extension providers are ordinary editor modules.
- Stable identifiers and deterministic ordering prevent collisions and reordering across sessions.
- Failure/unload of one provider must not invalidate built-in categories or other providers.
- Provider widgets must obey Focus visibility, spacing, native editing/selection constraints and color/accessibility conventions.
- Expensive callbacks should not scan the entire editor world for every Slate paint or hover event; cached/batched evaluation and invalidation are needed.
- Multi-select and component eligibility must be defined in the API instead of inferred from a clicked row.
- Never promise universal Edit Lock protection for custom tools. Decide how an extension opts into Focus's native guard and cancellation semantics.
- Public Blueprint access and binary ABI stability are **open design decisions**, not assumed.
- Prefer a narrow, documented API with examples and independent native automation tests over exposing internal mutable state.

### API acceptance criteria

An example **separate Editor plugin** registers a new category with at least one custom micro-widget, updates values on single Actors and mixed folder selections, survives changing visibility, supports Undo/Redo, and unregisters cleanly while Focus continues working. The example must work without editing Focus or Unreal Engine source. Existing ten categories must retain their appearance and behavior.

## v3.0 / 3. Quick Widget Builder

**Goal:** Let users create and manage Focus-compatible micro-widgets and optionally user-defined categories **inside Unreal Editor**, without writing an extension plugin.

### Foundation

The builder should create a **data-driven definition**, not a generated C++ file or a stand-alone alternative Outliner implementation. Its output should use the **same registry, target resolver, state evaluator and execution adapter** established for the API. Built-ins, third-party plugins and user-authored widgets should share a consistent visual language.

### Proposed authoring workflow (not yet specified UI)

1. Create a **Quick Widget** definition through a discoverable editor action or Content Browser asset workflow.
2. Select an existing category or define a custom category with a clear label and icon.
3. Choose the action type and compatible Actor/Component targets.
4. Choose an approved property/value binding or action provider; restrict the authoring surface to safe, supported operations.
5. Configure appearance, default state, tooltip, availability rules and mixed-state behavior.
6. Preview the compact widget against representative Actors before publishing it to the Outliner.
7. Enable it in Focus's category menu and edit/revert it using the normal Focus interactions.

The concrete asset type, exact builder UI, how visual layouts are authored, and whether any Blueprint callback extensions are allowed are **design questions**, not existing features.

### Initial scope recommendation

Start with **safe property-backed widgets**: booleans, numeric fields with constraints, enums, and compact multi-option choices. Support custom callbacks only through trusted extension providers with explicit permissions/validation. Avoid arbitrary user-entered code execution, unbounded world scans and unsafe bulk operations in the initial builder.

### Acceptance criteria

A designer creates a widget without recompiling, previews it, places it in a category, uses it on eligible single and mixed selections, receives helpful Not Applicable and validation messaging, and can edit/delete the widget definition cleanly. A broken or missing binding produces a visible diagnostic without affecting unrelated categories. Settings remain predictable after reopening the editor.

## Recommended sequence

**Foundation first, then expand:**

1. **Define an extensible Quick Actions contract and native registry**, covering the existing ten categories and providers; preserve current UI/behavior.
2. **Deliver the external plugin API** using an independently compiled example provider and full editor lifecycle, performance, Undo and hierarchy regression tests.
3. **Deliver the data-driven Widget Builder** on that same API, beginning with safe property-backed controls and preview/validation.
4. Add **further built-in categories** as researched and approved. Independent new built-ins can be developed alongside the API work, but should use the new registry once available.

This order reduces the chance of implementing a convenient builder UI and then discovering it requires a second incompatible widget runtime.

## Cross-cutting quality requirements

Every shipped capability should have: a Design user guide; a Technical integration reference; screenshots or short demonstrations; explicit supported targets/unsupported cases; Native Unreal Automation tests independent of any other plugin; versioned release documentation; and diagnostics for malformed provider or user-authored definitions.

## Open decisions

These should be answered during API discovery rather than assumed:

- Can third-party providers register only **new** categories, or also contribute widgets to existing categories?
- Should user-created categories be ordered manually, sorted, or grouped after built-ins?
- Should registry metadata be public C++ only initially, with data-driven editor definitions layered on top?
- Which built-in actions, if any, may carry session-only persistence? How does a provider declare a custom persistence strategy?
- Will the builder support only native property bindings at first, or also whitelisted Blueprint-callable/editor actions?
- How should large custom widget layouts, conflicting stable IDs and missing plugins be represented in the narrow Outliner?
- What user-facing asset/data location is appropriate for sharing definitions in Perforce while keeping personal visibility preferences per editor user?

## Not planned or promised

No specific new category list, replication system, runtime Focus framework, arbitrary scripting sandbox or delivery dates are committed. **The extension API and Widget Builder remain planned for v3.0**, not available in v2.1.0.

---

[Current Quick Actions](02-quick-actions/README.md) · [Technical source map](04-workflows-and-implementation/source-map/TECHNICAL.md) · [Documentation audit](DOCUMENTATION_AUDIT.md)
