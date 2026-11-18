# Focus Documentation Audit and Improvement Plan

**Review baseline:** Focus v2.1.0 on the current Focus repository, November 18, 2026.  
**Basis:** Current `docs/` content, the plugin README, and inspected C++ source including `FocusModule.cpp`, `FocusQuickProperties.cpp/.h`, `FocusSettings.h` and `FocusInteractionGuard.h`.  
**Status:** Editorial/source audit; **not** a substitute for running a complete Unreal Editor or browser-based QA pass.

## Executive assessment

The documentation's **organization is good**: five main sections, ten Quick Actions categories matching the plugin's actual categories, and Design/Technical companions. The source-backed native property list, state storage and saving behavior are also materially stronger than the original README.

However, organizing content by category is only the first step. Several pages still have repeated subsections from earlier consolidations, practical control interaction is inconsistently explained, and neither the site nor the documentation currently offers enough visual demonstrations and verified edge-case examples to answer almost any user question.

The most valuable next improvement is **control-level completeness**: exact actions, input methods, targeting, states, persistence, limitations, tests and screenshots for every micro-widget.

## Changes completed in this documentation pass

- Rewrote **all ten Quick Actions Design pages**, removing repeated tables and adding control explanations, precise input handling, eligibility, task-based workflows and failure cases.
- Reused the repository's genuine **Focus-Controls.png**, **Focus-Solo.gif** and **Focus-Locking.gif** within Visual and Interaction. The other eight categories still need authentic editor captures; images were not invented.
- Added **full-text Markdown search**: title results are immediate and content is indexed with bounded concurrency when searching; matching excerpts appear in results. Browser validation is still required.
- Added a [Verification Matrix](VERIFICATION_MATRIX.md) with concrete Unreal Editor and browser test cases. Unexecuted tests remain marked Not run.
- Updated the [Roadmap](ROADMAP.md) to show author-provided **v1.0** and **v2.0** history and **v3.0 planned** capabilities.

## Findings, ranked

| Priority | Finding | Evidence | Recommended correction | Status |
| --- | --- | --- | --- | --- |
| P0 | Exact mouse/keyboard interaction is missing from some category guides | `SFocusLODNumber` supports left-click +1, Alt-click −1, mouse wheel ±1, double-click text entry; earlier LOD Design page primarily described value meanings | Document control interaction precisely, including numeric entry and mixed state | Addressed for LOD in this audit pass |
| P0 | Category pages contain overlapping descriptions | World, Network, Navigation, Mobility and LOD Design guides contain repeated control tables or related explanations assembled from former combined pages | Consolidate to one authoritative controls table per category, retain distinct examples/edge cases | Addressed in rewritten ten category Design guides; verify in browser |
| P0 | Native and Focus-specific state terminology can blur | Visual/Interaction use `Focus::Get/Set` and optional session-only persistence; other eight categories use `FocusQuick::Write` and native save semantics | Maintain separate explanations and avoid generic Quick Actions language that implies one persistence model | Rewritten; real-editor verification remains |
| P1 | Visual documentation is sparse | Main documentation home uses a Focus screenshot; many category guides have no local annotated image showing click targets/states | Capture 10 category-specific annotated screenshots and short gifs for unusual interactions | Partially addressed with existing Solo/Lock GIFs and controls screenshot; remaining category captures needed |
| P1 | Search only indexes titles, summaries and paths | `docs.js::searchPages` uses `page.search` and `page.path`, not Markdown page bodies | Index Markdown on demand with body snippets | Implemented; browser verification outstanding |
| P1 | Edge-case behavior is under-tested/documented | Source includes complex native guards, Actor-package pre/post-save overlays, Blueprint replacements and unloaded World Partition handling | Publish a versioned test/verification matrix with results for targeted engine configurations | Verification matrix documented; live editor tests not run |
| P1 | Need “why is this control unavailable?” at the point of use | Native property filtering in `FocusQuick::FindProperty` and component scope checks are nontrivial | Give category-specific target eligibility and example inapplicable Actors, not only generic eligibility guidance | Planned |
| P1 | Pages lack small task-based outcomes | Several categories explain individual flags but not prerequisite → action → observable result → reversal | Add at least one full workflow and one failure case for each category | Added per-category workflows; editor validation still needed |
| P1 | Roadmap was absent from site | No `docs/ROADMAP.md` or corresponding sidebar destination in the inspected docs | Add roadmap clearly separating planned from shipped functionality | Addressed in this pass |
| P2 | Technical pages link to `main`, not a fixed version | Implementation links are to repository default branch while documentation claims v2.1.0 | Pin source links to release tags or commit SHAs when a maintained release/tag exists | Planned |
| P2 | Some category overviews are still navigation-heavy | Overviews now have useful introductions but can lack detailed diagrams/comparisons | Add a representative workflow diagram or troubleshooting pathway where it adds explanatory value | Planned |
| P2 | Static-site keyboard/accessibility and mobile states aren't reported as tested | Docs JS syntax and Markdown references were verified earlier, but no completed browser interaction matrix | Test keyboard search, anchor navigation, 320–400px layout, scrolling, contrast and screen-reader labels | Planned |

