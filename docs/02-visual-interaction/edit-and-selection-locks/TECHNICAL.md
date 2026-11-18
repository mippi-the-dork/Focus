# Selection, Transform and Edit Locks — Technical

> **Focus v2.1.0** · Unreal Engine 5.8.x · Win64 · Editor Only

Three separate protection behaviors and lock states.

This reference describes the shipped editor-facing behavior and native property boundaries documented in the Focus repository. It is not an assertion that the implementation has been independently audited line-by-line.

### Selection Lock

Prevent accidental viewport selection while keeping Actors selectable in the World Outliner. Focus-initiated Selection Lock changes can be saved or session-only.

### Transform Lock

Protect Actor location, rotation, and scale while leaving other properties editable. Transform Lock and Edit Lock share the Protection Lock persistence setting.

### Edit Lock

Protect supported Actor and Component editing while keeping their properties available for inspection. Edit Lock shares the Protection Lock persistence setting with Transform Lock.

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

---

## Related information

- [Focus source and complete README](https://github.com/mippi-the-dork/Focus#readme)
- [Focus issue tracker](https://github.com/mippi-the-dork/Focus/issues)
- [Installation](../01/installation/DESIGN.md)


