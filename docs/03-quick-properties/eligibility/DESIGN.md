# Applicability, Mixed States and Undo — Design

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Why a quick control is unavailable.

## Verified behavior

Only instance-editable properties on eligible registered Actor/Component targets can be changed. Edit-locked Actors are skipped; writes use native transactions.

## Source eligibility checks

The source excludes template objects, missing or unregistered components, properties without CPF_Edit, instance-disabled, EditConst or EditCondition metadata, and properties denied by CanEditChange. A disabled boolean, a mixed value and an unavailable property are distinct states. Quick edits call Modify, PreEditChange and PostEditChangeProperty; physics/lighting/mobility/LOD changes use their dedicated engine setters where necessary.
## Usage and workflows

# Mixed States

Folders and selections can contain Actors with different Focus settings.

Focus represents this using a mixed state.

For example:

```text
Environment
├── Wall_A        Transform Locked
├── Wall_B        Transform Locked
└── Door          Unlocked
```

The protection control for the `Environment` Folder will show that its contents do not all share the same state.

This makes it easier to understand the actual state of a hierarchy without expanding and inspecting every Actor individually.

---

# Example Workflow

---

### Mixed States

Focus indicates when a hierarchy contains a mixture of enabled and disabled states.

### Undo / Redo

Focus control changes participate in Unreal Engine's normal Undo and Redo workflow.

### Persistent Settings

## Expected limits

Focus is an Editor-only workflow tool. Folder and selected-row changes affect eligible loaded Actors and are not automatically inherited by Actors added later. Supported native protections do not prevent arbitrary external tools or scripts.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
