# Installation and Setup — Technical

> **Focus v2.1.0** · Unreal Engine 5.8.x · Win64 · Editor Only

Fab, GitHub releases, source builds and enabling the plugin.

This reference describes the shipped editor-facing behavior and native property boundaries documented in the Focus repository. It is not an assertion that the implementation has been independently audited line-by-line.

## GitHub Source

Developers who want the latest source or want to modify Focus can clone the repository directly.

### Requirements

Building Focus from source requires a working Unreal Engine C++ development environment.

For Windows this generally means:

- Unreal Engine 5.8.x
    
- Visual Studio with the appropriate C++ workloads
    
- A project capable of compiling C++ plugins
    

### Clone the Repository

Close Unreal Editor and navigate to your project's `Plugins` directory.

```bash
cd YourProject/Plugins
git clone https://github.com/mippi-the-dork/Focus.git
```

Your project should now contain:

```text
YourProject/Plugins/Focus/
```

### Generate Project Files

If necessary:

1. Right-click your `.uproject`.
    
2. Select **Generate Visual Studio project files**.
    

Then open the generated solution and build your project's Editor target.

For example:

```text
YourProjectEditor
Win64
Development Editor
```

Launch the project after compilation completes.

---

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

---

## Related information

- [Focus source and complete README](https://github.com/mippi-the-dork/Focus#readme)
- [Focus issue tracker](https://github.com/mippi-the-dork/Focus/issues)
- [Installation](../01/installation/DESIGN.md)


