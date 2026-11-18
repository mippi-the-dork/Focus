# Focus Documentation

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

**Focus** brings compact Actor and Component controls directly into Unreal Engine's World Outliner: visibility, Solo, selection/edit protection, simulation, physics events, lighting, networking, navigation, mobility and LOD. This manual documents both **how to use it** and **what its code actually does**.

## Which documentation should I read?

**Design** pages are the everyday user manual. They describe controls, steps, examples, expected results, and which common misunderstandings to avoid. You do not need to understand Unreal C++ to follow them.

**Technical** pages are the implementation and integration reference. They identify native engine properties and component scopes, editor hooks, package metadata, transactions, persistence rules, and known system boundaries. They are useful when debugging a project, integrating tools, or extending Focus.

Both page types are kept in a matched pair for each feature. They deliberately **do not duplicate the README**: use the Design/Technical switch to move between behavior and implementation.

## Start here

- [Installation](01-getting-started/install/DESIGN.md)
- [First Use](01-getting-started/first-use/DESIGN.md)
- [Common Questions](06-support/faq/DESIGN.md)
- [Complete Native Property Reference](05-implementation/reference/TECHNICAL.md)
- [Session-Only and Persistent Edits](04-settings-and-saving/persistence/TECHNICAL.md)

## Numbered Documentation Categories

| Category | Features | Description |
| --- | ---: | --- |
| [01. Getting Started](01-getting-started/README.md) | 3 | What Is Focus?, Installation and Compatibility, First Use |
| [02. Visual and Interaction](02-visual-and-interaction/README.md) | 4 | Hide in Editor and Hidden in Game, Solo, Selection, Transform and Edit Locks, Folders, Groups and Multi-Selection |
| [03. Native Quick Properties](03-quick-properties/README.md) | 5 | Simulation and Physics Events, Lighting and World, Network and Navigation, Mobility and LOD, Applicability, Mixed States and Undo |
| [04. Settings and Saving](04-settings-and-saving/README.md) | 4 | Category Visibility, State Colors, Session-Only and Persistent Edits, Restore, Save, Undo and Redo |
| [05. Workflows and Implementation](05-implementation/README.md) | 4 | Practical Workflows, How Focus Works Internally, Full Quick-Property Reference, Data Ownership and Source Map |
| [06. Support and Troubleshooting](06-support/README.md) | 3 | Questions and Answers, Troubleshooting, Updates and Support |

## Behavior boundaries

Focus is an editor utility, not a security or runtime permission layer. Folder and hierarchy controls affect loaded Actors. Some quick properties are unavailable on Actors without eligible registered components. Session-only edits can be visible in the editor without becoming saved package state. See the Design/Technical pairs for exact behavior.

## Support

- [Focus repository](https://github.com/mippi-the-dork/Focus)
- [Focus releases](https://github.com/mippi-the-dork/Focus/releases)
- [Report issues](https://github.com/mippi-the-dork/Focus/issues)

## Documentation status

The public Focus 2.1.0 C++ code, its settings, editor integration and quick-property definitions were inspected. Live website rendering and project-specific UE 5.8 workflows remain separate QA tasks.
