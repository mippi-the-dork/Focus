# Solo — Technical

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Temporary editor viewport isolation.

## Verified behavior

Solo uses the FFocusViewExtension per-view ShowOnlyPrimitives list, not mass rewrites of Actor Hidden flags. Lights and volumes may still contribute.

## What Solo does not isolate

Solo is per editor view and filters rendered primitive scene IDs. It does not guarantee removal of light influence, volumes or other non-primitive contributions. It preserves restrictions from other view extensions by intersecting its allowed set with any preexisting ShowOnlyPrimitives mask.
## Native behavior and original reference

## Solo

Solo isolates part of the level inside the editor viewport.

You can Solo:

- An Actor
    
- An Actor hierarchy
    
- A Folder
    
- Multiple Actors
    
- Multiple Folders
    
- Groups
    

Multiple Solo targets can be active simultaneously.

When Solo is cleared, normal editor visibility returns.

Solo does not change Hidden in Game.

Use the **Solo persistence** checkbox to choose between saving Focus's Solo choices and keeping them temporary for the editor session. A session-only active Solo icon uses **blue** rather than white; Mixed uses **orange** rather than yellow. **Restore Temporary Changes** discards tracked temporary Solo changes along with other temporary Focus edits.

![Using Solo with Focus](Doc/Images/Focus-Solo.gif)

### Solo and Existing Visibility

Actors already hidden using **Hide in Editor** remain hidden while Solo is active.

Solo should be treated as a temporary viewport isolation tool rather than a replacement for normal level visibility.

Lights, volumes, and other scene systems may continue influencing the viewport even when their Actor is outside the current Solo set.

---

## Selection Lock

## Verification boundary

These details are grounded in current v2.1.0 C++ source and the public README. Editor-specific combinations, such as external Actor saving and nonstandard editing modes, require validation in the target Unreal project.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
