# Focus Documentation Media Inventory

Each media slot includes **two distinct lines beneath its image**: a normal reader-facing caption, followed by **Capture instructions · Image** or **Capture instructions · GIF** with the precise framing, states, sequence, or diagram contents needed. The second line is intentionally visible during this documentation-production phase.

The detailed capture requirements are also stored in [the media registry](assets/media-slots.json) under `capture_instructions`, keyed by stable media ID.

The site's current artwork, screenshots and GIFs should show real Focus editor behavior. The new figure placements display the supplied neutral placeholder until their dedicated assets are ready. Reader-facing captions already describe the eventual examples, and should remain accurate after replacement.

**Image** is a still screenshot or diagram; **GIF** is an animation of a sequence or interaction. A GIF slot currently displays the same static PNG placeholder as an image slot.

| Page | Final type | Intended asset |
| --- | --- | --- |
| [01-getting-started/overview/DESIGN.md ](01-getting-started/overview/DESIGN.md) | IMAGE | `overview-outliner.png` |
| [01-getting-started/overview/TECHNICAL.md ](01-getting-started/overview/TECHNICAL.md) | IMAGE | `overview-editor-integration.png` |
| [01-getting-started/install/DESIGN.md ](01-getting-started/install/DESIGN.md) | IMAGE | `installation-plugin-folder.png` |
| [01-getting-started/first-use/DESIGN.md ](01-getting-started/first-use/DESIGN.md) | GIF | `first-use-controls.gif` |
| [01-getting-started/first-use/TECHNICAL.md ](01-getting-started/first-use/TECHNICAL.md) | IMAGE | `first-use-row-state.png` |
| [02-quick-actions/README.md ](02-quick-actions/README.md) | IMAGE | `quick-actions-category-menu.png` |
| [02-quick-actions/visual/DESIGN.md ](02-quick-actions/visual/DESIGN.md) | IMAGE | `visual-editor-vs-game.png` |
| [02-quick-actions/visual/TECHNICAL.md ](02-quick-actions/visual/TECHNICAL.md) | IMAGE | `visual-solo-primitive-filter.png` |
| [02-quick-actions/interaction/DESIGN.md ](02-quick-actions/interaction/DESIGN.md) | IMAGE | `interaction-mixed-protection.png` |
| [02-quick-actions/interaction/TECHNICAL.md ](02-quick-actions/interaction/TECHNICAL.md) | IMAGE | `interaction-native-guards.png` |
| [02-quick-actions/simulation/DESIGN.md ](02-quick-actions/simulation/DESIGN.md) | IMAGE | `simulation-controls.png` |
| [02-quick-actions/simulation/DESIGN.md ](02-quick-actions/simulation/DESIGN.md) | GIF | `simulation-prop-physics.gif` |
| [02-quick-actions/simulation/TECHNICAL.md ](02-quick-actions/simulation/TECHNICAL.md) | IMAGE | `simulation-component-scopes.png` |
| [02-quick-actions/physics-events/DESIGN.md ](02-quick-actions/physics-events/DESIGN.md) | IMAGE | `physics-events-controls.png` |
| [02-quick-actions/physics-events/DESIGN.md ](02-quick-actions/physics-events/DESIGN.md) | GIF | `physics-events-overlap.gif` |
| [02-quick-actions/physics-events/TECHNICAL.md ](02-quick-actions/physics-events/TECHNICAL.md) | IMAGE | `physics-events-path.png` |
| [02-quick-actions/lighting/DESIGN.md ](02-quick-actions/lighting/DESIGN.md) | IMAGE | `lighting-controls.png` |
| [02-quick-actions/lighting/DESIGN.md ](02-quick-actions/lighting/DESIGN.md) | IMAGE | `lighting-channel-example.png` |
| [02-quick-actions/world/DESIGN.md ](02-quick-actions/world/DESIGN.md) | IMAGE | `world-controls.png` |
| [02-quick-actions/world/DESIGN.md ](02-quick-actions/world/DESIGN.md) | IMAGE | `world-partition.png` |
| [02-quick-actions/world/TECHNICAL.md ](02-quick-actions/world/TECHNICAL.md) | IMAGE | `world-native-flags.png` |
| [02-quick-actions/network/DESIGN.md ](02-quick-actions/network/DESIGN.md) | IMAGE | `network-controls.png` |
| [02-quick-actions/network/TECHNICAL.md ](02-quick-actions/network/TECHNICAL.md) | IMAGE | `network-scopes.png` |
| [02-quick-actions/navigation/DESIGN.md ](02-quick-actions/navigation/DESIGN.md) | IMAGE | `navigation-controls.png` |
| [02-quick-actions/navigation/DESIGN.md ](02-quick-actions/navigation/DESIGN.md) | GIF | `navigation-navmesh.gif` |
| [02-quick-actions/mobility/DESIGN.md ](02-quick-actions/mobility/DESIGN.md) | IMAGE | `mobility-options.png` |
| [02-quick-actions/mobility/DESIGN.md ](02-quick-actions/mobility/DESIGN.md) | GIF | `mobility-bulk.gif` |
| [02-quick-actions/lod/DESIGN.md ](02-quick-actions/lod/DESIGN.md) | IMAGE | `lod-controls.png` |
| [02-quick-actions/lod/DESIGN.md ](02-quick-actions/lod/DESIGN.md) | GIF | `lod-gestures.gif` |
| [02-quick-actions/lod/TECHNICAL.md ](02-quick-actions/lod/TECHNICAL.md) | IMAGE | `lod-range.png` |
| [03-settings-and-saving/README.md ](03-settings-and-saving/README.md) | IMAGE | `settings-overview.png` |
| [03-settings-and-saving/categories/DESIGN.md ](03-settings-and-saving/categories/DESIGN.md) | IMAGE | `settings-category-visibility.png` |
| [03-settings-and-saving/colors/DESIGN.md ](03-settings-and-saving/colors/DESIGN.md) | IMAGE | `settings-state-colors.png` |
| [03-settings-and-saving/persistence/DESIGN.md ](03-settings-and-saving/persistence/DESIGN.md) | GIF | `settings-persistence-workflow.gif` |
| [03-settings-and-saving/persistence/TECHNICAL.md ](03-settings-and-saving/persistence/TECHNICAL.md) | IMAGE | `settings-package-overlay.png` |
| [03-settings-and-saving/restore/DESIGN.md ](03-settings-and-saving/restore/DESIGN.md) | GIF | `settings-restore-temporary.gif` |
| [04-workflows-and-implementation/README.md ](04-workflows-and-implementation/README.md) | IMAGE | `workflows-hierarchy.png` |
| [04-workflows-and-implementation/workflows/DESIGN.md ](04-workflows-and-implementation/workflows/DESIGN.md) | GIF | `workflows-bulk-lock.gif` |
| [04-workflows-and-implementation/workflows/TECHNICAL.md ](04-workflows-and-implementation/workflows/TECHNICAL.md) | IMAGE | `workflows-scope-resolution.png` |
| [04-workflows-and-implementation/architecture/TECHNICAL.md ](04-workflows-and-implementation/architecture/TECHNICAL.md) | IMAGE | `architecture-editor-layers.png` |
| [04-workflows-and-implementation/source-map/TECHNICAL.md ](04-workflows-and-implementation/source-map/TECHNICAL.md) | IMAGE | `source-map-data-ownership.png` |
| [05-support-and-troubleshooting/faq/DESIGN.md ](05-support-and-troubleshooting/faq/DESIGN.md) | IMAGE | `support-mixed-unavailable.png` |
| [05-support-and-troubleshooting/troubleshooting/DESIGN.md ](05-support-and-troubleshooting/troubleshooting/DESIGN.md) | IMAGE | `support-unavailable-tooltip.png` |
| [05-support-and-troubleshooting/updating/DESIGN.md ](05-support-and-troubleshooting/updating/DESIGN.md) | IMAGE | `support-plugin-update.png` |
| [ROADMAP.md ](ROADMAP.md) | IMAGE | `roadmap-milestones.png` |
| [ROADMAP.md ](ROADMAP.md) | IMAGE | `roadmap-extension-api.png` |

To replace a slot, add its final media file under `docs/assets/media/`, update that figure's `<img src>` to the correct relative path, and retain or refine its accurate caption. Use genuine screenshots and animations from the running plugin; do not substitute fabricated interface imagery.

Keep the two actual Focus GIFs (`Focus-Solo.gif` and `Focus-Locking.gif`) and the provided updated hero where they are already used.
