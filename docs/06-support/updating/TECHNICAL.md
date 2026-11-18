# Updates and Support — Technical

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Releases, versions, reports and contributions.

## Verified behavior

Releases replace the complete plugin folder. Source installs can pull and rebuild; supported configuration is UE 5.8.x Win64.

## Native behavior and original reference

# Updating Focus

## GitHub Release Installation

When updating a manually installed release:

1. Close Unreal Editor.
    
2. Remove the existing `Plugins/Focus` folder.
    
3. Extract the new Focus release into the `Plugins` directory.
    
4. Reopen the project.
    

Replacing the complete plugin folder is recommended rather than copying individual files over an older version.

## Git Source Installation

If you cloned the repository using Git:

```bash
cd YourProject/Plugins/Focus
git pull
```

Rebuild the project if the source has changed.

---

# Compatibility

Focus supports the following target configuration:

|||
|---|---|
|**Focus Version**|2.1.0|
|**Unreal Engine**|5.8.x|

|**Platform**|Windows 64-bit|
|**Plugin Type**|Editor|
|**Runtime Dependency**|None|
|**Packaged Game Impact**|None|

Focus is currently configured as a Win64 editor plugin.

Compatibility with additional Unreal Engine versions or platforms should not be assumed unless explicitly listed in a release.

---

# How Focus Works

---

# Reporting Bugs

If you encounter a problem, please open an issue:

[https://github.com/mippi-the-dork/Focus/issues](https://github.com/mippi-the-dork/Focus/issues)

When reporting a bug, include:

- Focus version
    
- Unreal Engine version
    
- Windows version
    
- Whether Focus was installed from Fab, a GitHub Release, or source
    
- The Focus control involved
    
- Whether the target was an Actor, Folder, Group, or multi-selection
    
- Whether World Partition was involved
    
- Steps to reproduce the problem
    
- Screenshots or video when relevant
    
- Any relevant Unreal Editor log output
    

Clear reproduction steps make issues much easier to diagnose.

---

# Feature Requests

Suggestions and feature requests are welcome through GitHub Issues.

When proposing a feature, describe the workflow problem you're trying to solve rather than only the implementation you would like to see.

That makes it easier to determine whether the feature belongs in Focus and whether there may be a simpler solution.

---

# Contributions

Pull requests are welcome.

If you're considering a significant change, opening an Issue first is recommended so the direction can be discussed before substantial work is done.

Focus concentrates frequently used Actor and Component settings into optional Outliner categories, while preserving the compact Visual and Interaction defaults.

---

# License

Focus is distributed under the **MIT License**.

See the [repository license](https://github.com/mippi-the-dork/Focus/blob/main/LICENSE) for details.

---

# About

Focus is an Unreal Engine editor utility created by **Mippi the Dork**.

The plugin was built around a simple idea:

## Verification boundary

These details are grounded in current v2.1.0 C++ source and the public README. Editor-specific combinations, such as external Actor saving and nonstandard editing modes, require validation in the target Unreal project.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
