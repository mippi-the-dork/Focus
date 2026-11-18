# Focus Roadmap

**Planning status:** Proposed directions, not shipped commitments.  
**Current documented release:** Focus v2.1.0 (Unreal Engine 5.8.x · Win64 · Editor-only).  
**Last reviewed:** November 18, 2026.

Focus currently offers ten built-in Quick Actions categories, including 28 native-property controls across the eight property-focused categories. The three future directions below were supplied by the plugin author. **None of the three is presented as implemented.**

## Product direction

Focus should become a practical, native-feeling **World Outliner Quick Actions platform**: useful built-in categories, a supported way for other plugins to contribute their own categories and micro-widgets, and an accessible editor tool for users to create custom Quick Actions without writing C++.

The user experience should remain compact. Advanced extensibility should not make the default Outliner wider, slower or harder to understand.

## Planned capabilities

| Direction | Status | Goal | Dependencies |
| --- | --- | --- | --- |
| More built-in Quick Actions categories | Planned · exploratory | Expand the useful native Unreal operations that Focus exposes | Clear category selection, engine-native behavior and tests |
| Extension API for external plugins | Planned · architecture/design | Let third-party Unreal plugins register Focus-compatible categories and micro-widgets | Extensible registry, stable identifiers, native Slate lifecycle and ownership contracts |
| Quick Widget Builder | Planned · product/design | Let users create their own Focus-compatible categories and micro-widgets in the editor | The same registration/evaluation framework used by built-ins and plugin extensions |

No additional category names, release versions, release dates or exact editor UI are committed yet.

## 1. Additional built-in Quick Actions

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

## 2. Extension API for third-party plugins

**Goal:** Allow other Unreal Editor plugins to supply **their own Focus-compatible Quick Actions categories and micro-widgets** without modifying the Focus source or replacing the Outliner column.

### Current technical constraint

The existing source is not yet a plugin extension system:

- `FocusQuick::ECategory` is a fixed enum of ten categories; `CategoryName`, `CategoryVisible` and spacing/width logic depend on it.
- `FocusQuick::Definitions()` returns a hard-coded array of built-in property definitions.
- `FFocusColumn::CompactCategory` selects specialized Slate widget implementations with fixed category branches and authored dimensions.
- `FFocusColumn::ConstructRowWidget` knows how to render the built-in Visual and Interaction controls and then iterates the fixed native property categories.

Therefore, public registration must be designed **below the existing UI**, rather than exposing the static definition array as an API and assuming third-party categories will automatically work.

### Proposed API architecture (subject to design review)

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

## 3. Quick Widget Builder

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

No specific new category list, no replication implementation, no runtime Focus framework, no arbitrary scripting sandbox, no public API available in v2.1.0, and no delivery dates or release numbers have been committed in this roadmap.

---

[Current Quick Actions](02-quick-actions/README.md) · [Technical source map](04-workflows-and-implementation/source-map/TECHNICAL.md) · [Documentation audit](DOCUMENTATION_AUDIT.md)
