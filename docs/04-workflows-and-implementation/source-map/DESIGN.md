# Source Code and Data Ownership — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## Where should you look first?

If you are using Focus, start with the Design pages rather than searching its source. If you're extending Focus or diagnosing an integration issue, its C++ files divide responsibilities in a reasonably clear way.

| Question | Start with |
| --- | --- |
| Why is a property control unavailable? | Quick Properties source |
| How does the Focus Outliner column render and respond to clicks? | Main module |
| Why does Edit Lock not block a particular native action? | Interaction/Command guards and editor mode |
| How does Solo avoid rewriting scene visibility? | View extension in main module |
| How do temporary changes interact with Save All? | Focus state and package-save hooks |
| Where are custom colors and persistence options? | Settings |
| Why is this plugin absent from packaged games? | Plugin descriptor and Build.cs |

## For users reporting an issue

Provide the control name, Actor and Component types, whether a folder/group/multi-selection was used, loaded state under World Partition, whether persistence was on, and exact steps. Technical owners can then follow the correct path without guessing.

## What is saved where?

Preferences are editor-per-project user settings. Persistent Focus Visual/Interaction state is held alongside the Actor's normal packages. Other quick properties are native Actor/Component data. There is no Focus runtime Actor that users must place.

---

For implementation, native data and engine-integration details, [see Technical](TECHNICAL.md).
