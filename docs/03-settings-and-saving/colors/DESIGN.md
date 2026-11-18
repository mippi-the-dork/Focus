# Control State Colors and Reading Signals — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## Five different visual meanings

<!-- Media target: IMAGE | settings-state-colors | docs/assets/media/settings-state-colors.png -->
<figure class="doc-media" data-kind="image" data-media-id="settings-state-colors">
  <img src="../../assets/media-placeholder.png" alt="Focus controls showing enabled, disabled, mixed, unavailable and alert colors" loading="lazy">
  <figcaption>Distinct visual states help differentiate active settings, mixed selections, unavailable properties and important warnings.</figcaption>
</figure>


| Meaning | Typical default | What it tells you |
| --- | --- | --- |
| Enabled | White | The control is available and on |
| Disabled | Gray | Available but off |
| Mixed | Yellow | Different values across the eligible scope |
| Not Applicable | Very dark gray | No eligible editable target |
| Alert | Red | Strong protection/warnings, including invalid forced LOD |

Colors are configurable in **Editor Preferences → Plugins → Focus**. You can change them to suit your theme or improve readability.

## Session-only colors

When a Visual or Interaction control has persistence **disabled**, Focus uses a separate color set. The shipped examples include **blue** for enabled, **orange** for mixed and **pink** for strong protection/alert. This is deliberately a visual reminder that a change you can currently see may not be part of the saved level.

Disabled and unavailable also have separate session-only colors. They must not be confused: disabled says the control could be enabled; unavailable says no compatible editable target exists.

## Read tooltips before making bulk changes

The miniwidget shape and state color work together. Hover to confirm which control you are about to use and how many eligible Actors/components it affects. A yellow or orange mixed indicator is especially important before toggling a folder containing many Actors.

## Changing appearance does not change the level

Color adjustments affect Focus's editor display. They do not modify Actor metadata, physics settings, or packaged output.

---

For exact native settings, serialization and recovery, [see Technical](TECHNICAL.md).
