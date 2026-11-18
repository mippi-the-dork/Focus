# 05. Support and Troubleshooting

Most Focus questions fall into one of four groups: **the control isn't visible**, **it doesn't affect a target**, **a lock doesn't block an expected action**, or **a change isn't saved**. Start with the symptom, then follow the focused guide.

## Identify the problem

| Symptom | First thing to check | Next reference |
| --- | --- | --- |
| Focus column missing | Plugin enabled; proper engine/platform; World Outliner column visibility | [Troubleshooting](troubleshooting/DESIGN.md) |
| A category is missing | Focus column-header category menu | [Category Visibility](../03-settings-and-saving/categories/DESIGN.md) |
| Control is dark/unavailable | Compatible registered Actor/Component and editable native property | [Quick Actions](../02-quick-actions/README.md) |
| A folder didn't affect something | Loaded World Partition state, attachment/group membership, time of the edit | [Practical Workflows](../04-workflows-and-implementation/workflows/DESIGN.md) |
| Selection Lock still permits Outliner selection | This is an intentional exception | [Interaction](../02-quick-actions/interaction/DESIGN.md) |
| A script changed a locked Actor | Locks only guard supported editor paths | [Interaction Technical](../02-quick-actions/interaction/TECHNICAL.md) |
| A change disappeared after reload | Persistence checkbox and whether the Actor package was saved | [Settings and Saving](../03-settings-and-saving/README.md) |

## Before reporting a problem

Try reproducing the behavior with one ordinary Actor in a disposable UE 5.8 level, then add a folder/multi-selection if needed. Note the exact Actor and Component classes, whether the Actor was loaded, the editing path, and the persistence preference.

## Getting help

Use the [Focus GitHub Issues](https://github.com/mippi-the-dork/Focus/issues) tracker. Attach concise reproduction steps and relevant Unreal Output Log lines. Include Focus and Unreal Engine versions, installation source, and whether you were working under World Partition.

## Support topics

| Topic | What it explains | Design | Technical |
| --- | --- | --- | --- |
| **Frequently Asked Questions** | Common behavior, scope and limitations | [Design](faq/DESIGN.md) | [Technical](faq/TECHNICAL.md) |
| **Troubleshooting** | Step-by-step investigation of problems | [Design](troubleshooting/DESIGN.md) | [Technical](troubleshooting/TECHNICAL.md) |
| **Updates and Support** | Version compatibility, installing updates and bug reports | [Design](updating/DESIGN.md) | [Technical](updating/TECHNICAL.md) |

For normal user behavior, start in Design; for engine internals and debugging, use Technical.
