# Questions and Answers — Technical

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Common behavioral questions with clear answers.

## Verified behavior

Unloaded Actors are not changed; Solo is viewport-only; Outliner selection still works with Selection Lock; custom tools can bypass Edit Lock.

## Fast answers

**Can I select a Selection-Locked Actor in the Outliner?** Yes. The lock intentionally prevents viewport picking rather than all access.

**Does Edit Lock stop scripts?** No. It protects standard supported editor interactions, not arbitrary scripts and custom tools.

**Does Solo rewrite other Actors' visibility?** No. It filters visible primitives per editor view; lights/volumes can still contribute.

**Why is a quick control unavailable?** No eligible instance-editable property or compatible registered component may exist, or an Unreal property condition may restrict it.

**Do new Actors inherit folder operations?** No. Apply the operation again after loading or adding them.

**Can I edit unloaded World Partition Actors?** No, not through these bulk controls. Load the Actors first.

**What does LOD 0 mean?** Automatic; 1 forces the mesh's first LOD, and invalid high values display an alert.

**Does session-only mean native Details edits are temporary?** No. Focus's Visual/Interaction persistence options affect Focus-initiated edits only.

**Can a plugin affect packaged games?** Focus does not load as a runtime module. It is an Editor plugin.
## Native behavior and original reference

# What Focus Does Not Do

Focus is an **editor workflow utility**, not a permissions or security system.

It does not:

- Prevent deliberate source-code changes
    
- Prevent scripts from modifying Actors
    
- Replace source control
    
- Implement user permissions
    
- Automatically check out files
    
- Prevent another developer from unlocking something
    
- Modify packaged game behavior
    
- Automatically apply Folder states to Actors added later
    

Focus is intended to prevent accidental editor interactions and make complex scenes easier to manage.

---

# Limitations

### Newly Added Actors

Focus Folder and hierarchy operations affect the Actors currently contained within that scope.

They are not inheritance rules.

If another Actor is added to a Folder later, apply the Focus state again if you want the new Actor included.

### World Partition

Bulk operations affect loaded Actors.

Unloaded World Partition Actors are not modified.

Load the required Actors before performing a bulk Focus operation.

### Selection Lock

Selection Lock prevents normal viewport selection.

The Actor remains intentionally selectable through the World Outliner so it can be inspected or unlocked.

### Edit Lock

Edit Lock protects supported standard editor workflows.

Specialized editor tools, scripts, custom editor systems, or direct engine operations may provide ways to modify an Actor outside those guarded workflows.

### Solo

Solo primarily isolates rendered primitive components.

Some scene contributions, such as lighting or volumes, may continue affecting the viewport even when their Actor is outside the Solo set.

### Session-Only Native Properties

Session-only **Hidden in Game** and native movement locking use package pre-save and post-save handling to avoid writing temporary values as persistent property changes. This needs in-editor validation across Save All, autosaves, External Actors / World Partition, Undo/Redo, and interrupted or failed saves. On a disposable level, confirm that Focus-only temporary edits do not request checkout on an otherwise clean Actor, that unrelated native edits save correctly, and that reloading restores the last persistent values.

An Actor already marked dirty for unrelated reasons can still appear in Unreal's save or checkout workflows. Focus's persistence options do not suppress native Details panel or Outliner edits, and they do not prevent other plugins or tools from dirtying a package.

---

# Troubleshooting

## Verification boundary

These details are grounded in current v2.1.0 C++ source and the public README. Editor-specific combinations, such as external Actor saving and nonstandard editing modes, require validation in the target Unreal project.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
