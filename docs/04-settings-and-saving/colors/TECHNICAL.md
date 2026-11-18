# Control State Colors and Reading Signals — Technical

**Focus v2.1.0 · Source-reviewed implementation**

## Settings ownership

`UFocusSettings` inherits `UDeveloperSettings` with `config=EditorPerProjectUserSettings` and `defaultconfig`. It appears in the Editor/Plugins preferences container. `EnabledColor`, `DisabledColor`, `MixedColor`, `NotApplicableColor`, and `AlertColor` are the general state palette.

Session-only equivalents are `SessionEnabledColor`, `SessionDisabledColor`, `SessionMixedColor`, `SessionNotApplicableColor`, and `SessionAlertColor`.

## Default values

The general enabled color is white, disabled is displayed sRGB **#616161**, Not Applicable is **#242424**, Mixed is yellow, and Alert is red. Session-only enabled is **#2196F3**, Mixed **#FF9800**, Alert **#FF4DA6**; default disabled and unavailable remain #616161 and #242424.

## Choosing the palette

`Focus::EnabledTint`, `DisabledTint`, `MixedTint` and `AlertTint` select the standard or session palette using `Focus::IsPersistent(EControl)`. Native quick-property categories are ordinarily persistent and use the general palette.

## sRGB/linear migrations

Slate paint colors are linear values. `FFocusModule` includes one-time color-default migration flags (`bMigratedSlateStateColorDefaults` and `bMigratedDisabledColorDefaultTo616161`) to correct older shipped gray defaults without overwriting user-customized colors.

## Rendering is not state authority

The tint expresses row state and persistence policy. Native eligibility, property data, and saved metadata are determined independently. Altering a palette never invokes an Actor or Component edit.

---

## Relevant source files

- [FocusModule.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [FocusSettings.h](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [FocusSettings.cpp](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.cpp)

For editor workflows and examples, [see Design](DESIGN.md).

**QA status:** Source verification completed; exhaustive real-editor package-save and source-control testing not claimed.
