# Network and Navigation — Design

**Focus v2.1.0 · Unreal Engine 5.8.x · Editor only**

## Network controls

Focus exposes three narrow native networking options rather than a complete replication editor.

- **Net Load on Client:** whether a placed Actor loads on clients as part of networked level loading. It does not activate replication.
- **Replicate Physics to Autonomous Proxy:** permits a compatible Primitive Component physics state to replicate to autonomous proxies when the gameplay setup supports it.
- **Static Mesh Replicate Movement:** controls the corresponding Static Mesh Actor movement-replication setting. It does not apply to every arbitrary Actor type.

For a networked test, first configure the Actor's broader replication, ownership, movement and physics settings, then use Focus for the specific property. Verify behavior in multi-instance PIE; a lit segment cannot prove a packet was sent.

## Navigation controls

| Control | Intended effect |
| --- | --- |
| Can Ever Affect Navigation | Eligible Actor Component may contribute navigation data |
| Dynamic Obstacle | Eligible Shape Component contributes as a dynamic obstacle where supported |
| Fill Collision Underneath for Navmesh | Fill navigable space beneath compatible primitive collision |
| Rasterize as Filled Convex Volume | Treat compatible primitive geometry as a filled convex region for navigation rasterization |

These affect inputs to navigation generation. They do not guarantee an immediate navmesh rebuild or a valid navigation path.

## A practical scene example

You have decorative collision that should not modify walking routes. Disable its navigation contribution. For a collision volume intended to carve a dynamic area, review Dynamic Obstacle. Verify the generated NavMesh visually in Unreal rather than assuming the toggle alone accomplished the intended navigation result.

## When controls appear dark

Not every Actor has compatible Primitive, Shape, or other Component types. Dark/unavailable is different from a valid disabled state.

---

For editable C++ property targets, transaction details and native constraints, [see Technical](TECHNICAL.md).
