# Simulation and Physics Events — Design

**Focus v2.1.0 | Unreal Engine 5.8.x | Windows 64-bit | Editor only**

Simulation, gravity and event flags.

## Verified behavior

Properties use registered eligible components and native setters when applicable.

## Usage and workflows

## Simulation

The **Simulation** miniwidget exposes six physics-related controls in a radial arrangement. Enable it from the Focus header menu when working with physics Actors, destructible scene elements, or other components that may react to forces. Each triangular piece is independent, so changing gravity does not also change physics simulation, for example.

### Physics simulation and gravity

**Simulate Physics** allows an eligible primitive component to be driven by rigid-body physics instead of relying solely on its authored transform. A component needs appropriate collision and mobility settings for simulation to behave as expected. If a component cannot be edited through Focus, its segment is shown as Not Applicable; Focus does not silently change those prerequisites.

**Enable Gravity** determines whether world gravity influences an eligible component while it is simulating physics. You can enable gravity without enabling simulation, but gravity will not by itself make a non-simulating component fall.

For example, when preparing a collection of movable props, use the Simulate Physics segment to enable physics on eligible components and the Gravity segment to decide which props should fall under world gravity.

### Radial impulses, forces, and damage

**Ignore Radial Impulse** makes a component ignore supported instantaneous radial impulses, such as a burst of outward force. **Ignore Radial Force** instead applies to ongoing radial force effects. These are separate settings: disabling one type of response does not disable the other.

**Apply Impulse on Damage** permits supported damage events to impart a physical impulse. This option does not automatically configure damage handling, collision, or physics simulation.

A useful workflow is to select the Actors surrounding an explosion effect, inspect their radial impulse and force settings, and change only the response you want to suppress.

### Async physics ticking

**Async Physics Tick Enabled** is an Actor-level option. It controls whether Unreal may invoke the Actor's asynchronous physics tick where supported; it is not an alternative switch for Simulate Physics or the regular Actor tick.

The widget may therefore show different applicability for this segment than for the component-level physics segments. Hover each piece to see which targets Focus can actually edit.

---

## Physics Events

The **Physics Events** miniwidget has three horizontally stacked pills. From top to bottom, they control **Generate Hit Events**, **Generate Overlap Events**, and **Generate Wake Events**. Use it when checking why collision or simulation events are or are not being produced by a component.

### Hit events versus overlap events

**Generate Hit Events** controls rigid-body collision hit notifications for eligible primitive components. A hit event still depends on collision setup and the type of contact involved; this checkbox alone does not guarantee that every collision produces an event.

**Generate Overlap Events** controls overlap notification generation. Overlap behavior also depends on the other object, both collision configurations, and whether their responses permit overlaps. An overlap is not the same thing as a blocking hit.

For example, a blocking wall and a trigger volume typically require different event setups. The two upper pills let you inspect the intended event type without navigating between Details panels.

### Wake events

**Generate Wake Events**, the bottom pill, controls supported physics wake and sleep notifications. These are associated with physics-body activity, rather than ordinary actor visibility or overlap changes. An applicable physics simulation setup is needed for meaningful wake/sleep events.

Each pill reflects its own value. A mixed selection can show Mixed for overlap events while hit and wake events remain uniformly enabled or disabled. Enabling event generation in Focus does not automatically create Blueprint or C++ handlers for the resulting events.

---

## Lighting

## Expected limits

Focus is an Editor-only workflow tool. Folder and selected-row changes affect eligible loaded Actors and are not automatically inherited by Actors added later. Supported native protections do not prevent arbitrary external tools or scripts.

## Related source

- [Main editor module](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusModule.cpp)
- [Native quick properties](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusQuickProperties.cpp)
- [Editor preferences](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusSettings.h)
- [Interaction guards](https://github.com/mippi-the-dork/Focus/blob/main/Source/Focus/Private/FocusInteractionGuard.h)
- [Full README](https://github.com/mippi-the-dork/Focus/blob/main/README.md)
