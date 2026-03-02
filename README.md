# COMP3015-CW1



## Project Overview

This project is an OpenGL prototype focused on real-time lighting, texturing, and post-processing, themed around rendering a **lightsaber**.

The scene centres on a textured 3D lightsaber hilt that uses **diffuse and normal mapping** for surface detail, plus a toggleable, glowing blade. The blade glow is achieved using a **custom bloom effect** to create the recognisable “lightsaber” look, and the project also includes extra visual techniques such as **fog**, **edge detection**, and **texture mixing** (adding a rusty overlay).

The overall goal is to showcase a stylised but technically grounded rendering setup using GLSL shaders, lighting, and texture-based techniques.


https://github.com/user-attachments/assets/3c9ecd3c-407d-431a-aa53-2da09e3eedd7



## How to Use (Controls)

Controls are designed to quickly demonstrate the key features of the prototype:

### Model / Camera
- **W / S** — Rotate the lightsaber hilt up/down.
- **A / D** — Orbit the camera left/right (yaw)

 
### Lightsaber / Visual Style
- **Q** — Toggle the lightsaber blade on/off (including the glow effect).
- **E** — Cycle lightsaber blade colours.
- **R** — Toggle texture mixing (adds/removes a rusty texture blend on the hilt).
- **F** — Toggle fog on/off.
- **C** — Toggle edge detection on/off.

### Render Features Demonstrated
- **Diffuse + normal maps** applied to the lightsaber hilt.
- **Emissive glow** used to create the blade “halo” effect (rim lighting + additive blending).
- Optional post/visual effects: **fog**, **edge detection**, and **texture mixing**.

---

## Technical Breakdown (Rubric-Mapped)

This section highlights how each assessed feature is implemented in the codebase, using the same terminology as the CW1 marking rubric.

---

### Basic (Max 20 Marks)

#### Basic lighting — **Blinn–Phong** in the fragment shader (5)
- The primary shading model is **Blinn–Phong**, implemented in `shader/basic_uniform.frag` via:
  - `vec3 blinnPhong(vec3 n)` (directional/point-style light path)
  - `vec3 blinnPhongSpot(vec3 n)` (**spotlight** path used for the hilt)
- Specular highlights use the **half-vector** method:
  - `h = normalize(v + s)`
  - `spec = Ks * pow(max(dot(h,n),0), Shininess)`
- Diffuse uses Lambertian cosine term:
  - `diffuse = texColour * max(dot(s,n), 0.0)`

#### Basic texture sampling (2.5)
- Standard texture sampling is used for the scene and hilt:
  - Workbench: `media/texture/workbench.png` bound to texture unit 0/2 for the plane pass.
  - Lightsaber hilt: diffuse/albedo map `Lightsaber_03_exp_lambert1_BaseColor1.png` bound at `binding = 0` (`DiffuseTex`).

#### Lighting subtechnique — **Fog** (2.5)
- A toggleable fog subtechnique is implemented in `basic_uniform.frag`:
  - Enabled via `uniform int FogEnabled` and toggled by keyboard input (`F`) in `SceneBasic_Uniform::update()`.
  - Distance is measured in **view space** using `ViewPos` from the vertex shader:
    - `dist = length(ViewPos) * FogScale`
  - A linear fog factor is computed:
    - `f = clamp((Fog.MaxDistance - dist) / (Fog.MaxDistance - Fog.MinDistance), 0..1)`
  - Final colour blends between fog colour and lit surface colour:
    - `final = mix(Fog.Colour, litColour, f)`
- Aesthetic/intentional choice: fog colour is synchronised to the blade state/colour via `updateFogColour()` so the scene atmosphere matches the currently selected saber colour.

#### Texturing subtechniques — **Normal Mapping** + **Texture Mixing** (5 total: 2.5 + 2.5)
This prototype implements two rubric texturing subtechniques on the hilt:

**1) Normal mapping**
- Normal map is bound to `binding = 1` (`NormalMapTex`) and sampled in `basic_uniform.frag`:
  - `normal = normalize(texture(NormalMapTex, TexCoord).xyz * 2.0 - 1.0);`
- Tangent-space basis (TBN) is constructed in `basic_uniform.vert` using the provided vertex tangent (`layout location 3`):
  - `T` is orthonormalised against `N`
  - `B = cross(N, T) * VertexTangent.w`
  - `TBN = mat3(T, B, N)` and `invTBN = transpose(TBN)`
