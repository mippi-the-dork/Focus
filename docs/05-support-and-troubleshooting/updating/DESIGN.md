# Updating Focus and Getting Support — Design

**Focus v2.1.0 · User guide and workflow reference**

## Choose the right update route

### From a precompiled release

<!-- Media target: IMAGE | support-plugin-update | docs/assets/media/support-plugin-update.png -->
<figure class="doc-media" data-kind="image" data-media-id="support-plugin-update">
  <img src="../../assets/media-placeholder.png" alt="Focus release folder located inside an Unreal Engine project’s Plugins directory" loading="lazy">
  <figcaption>Replacing the full Focus folder while Unreal is closed avoids mixing files from incompatible plugin releases.</figcaption>
</figure>


1. Close the Unreal Editor.
2. Download the new compatible ZIP from [Focus Releases](https://github.com/mippi-the-dork/Focus/releases).
3. Remove the old `YourProject/Plugins/Focus/` folder.
4. Extract the new package as `YourProject/Plugins/Focus/`.
5. Restart the project, confirm the plugin version, and test your ordinary Outliner controls.

Replacing the complete plugin folder avoids leaving orphaned C++ or Resource files from a previous release.

### From Git source

If you cloned Focus into the project's Plugins directory, pull the version or tag you intend to use and rebuild the Editor target. Avoid mixing source checkout and precompiled release files without checking exactly what is installed.

## Compatibility

Focus v2.1.0 currently targets **Unreal Engine 5.8.x, Windows 64-bit**, and is an **Editor** plugin. Additional engine versions and platforms are not implied by this manual.

## Report a problem

Open an issue at [Focus Issues](https://github.com/mippi-the-dork/Focus/issues) and include: Focus version; Unreal version; Windows version; installation source; target Actor/Component class; Actor/folder/group/multi-selection scope; World Partition status; persistence preference; steps; screenshots/video; and relevant Output Log lines.

## Suggest a feature

Describe the underlying workflow problem and expected outcome, rather than only naming an implementation. That helps distinguish a native Unreal feature that could simply be exposed from a capability requiring new Focus behavior.

## Contributing

Pull requests are welcome. For significant new behavior, discuss the intended scope first so it remains a compact, native World Outliner tool.

## License

Focus's repository uses the **MIT License**. Refer to the repository's LICENSE for actual terms.

---

For diagnostics, native editor paths and compatibility, [see Technical](TECHNICAL.md).