## Recommended “complete category” checklist

A category guide should be **independently usable**. Apply this checklist to every new or existing Quick Action:

- [ ] One-sentence purpose and expected use cases
- [ ] All controls in the category, with correct shipped names
- [ ] Exact mouse, keyboard and numeric interaction behavior
- [ ] Appropriate Actor/Component targets and typical unavailable cases
- [ ] Off, On, Mixed, Not Applicable, alert and protection-state interpretation where relevant
- [ ] Single Actor, folder, attached hierarchy, native group and multi-selection behavior
- [ ] Undo/Redo, native property saving and Focus session-only persistence rules
- [ ] Interaction with Edit Lock and other protection controls
- [ ] Concrete workflow from setup through observable result and reversal
- [ ] Common misconceptions, native-engine prerequisites and unsupported cases
- [ ] Annotated image or short animation of the actual editor UI
- [ ] Technical reference to the relevant property, setter, data owner and source version
- [ ] Reproducible verification steps and precise results, without invented test passes

## Areas most likely to generate support questions

### Visual / Interaction

Questions about when Solo affects light and volume contributions, why Selection Lock still allows Outliner selection, why a script can modify a locked Actor, and which edits persist require contextual examples and side-by-side comparisons.

### Simulation / Physics Events

Documentation should distinguish eligible property state from successful physics simulation or event delivery. Readers need collision, mobility and other prerequisites called out for each control rather than being told only that the property is enabled.

### Network / Navigation

Native properties are inputs, not complete systems. The docs should avoid implying that one click enables replication or rebuilds NavMesh. Recommended improvements are clear “does / does not” examples and native verification steps.

### World / LOD

World Partition loaded/unloaded Actor behavior, Editor Only cooking consequences, HLOD participation and forced LOD vs distance-culling distinctions deserve visual scenarios. LOD's nonstandard click gestures must be documented exactly.

### Settings / Saving

This is the highest-risk content: a casual statement such as “session-only means no checkout” would be too broad. Distinguish Focus-initiated temporary changes, other native edits that may dirty Actor packages, package save overlays, switching persistence OFF/ON and External Actor files. A regression/QA matrix is necessary before claiming all Save All/autosave/Perforce flows are guaranteed.

## Verification responsibility

Use the [Verification Matrix](VERIFICATION_MATRIX.md) for reproducible test cases and observed results. C++ review and JavaScript parsing are useful checks, but do not establish that an editor workflow or browser interaction passed. Do not promote a proposed test to Passed without evidence.

## Proposed documentation work plan

### Pass A — Completeness and factual audit (P0)

Consolidate repeated sections; verify actual native control gestures, category states, target types and persistence behaviors against current code. Ensure each Design page answers “what, why, how and what happens next” without requiring the Technical page.

**Deliverable:** clean category pages, corrected interaction reference and no contradictory control descriptions.

### Pass B — Visual teaching materials (P1)

Capture annotated category screenshots from UE 5.8. Include ordinary On/Off, Mixed and Not Applicable states, protection-level cycling, Solo in a representative scene, and LOD number editing. Use files authored by/for Focus; do not substitute unrelated Unreal screenshots.

**Deliverable:** a small, consistent image/GIF library with alt text and linked captions.

### Pass C — Search and troubleshooting (P1)

Implement content-aware search over documentation Markdown rather than only metadata. Add issue-oriented help cards or FAQ headings for common queries, with deep links to the exact section. Maintain a searchable common control-name/synonym glossary.

**Deliverable:** reliable search for terms that occur in page bodies; exact user-visible troubleshooting steps.

### Pass D — Verification and maintenance (P1/P2)

Add a real UE 5.8 testing checklist covering simple Actors, Primitive Components, Blueprint Actors, native groups, folder selections, World Partition, package saving, Undo/Redo and Edit Lock interactions. Separately verify mobile layout, browser navigation and link/asset integrity. Update the version baseline and source references per release.

**Deliverable:** measured release-specific compatibility and documentation QA report.

### Pass E — Prepare for extensibility (planned product work)

Once the [Roadmap](ROADMAP.md) extension API and Widget Builder are designed, add **API Reference**, **Provider Integration Guide** and **Widget Builder Manual** pages only when implementations exist. In the meantime the roadmap must clearly mark them planned; do not show empty feature categories in the current shipped feature navigation.

## Review principles

- **Current shipped behavior is not future product intent.** Source-backed current docs belong in the category manuals; unimplemented proposals belong in Roadmap.
- **Design explains user behavior, Technical explains engine implementation.** Cross-link without copying whole sections.
- **Useful specificity beats length.** A concise exact click sequence is more valuable than two broad paragraphs.
- **Verification is explicit.** Cite source review separately from passing automation tests or hands-on UE/browser observations.
- **Do not change runtime/editor behavior as part of docs updates.** Improvements here should be docs/site-only unless an implementation task is explicitly approved.

---

[Verification Matrix](VERIFICATION_MATRIX.md) · [Roadmap](ROADMAP.md) · [Quick Actions](02-quick-actions/README.md) · [Maintaining documentation](DOCS_MAINTENANCE.md)
