# Installation and Compatibility — Design

**Focus 2.1.0 · Unreal Engine 5.8.x · Win64 · Editor only**

## Choose an installation method

<!-- Media target: IMAGE | installation-plugin-folder | docs/assets/media/installation-plugin-folder.png -->
<figure class="doc-media" data-kind="image" data-media-id="installation-plugin-folder">
  <img src="../../assets/media-placeholder.png" alt="Focus plugin folder installed in an Unreal Engine project" loading="lazy">
  <figcaption>A project installation places the Focus plugin folder directly beneath the project’s Plugins directory.<span class="media-capture-instructions"><strong>Capture instructions · Image:</strong> Show File Explorer at YourProject/Plugins/Focus with Focus.uplugin, Source, Resources and other actual top-level plugin files visible. Include enough of the address bar and parent folder to make the installation location unambiguous.</span></figcaption>
</figure>


The easiest option is a matching **Fab installation** or a **precompiled GitHub Release**. If you want to inspect or change the code, install the source instead.

### Precompiled release

1. Close Unreal Editor.
2. Download the release ZIP for your Unreal Engine and platform from [Focus Releases](https://github.com/mippi-the-dork/Focus/releases).
3. Extract the **Focus** folder to `YourProject/Plugins/Focus/`.
4. Verify that `Focus.uplugin` is directly under `Focus/`, not nested in a second Focus folder.
5. Open the project, enable Focus under **Edit → Plugins** if necessary, and restart if prompted.
6. Find the Focus column in the World Outliner.

### Source installation

With Unreal closed, clone the [Focus repository](https://github.com/mippi-the-dork/Focus) into `YourProject/Plugins/Focus`. Generate your project's Visual Studio files if required and build the Editor target with the UE C++ toolchain.

## Updating

For a downloaded release, replace the *entire* old plugin directory while the editor is closed. For a Git checkout, pull the desired version, rebuild, and restart. Do not mix source files from different releases.

## Troubleshooting first launch

Check that you installed into the right project, enabled the plugin, used a supported engine/platform, and are looking at the standard World Outliner. If the column was manually hidden, restore it through the Outliner column menu.

---

## Technical detail

[Read the Technical page](TECHNICAL.md) for engine integration, persistence and implementation boundaries.
