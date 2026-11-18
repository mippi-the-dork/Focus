# Mobility and LOD — Technical

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Component mobility, forced LOD and culling.

## Verified behavior

Mobility uses SetMobility, forced LOD uses mesh-specific setters; 0 is automatic and n forces LOD index n-1.

## Native behavior and original reference

## Mobility

The **Mobility** miniwidget is a single upright pill divided into **Static**, **Stationary**, and **Movable** sections, from top to bottom. Unlike the independent switches in most other categories, these three positions are **mutually exclusive choices for one native component property**.

### Selecting a mobility mode

**Static** is intended for components that are not expected to move during play. **Stationary** represents a limited-movement mode where supported, often relevant to particular rendering or lighting configurations. **Movable** allows the component to move dynamically, subject to its class and other engine rules.

Click the corresponding third of the pill to assign that mobility mode to eligible Scene Components. Selecting Movable does not automatically enable physics simulation, and selecting Static does not automatically lock an Actor against editor edits. Focus's Transform Lock and Edit Lock are separate controls.

### Reading mixed and unavailable states

When eligible components disagree on their mobility settings, the entire pill shows a **Mixed-colored outline** with neutral interior bands. It does not pretend one of the three modes is selected for the whole scope. When values agree, the selected band uses the Enabled color and the others use the Disabled color.

If no editable component property exists, the control is Not Applicable. Some component types also have restrictions on available mobility modes. Use the tooltip and the native Details panel to inspect restrictions before attempting bulk mobility changes.

---

## LOD

The **LOD** miniwidget combines one numeric control with two independent culling switches. The **upper number** is **Forced LOD Model**; the **two lower bars** are **Never Distance Cull** and **Allow Cull Distance Volume**.

### Forced LOD Model

A value of **`0`** means automatic LOD selection. Positive values use Unreal's **one-based forced-LOD convention**: **`1`** forces mesh LOD 0, **`2`** forces mesh LOD 1, and so on. The number applies to eligible static or skinned mesh components, not to every possible Actor type.

You can operate the number without opening another editor:

- **Left-click** to increment it by one.
- **Alt + left-click** to decrement it by one.
- **Mouse wheel** over the number to adjust it.
- **Double-click** to open the numeric editor and enter a value directly.

The editor accepts values from **0 to 255**, but a particular mesh may contain fewer LODs than the value requests. In that case, Focus retains the native value and displays an **Alert-colored warning** instead of silently selecting a different LOD. Hover the number for its current value and applicable range.

### Distance culling

**Never Distance Cull**, the first lower bar, prevents eligible primitives from being removed by supported distance-culling systems that honor the setting. It does not override all visibility, HLOD, or World Partition behavior.

**Allow Cull Distance Volume**, the second lower bar, permits an eligible primitive to be influenced by Cull Distance Volumes. A suitable volume and level setup must exist before the setting can affect rendering.

A useful workflow is to select several meshes, adjust their forced LOD for a visual review, then return the number to `0` for automatic selection. Set the two culling bars separately according to the intended distance behavior. **Right-click** on the widget continues to use the native Outliner context menu.

---

# Working with Folders and Hierarchies

## Verification boundary

These details are grounded in current v2.1.0 C++ source and the public README. Editor-specific combinations, such as external Actor saving and nonstandard editing modes, require validation in the target Unreal project.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
