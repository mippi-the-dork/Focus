# Frequently Asked Questions — Design

**Focus v2.1.0 · User guide and workflow reference**

## Basics

**Does Focus change my packaged game?** No. Focus is an Unreal Editor plugin. The 28 native quick-property groups may change ordinary properties that subsequently influence a game, but the Focus editor UI itself is not packaged.

**Why isn't every category visible?** Visual and Interaction are enabled by default. Use the Focus column-header menu to reveal the eight optional native-property groups.

**Does Focus require a special Actor?** No. It adds controls to standard Actor Browser rows and edits native Actor/Component data.

## Visibility

**Does Hide in Editor also hide an Actor in game?** No. They are independent controls.

**Why can a Soloed Actor still be influenced by lighting or volumes?** Solo primarily filters rendered primitives. It is not guaranteed to suppress every lighting and volume contribution.

**Does Solo permanently change the other Actors?** No. Solo isolation itself does not rewrite all other Actors' hidden state.

## Protection

**Why can I still select a Selection-Locked Actor in the Outliner?** That is intentional, so you can inspect and unlock it. The protection is for normal viewport selection.

**What's the difference between Transform Lock and Edit Lock?** Transform Lock protects position, rotation and scale in supported editor paths. Edit Lock adds broader editing protection and includes transform protection.

**Can a Python script or third-party tool modify an Edit-Locked Actor?** Potentially, yes. Focus is intended to prevent accidental edits through supported native workflows; it is not an authorization system.

## Hierarchies

**Does locking a folder protect Actors created inside it tomorrow?** No. Folder operations apply to currently loaded members. Reapply after adding Actors.

**Why didn't an unloaded World Partition Actor change?** Focus bulk operations do not modify unloaded Actors. Load it first.

**What happens to a mixed state when I click?** A mixed or off toggle generally enables the setting on eligible targets; a fully on toggle disables it. Protection level cycling has its own sequence.

## Physics and properties

<!-- Media target: IMAGE | support-mixed-unavailable | docs/assets/media/support-mixed-unavailable.png -->
<figure class="doc-media" data-kind="image" data-media-id="support-mixed-unavailable">
  <img src="../../assets/media-placeholder.png" alt="Comparison of Mixed and Not Applicable states in Focus’s Outliner column" loading="lazy">
  <figcaption>Mixed indicates differing eligible values, while Not Applicable indicates that the target lacks an editable matching property.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Capture two Outliner rows side by side or stacked: a folder with eligible child Actors in differing states showing Mixed, and an Actor without an eligible component showing Not Applicable. Include each control tooltip to clarify the difference.</span></figcaption>
</figure>


**Why is a segment very dark or unclickable?** There may be no compatible registered component or instance-editable property. That's Not Applicable, not Off.

**Does Enable Gravity enable physics simulation?** No. Simulation and Gravity are separate native properties.

**Why doesn't Generate Hit Events produce callbacks?** Collision compatibility, physics setup and event bindings must also be valid.

**What does Forced LOD 0 mean?** Automatic LOD; 1 selects mesh LOD 0.

## Saving

**Does session-only apply to every native property?** No. Focus session-only controls are limited to Visual and Interaction. Other quick property groups save normally.

**Why is there still a Perforce checkout?** Another native edit may have dirtied the Actor or level package. Test on a clean package to isolate the cause.

**What does Restore Temporary Changes do?** It discards pending session-only Focus changes. It doesn't undo unrelated persistent changes or rewrite preference checkboxes.

## Need more detail?

Use the corresponding Design topic for examples and exact user behavior. The matching Technical topic explains property eligibility, storage and editor implementation.

---

For diagnostics, native editor paths and compatibility, [see Technical](TECHNICAL.md).
