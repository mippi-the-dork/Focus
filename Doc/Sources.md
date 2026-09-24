# Focus sources and implementation notes

API and engine-source references for Focus's Outliner controls, visibility isolation, selection and edit protection, shared settings, and editor lifecycle. These notes describe the implementation targeting Unreal Engine 5.8.2.

## Public API references

| Reference | Use in Focus |
| --- | --- |
| [FSceneOutlinerModule](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Editor/SceneOutliner/FSceneOutlinerModule) | `OnCreateActorBrowserColumns()` adds Focus to regular actor-browser instances. The callback changes the instance's column map rather than replacing the global default-column registration. |
| [IDetailsView](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Editor/PropertyEditor/IDetailsView) | Property read-only and property-editing-enabled delegates protect standard Details values while preserving navigation. |
| [FSceneView](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/Engine/FSceneView) | `ShowOnlyPrimitives` supplies editor-view isolation for Solo. |
| [FScopedTransaction](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Editor/UnrealEd/FScopedTransaction) | Groups a Focus control action into one Undo/Redo operation, including bulk actions. |
| [FCommandChange](https://dev.epicgames.com/documentation/unreal-engine/API/Runtime/Core/FCommandChange) | Explicit before/after changes restore Focus metadata and live state during Undo and Redo. |
| [FMetaData](https://dev.epicgames.com/documentation/en-us/unreal-engine/API/Runtime/CoreUObject/FMetaData) | Per-object key/value storage in the owning package for shared Focus settings. |

Other public interfaces used by the implementation include `ISceneOutlinerColumn`, `FActorTreeItem`, `FFolderTreeItem`, `UEdMode`, `FUICommandList`, `IInputProcessor`, `FSceneViewExtensionBase`, `FPropertyEditorModule`, `SSubobjectEditor`, and `FActorPickerModeModule`.

The module dependencies are Core, CoreUObject, Engine, Slate, SlateCore, InputCore, UnrealEd, EditorFramework, LevelEditor, SceneOutliner, PropertyEditor, RenderCore, Projects, SubobjectEditor, and ActorPickerMode. The module uses explicit/shared PCHs. It has no Motion Design, Surface or Actor Locker dependency.

## Engine source references

Paths below are relative to `Engine/Source`. They identify engine behavior relevant to Focus; these files do not need to be modified or distributed with the plugin.

| Source | Relevant behavior |
| --- | --- |
| `Editor/SceneOutliner/Public/SceneOutlinerModule.h` and `Private/SceneOutlinerModule.cpp` | Actor-browser column creation and column registration boundaries. |
| `Editor/SceneOutliner/Private/ActorHierarchy.cpp` and `ActorBrowsingMode.cpp` | Loaded actor/folder hierarchy, actor-added notifications and standard actor-browser interactions. |
| `Editor/UnrealEd/Private/EditorModeManager.cpp` and `EditorViewportClient.cpp` | Editor-mode activation, compatibility and standard viewport transform routing. |
| `Editor/PropertyEditor/Public/IDetailsView.h` and `Private/DetailCategoryBuilderImpl.cpp` | Details editing delegates and category-level enabled state inherited by custom widgets. |
| `Editor/DetailCustomizations/Private/ComponentTransformDetails.cpp` | Custom Transform and Mobility controls, which require more than a property-handle read-only gate. |
| `Editor/PropertyEditor/Private/MaterialList.cpp` | Custom material editing widgets relevant to Edit Lock coverage. |
| `Editor/LevelEditor/Private/SActorDetails.cpp` and `LevelEditorActions.cpp` | Actor Details hosts, command bindings and native attachment operations. |
| `Editor/SubobjectEditor/Private/SSubobjectEditor.cpp` and `SSubobjectInstanceEditor.cpp` | Component-tree commands, inline editing and component hierarchy interactions. |
| `Editor/ActorPickerMode/Private/ActorPickerMode.cpp` | Interactive actor picking used by attachment workflows. |
| `Editor/UnrealEd/Private/EditorTransaction.cpp` | `FTransaction::StoreUndo()` records custom changes and associates them with an object's package. |
| `Runtime/Engine/Classes/GameFramework/Actor.h` and `Private/ActorEditor.cpp` | Native movement-lock flags, editor visibility and actor editor behavior. |
| `Runtime/Engine/Private/ActorConstruction.cpp` and `Private/Components/SceneComponent.cpp` | Construction-script execution, component reconstruction and transform propagation. |
| `Runtime/CoreUObject/Public/UObject/MetaData.h` and `Private/UObject/MetaData.cpp` | Metadata lookup/mutation, object-path identities and metadata movement helpers. |
| `Runtime/CoreUObject/Public/UObject/Package.h` and `Private/UObject/Package.cpp` | Package metadata access and package ownership. |

Epic's Motion Design/Avalanche Outliner and viewport sources were also consulted as behavior and UI references. Focus integrates with the standard Level Editor Outliner and does not load Avalanche modules.

## Implementation

### Add controls to the regular Outliner

`FFocusColumn` implements `ISceneOutlinerColumn`. An actor-browser creation callback adds the Focus column and removes the standard visibility gutter from that browser's column map. Actor and folder rows receive the controls; unsupported row types receive no Focus widgets.

The visible column label is **Focus**. The internal column ID remains `Focus.PrototypeControls` to preserve existing column preferences. This identifier is not the displayed label.

Rows retain weak references and periodically refresh aggregate state. Slate attributes read those aggregates. This is not an asynchronous metrics system or a guarantee of fixed frame time.

### Resolve actor, folder and group scope

Actions resolve their scope from loaded actors rather than only the currently expanded or filtered tree rows. Folder matching includes the folder root object and nested paths. Attached descendants and native group/subgroup members are traversed with a visited set to avoid duplicate changes.

If the clicked row is selected, the action combines the scope of all selected rows. Mixed indicators summarize each row's own scope. Folder and parent operations write settings to the affected actors; there is no persistent inheritance rule for actors added later. Unloaded actors are outside bulk-action scope.

### Separate visibility from Solo

Hide in Editor uses the actor's temporary editor-hidden flag. The base `AActor` setter is called explicitly after scope resolution so group overrides do not recursively apply the action a second time. Hidden in Game writes the native actor property through its normal transaction path and skips edit-locked actors.

`FFocusViewExtension` collects the primitive-component IDs of soloed actors and supplies a `ShowOnlyPrimitives` set for Editor-world views. If another extension has already restricted the view, Focus intersects with that restriction. Solo does not rewrite Hidden in Game or editor-hidden flags.

Explicit editor hiding still applies. Primitive filtering does not guarantee isolation of lights, volumes or other non-primitive contributions.

### Keep selection and editing protection independent

`UFocusEditorMode` rejects viewport selection of selection-locked actors. Its current Outliner exception checks the Slate widget path beneath the pointer. This permits clicking Outliner rows but is not a complete keyboard/focus-based selection policy or click-through selection system.

The protection padlock cycles Unlocked, Transform Lock and Edit Lock. Edit Lock includes transform protection. Selection Lock is a separate flag.

Transform protection combines the native movement-lock flag, standard viewport input handling, Details read-only rules and `FFocusTransformGuard`. The guard keeps movement-scoped world-transform snapshots and responds to movement, property and transaction notifications. This supports protection when parent/group operations would otherwise move locked descendants.

Blueprint execution is checked through the tracked script-frame chain. Blueprint-authored transform changes are accepted, and restoration avoids running through component reconstruction. Undo/Redo resets temporary movement snapshots rather than treating history replay as a new forbidden edit.

### Protect editing without disabling inspection

Focus composes with existing Details read-only and editing-enabled delegates. It checks the actual actor/component owners of property rows, including external component properties. The editing-enabled gate covers custom controls such as Mobility and Materials that can bypass ordinary property read-only checks.

The entire Details widget is not disabled. Search, category expansion, favorites and supported navigation remain available.

`FFocusCommandGuard` wraps supported Level Editor and component-tree commands. `FFocusInteractionGuard` handles supported inline editing, Add Component controls, drag/drop and interactive attachment picking. Attachment destinations are rechecked when an operation commits.

These are editor workflow protections, not a universal veto on object mutation. Custom tools, specialized widgets, scripts and direct engine calls can bypass the guarded routes.

### Save shared settings with actor packages

Focus stores per-actor metadata under `Focus.SharedState.V1`. Its five-character payload records Selection Lock, Transform Lock, Edit Lock, Solo and Hide in Editor in that order. Each character must be `0` or `1`; invalid payloads are ignored.

Storage uses `Actor->GetPackage()`, including external actor packages, rather than assuming every actor belongs to the map's outermost package. The package is marked dirty after changes. **Save All** makes the changes persistent and available for normal source-control submission. Hidden in Game remains a native saved property.

In the inspected UE 5.8 source, `FMetaData` is not a transactional UObject and `SetValue()` does not record Undo or automatically mark the package dirty. Focus therefore records explicit `FCommandChange` snapshots through `GUndo->StoreUndo()`. Snapshots preserve both live state and the original metadata value or absence of a value.

Saved state is restored when the editor world opens and as actors/levels load. Actor-added restoration is deferred to allow initialization. An object-replacement callback transfers Focus's metadata for Blueprint actor replacement when available. Weak actor references are pruned as objects disappear.

Focus does not automatically submit source-control changes or impose user permissions. Shared locks require recipients to have Focus enabled. Cross-checkout source-control transfer remains unverified; level reload and editor restart persistence have been exercised during development.

### Manage editor lifetime and artwork

The Editor module loads at `PostEngineInit` and waits for the Level Editor and editor-mode registry before activation. It preserves the normal default-mode setup so its selection gate does not replace the transform-widget workflow.

Shutdown removes registered callbacks, the input preprocessor and ticker. Wrapped delegates and commands retain prior bindings and are restored only where Focus still owns the installed binding. Dynamic module reloading is disabled.

Solo and selection icons use the supplied SVG resources through `FSlateVectorImageBrush`; shared brush ownership keeps pointers alive while row widgets use them. The protection padlock uses native editor style brushes. `FSlateIcon` is included through `Textures/SlateIcon.h`.
