# Focus v2.1.0 Verification Matrix

**Target:** Unreal Engine 5.8.x · Win64 · Focus v2.1.0.  
**Purpose:** Reproducible release and documentation quality checks. These are **not reported as passed tests**. Status `Not run` means a test still needs to be executed in the Unreal Editor or browser. `Source-reviewed` confirms only the implementation/documentation was inspected, not that the behavior passed testing.

## Record the environment

When executing tests, record the **exact engine version, Focus commit/release, project type, World Partition and One File Per Actor settings, Perforce/other source-control state, tester, test date, Unreal Output Log and screenshots**. Do not overwrite historical results without preserving evidence.

## Quick Actions and Editor usability

| ID | Action to test | Expected behavior | Status |
| --- | --- | --- | --- |
| FOC-01 | Enable Focus on a fresh UE 5.8 Win64 project; open Outliner | Focus column is present, Visual and Interaction default visible | Not run |
| FOC-02 | Toggle each of ten category checkboxes, then reset | Width/layout changes; no Actor properties changed; reset shows Visual + Interaction | Not run |
| FOC-03 | Hide in Editor, Hidden in Game and Solo independently | Each action uses its distinct native/editor behavior | Not run |
| FOC-04 | Toggle Selection Lock, then cycle Protection | Selection independent; Unlocked → Transform → Edit → Unlocked | Not run |
| FOC-05 | Viewport-pick vs Outliner-select a Selection-Locked Actor | Normal viewport selection blocked; intentional Outliner selection works | Not run |
| FOC-06 | Native Details transform edit under Transform Lock | Standard transform edits blocked; unrelated properties remain editable | Not run |
| FOC-07 | Native Details, rename and component editing under Edit Lock | Supported edits blocked without losing Details browsing | Not run |
| FOC-08 | Solo scene with meshes, lights, volumes and hidden Actors | Primitive isolation and documented exceptions are observed | Not run |
| FOC-09 | Simulation: gravity + physics on compatible and incompatible components | Native values change only for eligible editable targets | Not run |
| FOC-10 | Physics Events: hit, overlap, wake with correct/incorrect collision setup | Flags change; events require appropriate native prerequisites | Not run |
| FOC-11 | Lighting: toggle channel 1 while channel 0 is enabled | Channel 1 changes without clearing channel 0 | Not run |
| FOC-12 | World: editor-only, main-world, spatial loading and HLOD | Instance-editable native flags change, subject to world configuration | Not run |
| FOC-13 | Network: Actor and Primitive settings; multi-client PIE | Native fields change; replication effect verified separately | Not run |
| FOC-14 | Navigation: Actor Component, Shape and Primitive flags | Eligible component types change; NavMesh result checked separately | Not run |
| FOC-15 | Mobility: top/middle/bottom segment selection | Static, Stationary, Movable set using native scene-component setter | Not run |
| FOC-16 | LOD: click, Alt-click, wheel, double-click, right-click | +1, −1, ±1, numeric entry and native context menu respectively | Not run |
| FOC-17 | LOD: choose values 0/1/2 and invalid higher value | Automatic/LOD0/LOD1, plus Alert on out-of-range | Not run |
| FOC-18 | Mixed eligible and ineligible Actor/Component multi-selection | Mixed and Not Applicable states and eligible counts correct | Not run |
| FOC-19 | Apply to nested/collapsed folder, attached hierarchy and native group | Includes loaded eligible descendants regardless of filtered rows | Not run |
| FOC-20 | Add Actor after folder edit; unload WP Actor before bulk action | New/unloaded Actor does not inherit or receive earlier action | Not run |
| FOC-21 | Undo/Redo single and multi-Actor quick property changes | Native values and state restored consistently | Not run |

## Persistence, packages and source control

| ID | Action to test | Expected behavior | Status |
| --- | --- | --- | --- |
| FOC-22 | Persistent Focus locks → Save All → reload | Last saved Actor package metadata restores the controls | Not run |
| FOC-23 | Solo or Hidden in Game set session-only → Save All → reload | Temporary Focus value does not replace saved baseline | Not run |
| FOC-24 | Session-only Hidden in Game change on a clean Perforce OFPA Actor | No **Focus-owned** unintended package dirtiness or checkout; unrelated edits examined separately | Not run |
| FOC-25 | Native Details change to same property after temporary Focus edit | Native edit takes precedence under normal save behavior | Not run |
| FOC-26 | Turn persistence OFF then ON during temporary edit | OFF does not undo; ON promotes live value for future saving | Not run |
| FOC-27 | Restore Temporary Changes after several separate controls | Pending Focus temporary changes revert; persistent/native edits remain | Not run |
| FOC-28 | Save External Actor packages, autosave and interrupted save | Pre/post-save baselines and current editor values remain coherent | Not run |
| FOC-29 | Stream in World Partition Actor, reopen level and compile Blueprint | Correct saved Focus state is restored after load/replacement | Not run |
| FOC-30 | Reload/unload plugin while editor is active | Guards, mode, input and save callbacks safely tear down | Not run |
| FOC-31 | Large level and Outliner with many loaded Actors | Responsive row refresh and category changes; no unexpected frame hitch | Not run |

## Documentation website

| ID | Action to test | Expected behavior | Status |
| --- | --- | --- | --- |
| DOC-01 | Navigate all five sections and ten Quick Actions | Links, breadcrumbs, Design/Technical switch work | Link/source reviewed, browser not run |
| DOC-02 | Search a title then a phrase only in a Markdown body | Title appears immediately, body content matches after indexing | Source reviewed, browser not run |
| DOC-03 | Simulate a failed/slow Markdown fetch during content indexing | Title search still works; index failure doesn't crash site | Source reviewed, browser not run |
| DOC-04 | Keyboard /, Ctrl/Cmd+K, arrows, Enter, Escape | Search opens, navigates, activates and closes | Not run |
| DOC-05 | Test 320px, 375px, tablet and desktop widths | Sidebar, content, tables and TOC remain usable | Not run |
| DOC-06 | View updated hero, header icon, Focus controls, existing Solo/Lock GIFs and all placeholder figures | Assets render inside article width; captions/alt descriptions are clear; no overflow | Source asset paths checked, browser not run |
| DOC-07 | Open old ?page= documentation URLs | Aliases lead to relevant current pages | Source reviewed, browser not run |
| DOC-08 | Check heading links, history/back, focus and contrast | Correct links and keyboard accessibility | Not run |
| DOC-09 | Read Roadmap v1.0, v2.0 and v3.0 labels | Shipped history distinct from v3.0 planned capabilities | Content reviewed |

## Recording real results

Replace `Not run` only after testing. Record **Passed**, **Failed**, **Blocked** or **Not applicable**, with version/build, exact steps, observed result, screenshots/logs and issue link for failures. Source review is **not** a passing editor test.

Use Unreal's native Automation Framework for repeatable cases when automation is implemented. No other testing plugin is required.

[Documentation Audit](DOCUMENTATION_AUDIT.md) · [Roadmap](ROADMAP.md) · [Quick Actions](02-quick-actions/README.md)