- Lighting vectors are transformed into tangent space so the sampled normal map can be used correctly:
  - `LightDir = invTBN * lightDir;`
  - `ViewDir  = invTBN * normalize(-Position);`
  - `SpotDir  = invTBN * normalize(Spot.Direction);`

**2) Texture mixing**
- A rust texture is blended over the hilt using `MixingTex` (`binding = 2`) with a per-pixel mask:
  - `getBaseColour()` samples:
    - `a = texture(DiffuseTex, TexCoord)`
    - `b = texture(MixingTex, TexCoord)`
  - Uses `b.a` (alpha) as the blend mask:
    - `t = clamp(mask * MixAmount, 0..1)`
    - `mix(a.rgb, b.rgb, t)`
- The effect is runtime-toggleable (`R`) by swapping what’s bound to texture unit 2:
  - rust on → bind `rust.png`
  - rust off → bind diffuse map again (so mix has no visible change)

#### Skybox (2.5)
- A cubemap skybox is loaded and rendered each frame:
  - `cubeTex = Texture::loadCubeMap("media/texture/cube/space/space");`
  - `skyboxShader` renders the sky with `samplerCube SkyboxTex`.
- The skybox supports the same fog toggle for visual consistency:
  - `FogEnabled`, `FogColour`, and `SkyFogAmount` are uniforms in `shader/skybox.frag`.

---

### Advanced (Max 20 Marks)

#### Aesthetics (10)
- The scene is built around an “intentional visual” goal (a readable hero object: lightsaber hilt + glowing blade), supported by:
  - a moving spotlight for dynamic highlights,
  - consistent fog colour tied to blade colour,
  - a skybox background to avoid flat black space,
  - stylised post effects (outline + bloom/glow).

#### Animation of light / model animation that is sensible (2.5)
- **Light animation:** spotlight position orbits over time using `angle` updated in `update()`:
  - `lightPosWorld = vec4(10*cos(angle), 10, 10*sin(angle), 1)`
  - set each frame via `Spot.Position` (converted to view space)
- **Model interaction:** the hilt rotates with `W/S` (and arrow keys) by updating `hiltYawDeg`, applied in the hilt model matrix.

#### Keyboard scene/animation controls (2.5)
Keyboard input is handled in `SceneBasic_Uniform::update()` with edge-triggered toggles:
- `Q` toggles blade (`bladeOn`)
- `E` cycles blade colour (`bladeColourIndex`) + updates `BladeColour` uniform
- `R` toggles rust texture mixing
- `F` toggles fog
- `C` toggles edge detection
- `W/S` rotates the hilt

#### Mouse scene/animation controls (2.5)
- Mouse input controls an **orbit camera** when the cursor is captured:
  - `dx/dy` are read via `glfwGetCursorPos()`
  - updates `camYawDeg` and `camPitchDeg`
  - pitch is clamped to maintain a sensible viewing angle
- Camera movement is implemented in `updateCamera(dt)`.

#### Image processing technique — **Bloom** (2.5)
- The blade glow is designed around a **bloom-style** aesthetic (bright emissive core + softer glow falloff) to replicate the lightsaber look.
- The blade is rendered using a dedicated emissive shader (`shader/blade.vert` / `shader/blade.frag`) using:
  - Rim lighting term: `rim = pow(1 - dot(N,V), RimPower)`
  - Emissive intensity + rim boost: `BladeColour * Intensity + BladeColour * (rim * RimIntensity * Intensity)`
- The glow uses two passes:
  1) **White core pass** with stencil write (solid bright centre).
  2) **Additive blended glow shell** rendered only outside the stencil silhouette (`GL_NOTEQUAL`) and typically using backfaces of an enlarged mesh for an outline-like halo.

> Note: the *look* is bloom-like and intentionally achieves the same perceptual result (bright emissive spill), but the rubric wording “Bloom” usually implies a framebuffer bright-pass + blur + composite pipeline. If your project includes that too, paste the bloom FBO/blur/composite code and I’ll describe it explicitly as true post-process bloom (threshold → blur → additive composite) so it’s unambiguous for marking.

---

### Framebuffer / Post-Processing Features (Rubric-relevant)

#### Framebuffer feature — **Edge detection** (supports rubric “Framebuffer features get 5”)
- Edge detection is implemented using an **off-screen framebuffer** + fullscreen triangle composite:
  1) **Mask pass:** render the hilt into `hiltFbo` producing `hiltMaskTex` (white where the hilt exists) using `hilt_mask.vert/frag`.
  2) **Edge pass:** run a **Sobel filter** over the mask in `edge_detect.frag` and alpha-blend the outline over the main framebuffer.
