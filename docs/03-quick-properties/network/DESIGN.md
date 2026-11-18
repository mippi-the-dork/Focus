# Network and Navigation — Design

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Networking options and navigation contribution.

## Verified behavior

These toggle supported native properties, they are not a replacement networking or navigation implementation.

## Usage and workflows

## Network

The **Network** miniwidget contains three separately clickable angular pieces arranged around a center. These controls relate to different parts of Unreal's networking behavior and should not be treated as a single Replication On/Off switch.

### Client loading

**Net Load on Client**, the upper piece, determines whether a placed Actor is eligible to load on clients as part of the relevant networked level-loading behavior. **It does not, by itself, enable Actor replication.** A level-placed Actor loading on a client and a server replicating its state are separate concerns.

### Physics and movement replication

**Replicate Physics to Autonomous Proxy**, the lower-left piece, controls the corresponding physics replication setting on supported primitive components. Its effect requires the appropriate physics and network configuration; it cannot make an otherwise unsupported component replicate its physics automatically.

**Static Mesh Replicate Movement**, the lower-right piece, exposes the corresponding movement-replication setting for supported Static Mesh Actors. It is not a universal toggle for movement replication on every Actor type.

For example, when reviewing networked moving props, check the movement piece on eligible Static Mesh Actors and the physics piece on relevant primitives. A dark Not Applicable piece often indicates a different Actor or component class, not that networking has been globally turned off.

---

## Navigation

The **Navigation** miniwidget uses a diamond of **four circular controls**. Its compass-like arrangement provides four distinct properties, with larger invisible click regions than the dots themselves.

### Navigation contribution and dynamic obstacles

The **top circle**, **Can Ever Affect Navigation**, controls whether an eligible Actor component may contribute to navigation data. Navigation geometry, collision settings, and rebuild behavior still determine the outcome.

The **right circle**, **Dynamic Obstacle**, marks supported shape components as dynamic navigation obstacles. This may not be available on ordinary components that do not expose the corresponding property.

For example, you can inspect whether a placed obstacle component is allowed to influence navigation before investigating a missing or unexpected navmesh cutout.

### Navmesh collision treatment

The **bottom circle**, **Fill Collision Underneath for Navmesh**, controls how supported primitive collision fills the navigation representation beneath it, subject to the navmesh configuration.

The **left circle**, **Rasterize as Filled Convex Volume**, uses filled convex-volume rasterization for eligible primitive collision during navigation generation.

The four controls affect the input or behavior of navigation generation; they do not replace navmesh volumes, agent settings, collision setup, or navigation rebuilding. When a change does not immediately alter navigation, check those other systems rather than assuming the Focus toggle failed.

---

## Mobility

## Expected limits

Focus is an Editor-only workflow tool. Folder and selected-row changes affect eligible loaded Actors and are not automatically inherited by Actors added later. Supported native protections do not prevent arbitrary external tools or scripts.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
