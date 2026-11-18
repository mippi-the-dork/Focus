# Lighting and World — Technical

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Lighting channels, shadow, HLOD and World Partition.

## Verified behavior

Lighting channel edits use SetLightingChannels, retaining the other channels.

## Native behavior and original reference

## Lighting

The **Lighting** miniwidget groups four independent properties. Its **three upright upper bars** are Lighting Channels **0**, **1**, and **2**, from left to right. The **lower horizontal bar** controls **Cast Shadows**.

### Lighting channels

Lighting channels help determine which eligible lights can interact with which eligible primitives. For a light to affect a primitive through this mechanism, both need compatible channel assignments, and the normal rendering rules must still permit the interaction.

You can use the upper bars to check or change channels on supported light and primitive components. Focus changes the selected channel without intentionally resetting the other two channels. For example, you might place a set of character meshes on Channel 1 and assign a dedicated light to that same channel while leaving ordinary environment lighting on Channel 0.

The individual channel controls can show different states within a multi-selection. Hover a bar to confirm its channel and target eligibility before toggling it.

### Cast Shadows

The bottom bar toggles the corresponding **Cast Shadows** property on eligible components. A component's final shadow behavior also depends on its material, lighting type, and render configuration. Turning this on does not override those other requirements.

This category is useful for checking channel assignments and shadow casting across a group of supported objects without repeatedly opening their individual Details panels.

---

## World

The **World** miniwidget displays four Actor-level settings in a **two-by-two grid**. Unlike categories that operate on component properties, these controls apply to an Actor's own world-related flags.

### Main-world and editor-only behavior

The **top-left square**, **In Main World Only**, controls the corresponding main-world restriction where Unreal supports it. It does not otherwise change which level or folder contains the Actor.

The **top-right square**, **Is Editor Only Actor**, marks the Actor as editor-only where the engine honors that flag. Editor-only Actors can be excluded from cooked gameplay content, so use this carefully if other gameplay objects reference the Actor. Hiding something in the editor is not the same as marking it editor-only.

### World Partition and HLOD

The **bottom-left square**, **Is Spatially Loaded**, controls eligibility for World Partition spatial loading. Its effect depends on whether the world uses World Partition and how the world is configured. Changing the flag does not itself load every cell or alter unloaded Actors.

The **bottom-right square**, **Include Actor in HLOD**, controls participation in supported Hierarchical Level of Detail generation. This does not immediately build new HLODs. The world must have an applicable HLOD setup and generation workflow.

When preparing a large World Partition level, use the bottom two squares to review spatial loading and HLOD participation across loaded Actors. The four settings remain independent, and only eligible, instance-editable Actor properties can be changed.

---

## Network

## Verification boundary

These details are grounded in current v2.1.0 C++ source and the public README. Editor-specific combinations, such as external Actor saving and nonstandard editing modes, require validation in the target Unreal project.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
