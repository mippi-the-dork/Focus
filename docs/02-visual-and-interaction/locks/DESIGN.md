# Selection, Transform and Edit Locks — Design

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Choose the protection you need

| Control | What it protects | What remains possible |
| --- | --- | --- |
| Selection Lock | Accidental viewport picking | Select from World Outliner; inspect or unlock |
| Transform Lock | Normal position, rotation and scale editing | Edit unrelated properties |
| Edit Lock | Supported Actor and Component edits, including transforms | Inspect Details, use search and category navigation |

Selection Lock is independent of the other two. **Transform Lock** and **Edit Lock** are protection levels: Edit Lock includes transform protection.

## Practical examples

Use Selection Lock on dense background meshes so clicking through the scene is easier. Transform Lock finished architecture without preventing an artist from adjusting its material. Use Edit Lock on a reviewed assembly when accidental modifications would be costly.

## A locked Actor is not inaccessible

Select it in the Outliner to inspect and unlock it. Some specialized tools, scripts or custom editor plugins may still change it: Focus guards supported Unreal editing paths, not arbitrary code.

## Hierarchy behavior

A mixed protection icon indicates different eligible Actors have different levels. Applying a level affects eligible loaded Actors in that scope. Before changing many Actors, confirm you selected the right folder or selection set.

---

## Technical detail

[Read the Technical page](TECHNICAL.md) for engine integration, persistence and implementation boundaries.
