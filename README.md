# COMP3015-CW1

## Project Overview
This project is an OpenGL prototype focused on real-time lighting, texturing, and post-processing, themed around rendering a **lightsaber**.

The scene centres on a textured 3D lightsaber hilt that uses **diffuse and normal mapping** for surface detail, plus a toggleable glowing blade. The blade glow is achieved using an **emissive glow / halo effect** (rim lighting + additive blending) to create the recognisable “lightsaber” look, and the project also includes optional visual techniques such as **fog**, **edge detection**, and **texture mixing** (rust overlay).

The overall goal is to showcase a stylised but technically grounded rendering setup using GLSL shaders, lighting, and texture-based techniques.

https://github.com/user-attachments/assets/3c9ecd3c-407d-431a-aa53-2da09e3eedd7

---

## Download / Build / Run

### Download from GitHub (ZIP)
1. Open the repository page on GitHub.
2. Click **Code** → **Download ZIP**.
3. Extract the ZIP somewhere sensible (e.g., `Documents/COMP3015-CW1/`).

### Running the pre-built executable
- A compiled **.exe** is included in the download (see the included executable folder in the repo/submission).
- Run the `COMP3015_CW1_OCole.exe` directly to use the prototype without Visual Studio.
- If Windows blocks it: right click → **Properties** → tick **Unblock** → Apply.

### Building from source (Visual Studio)
1. Open the solution file: `Project_Template.sln` (name may differ slightly).
2. In Visual Studio:
   - Set configuration to **Release** (recommended).
   - Set platform to **x64** (if available / supported by the template).
3. Build: **Build → Build Solution**
4. Run:
   - **Local Windows Debugger**, or **Ctrl+F5** to run without debugging.

If textures/models appear missing, ensure the working directory is set so the program can find the `media/` folder (project root). The included executable is already configured with the correct asset paths.

---

## How to Use (Controls)

### Model / Camera
- **W / S** — Rotate the lightsaber hilt up/down.
- **A / D** — Orbit the camera left/right (yaw).

### Lightsaber / Visual Style
- **Q** — Toggle the lightsaber blade on/off (including glow).
- **E** — Cycle lightsaber blade colours.
- **R** — Toggle texture mixing (adds/removes a rusty texture blend on the hilt).
- **F** — Toggle fog on/off.
- **C** — Toggle edge detection on/off.

### Render Features Demonstrated
- **Diffuse + normal mapping** on the lightsaber hilt.
- **Emissive glow / halo** for the blade (rim lighting + additive blending).
- Optional effects: **fog**, **edge detection**, **texture mixing**.

---

## Technical Summary (Features Implemented)
- **Custom shading model (GLSL): Blinn–Phong lighting**
  - Implemented in `shader/basic_uniform.frag` using the half-vector method for specular highlights.
- **Lighting**
  - A moving **spotlight** is updated each frame to create animated highlights.
  - A second light contribution is also present for consistent fill lighting.
- **Texturing**
  - **Diffuse texture sampling** on the hilt and the workbench plane.
  - **Normal mapping** on the hilt using tangent-space (TBN) basis from `basic_uniform.vert`.
  - **Texture mixing** (rust overlay) controlled by `MixAmount` and toggled with `R`.
- **Fog (lighting subtechnique)**
  - Toggleable linear fog in `basic_uniform.frag`, using view-space distance (`ViewPos`).
- **Skybox**
  - Cubemap skybox rendered with `shader/skybox.vert/frag`, with optional fog tinting.
- **Edge detection (framebuffer feature)**
  - Hilt-only outline via an off-screen **FBO mask pass** and **Sobel edge detect** fullscreen pass (`edge_detect.frag`), toggled with `C`.
- **Blade glow (emissive halo)**
  - Blade is rendered with an emissive shader (`blade.vert/frag`) using rim lighting.
  - Glow halo is achieved using stencil + additive blending with a scaled “shell” pass.

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

## References

### Core framework / starter template
- Module-provided Visual Studio / OpenGL template from **COMP3015 Lab 1** (as required by the brief).

### Assets (models/textures/audio)
- "Grand Lightsaber Hilt" (https://skfb.ly/6Y9qz) by JeanStroh is licensed under Creative Commons Attribution (http://creativecommons.org/licenses/by/4.0/). — Edits: Diffuse texture colour edited for vibrancy
- *Skybox cubemap* — Generated using tool: https://mikletuapse.itch.io/space-skybox-generator

### Generative AI tools used (tools, not sources)
- **ChatGPT (OpenAI)** — used as a coding assistant and for README/report drafting support (declared below).
- **GitHub Copilot** — used in VS2022 for code autocompletion of sections already being written (efficiency only).

---

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
- Provide guidance on implementing the blade’s emissive/glow look (rim lighting, additive blending, stencil masking).

### What remained my own work
- Final integration decisions, implementation, and testing within the COMP3015 Lab 1 template project.
- All code committed to the repository was reviewed, adapted, and integrated by me.
- Scene composition, chosen assets, and final aesthetic decisions.
- Video explanation content and project narrative.

### How prompts/transcripts are handled (to satisfy the brief)
Per the requirement that **all features must be accompanied by AI prompt and transcript**:
- A copy of relevant prompts and assistant responses used during development is included as an appendix in the submission (e.g., `AI_Transcript.md`), organised by feature (lighting model, spotlight, normal mapping, skybox, controls, blade glow, edge detection, etc.).
- Each major feature discussed in the README/video corresponds to a labelled transcript section in the appendix.
- Where external algorithms were used, sources are cited inline in code comments and referenced above (and module leader permission obtained where required).

### Signed form
A **signed Generative AI Declaration form** (from DLE / programme page) is included in the submission zip as an appendix document, as required. It is not part of any word count.
