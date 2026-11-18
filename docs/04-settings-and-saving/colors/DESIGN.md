# State Colors — Design

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Enabled, disabled, mixed and unavailable states.

## Verified behavior

Settings use distinct colors for persistent and session-only changes. Unavailable is not the same as disabled.

## Usage and workflows

### State colors

Focus uses two independently configurable color palettes in **Edit > Editor Preferences > Plugins > Focus**. **Control State Colors** apply to persistent controls and to the eight native quick-property categories. **Session-Only State Colors** apply to Visual and Interaction controls when their individual persistence options are turned off.

| State | Persistent default | Session-only default | Meaning |
|---|---|---|---|
| Enabled | White | Blue (`#2196F3`) | A control is active or a supported option is selected |
| Disabled | `#616161` | `#616161` | A control is inactive or an option is not selected |
| Mixed | Yellow | Orange (`#FF9800`) | Eligible targets have different values |
| Not Applicable | `#242424` | `#242424` | The control cannot be edited for the current target |
| Alert | Red | Pink (`#FF4DA6`) | A warning or strong protection state |

Each of the ten colors is independently customizable in Editor Preferences. The default grays match between palettes, but they can be changed separately. The color settings affect presentation only, not Actor or Component property values.

For **Hide in Editor**, an active hidden dot uses the **Enabled** color (white or blue), not the Mixed color. A genuinely Mixed selection uses yellow or orange. **Hidden in Game** uses the **Alert** color (red or pink) when active; this is distinct from the Mixed color. Tooltips identify the current persistence mode even where the two palettes share the same gray.

### Visual and Interaction persistence

## Expected limits

Focus is an Editor-only workflow tool. Folder and selected-row changes affect eligible loaded Actors and are not automatically inherited by Actors added later. Supported native protections do not prevent arbitrary external tools or scripts.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