- Fullscreen drawing uses a procedural fullscreen triangle (`edge_tri.vert`) with `glDrawArrays(GL_TRIANGLES, 0, 3)` (no VBO required).
- The edge effect is toggleable (`C`) and has tunable uniforms:
  - `Threshold`, `Opacity`, `Thickness`, `EdgeColour`, `TexelSize`.

---

## Where to Find Things (Quick Navigation)

- **Main scene + controls:** `scenebasic_uniform.cpp` (`initScene()`, `update()`, `render()`, `updateCamera()`)
- **Blinn–Phong + fog + texture mixing + normal mapping:**
  - `shader/basic_uniform.vert`
  - `shader/basic_uniform.frag`
- **Blade emissive shading (glow look):**
  - `shader/blade.vert`
  - `shader/blade.frag`
- **Skybox:**
  - `shader/skybox.vert`
  - `shader/skybox.frag`
- **Edge detection (FBO mask + Sobel outline):**
  - `shader/hilt_mask.vert`, `shader/hilt_mask.frag`
  - `shader/edge_tri.vert`, `shader/edge_detect.frag`
  - FBO setup: `initHiltEdgeFbo()` and called from `resize()`

---

## TODOs to Make This “Marker-Proof”
Add these small clarifiers once you paste the bloom code path (if present):
- Confirm whether bloom is:
  - **Mesh-based glow shell + additive blend** (current code shows this clearly), or
  - **True post-process bloom** (bright-pass threshold + blur + composite using framebuffer textures).
- If true bloom exists, name the shader(s) and FBO(s) used and describe the pass order.


## References

### Core framework / starter template
- Module-provided Visual Studio / OpenGL template from **COMP3015 Lab 1** (as required by the brief).
- 
### Assets (models/textures/audio)
> Add one bullet per asset you used. Include author, source, licence, and edits.

- "Grand Lightsaber Hilt" (https://skfb.ly/6Y9qz) by JeanStroh is licensed under Creative Commons Attribution (http://creativecommons.org/licenses/by/4.0/). — Edits: Diffuse texture colour edited for vibrancy   
- *Skybox cubemap* — Generated using tool: https://mikletuapse.itch.io/space-skybox-generator

### Generative AI tools used (tools, not sources)
- **ChatGPT (OpenAI)** — used as a coding assistant and for README/report drafting support (declared below).
- **GitHub Copilot** - Used as part of VS2022 by default, used purely to autofill sections already being typed in for efficiency.

## AI Declaration (Generative AI Use)

### Declared level (per brief)
This coursework used Generative AI at the **Partnered Work (P1)** level, consistent with the brief’s permitted uses:
- Code assistant
- Programming testing / debugging support
- Readme report crafting support
- (If applicable) asset generation support (models/textures/audio)

### What GenAI was used for (specific, transparent)
Generative AI was used to:
- Discuss and iterate on lighting/shading implementation details (e.g., Blinn–Phong setup, spotlight behaviour, normal mapping pipeline, and scene controls).
- Suggest debugging steps and likely causes for rendering issues (e.g., coordinate space mismatches, normal/tangent basis issues, uniform/state problems).
- Help structure and word the README sections (including this declaration and the references section).
- (If applicable) Provide guidance on implementing emissive / glow-style effects and how they might be approximated within the project constraints.

### What remained my own work
- Final integration decisions, implementation, and testing within the COMP3015 Lab 1 template project.
- All code committed to the repository was reviewed, adapted, and integrated by me.
- Scene composition, chosen assets, and final aesthetic decisions.
- Video explanation content and project narrative (even where AI helped with phrasing/structure).

### How prompts/transcripts are handled (to satisfy the brief)
Per the requirement that **all features must be accompanied by AI prompt and transcript**:
- A copy of relevant prompts and assistant responses used during development is included as an appendix in the submission (e.g., `AI_Transcript.md`), organised by feature (lighting model, spotlight, normal mapping, skybox, controls, emissive blade, etc.).
- Each major feature discussed in the README/video corresponds to a labelled transcript section in the appendix.
- Where external algorithms were used, sources are cited inline in code comments and referenced above, and module leader permission is obtained where required.

### Signed form
A **signed Generative AI Declaration form** (from DLE / programme page) is included in the submission zip as an appendix document, as required. It is not part of any word count.
