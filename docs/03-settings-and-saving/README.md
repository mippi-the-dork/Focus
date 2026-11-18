# 03. Settings and Saving

Focus has two separate kinds of configuration: **which controls appear in your Outliner**, and **whether Focus-authored Visual/Interaction changes persist**. Understanding this distinction is especially important when working with large shared levels, One File Per Actor, or source control.

## Where to configure Focus

Open **Editor Preferences → Plugins → Focus** to customize control colors and Visual/Interaction persistence. Use the **Focus column-header menu** to show or hide the ten Quick Action categories and access **Restore Temporary Changes**.

## Three things that are easy to confuse

| Decision | What it changes | What it does not change |
| --- | --- | --- |
| Category visibility | Which Quick Action groups appear in the Outliner | The values of the underlying Actor/Component properties |
| State colors | How enabled, disabled, mixed, unavailable and temporary values look | Whether those values are actually saved |
| Persistence | Whether Focus-authored Visual/Interaction edits are session-only or saved | How independent native Details edits and the eight other Quick Action groups save |

## A safe temporary workflow

Suppose you're inspecting a cluttered level. Disable persistence for Solo, Solo your area of interest, then inspect the scene. Use **Restore Temporary Changes** to discard the temporary Solo edits when finished. Your normal saved Actor state is not supposed to be replaced by that temporary isolation.

Turning a persistence checkbox **off** does not undo an existing value. Turning it **on** makes the current value eligible for the next save.

## Persistent Actor data

Focus's persistent locks and other Focus-specific flags are stored with Actor packages, including External Actor packages. Its other Quick Actions modify Unreal's native properties using ordinary save behavior. A temporary Focus action cannot guarantee that an unrelated native edit won't dirty or check out an Actor package.

## Settings topics

| Topic | What it explains | Design | Technical |
| --- | --- | --- | --- |
| **Category Visibility** | Which controls appear and default category layout | [Design](categories/DESIGN.md) | [Technical](categories/TECHNICAL.md) |
| **State Colors** | Reading and customizing signals | [Design](colors/DESIGN.md) | [Technical](colors/TECHNICAL.md) |
| **Session-Only and Persistent Edits** | Which values save and why | [Design](persistence/DESIGN.md) | [Technical](persistence/TECHNICAL.md) |
| **Restore, Save, Undo and Redo** | Undo, temporary restoration, reload and save handling | [Design](restore/DESIGN.md) | [Technical](restore/TECHNICAL.md) |

For exact behavior of each individual control, use [Quick Actions](../02-quick-actions/README.md).
