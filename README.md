# MonoComposure

**An Unreal Engine compositor that ingests a background video feed and composites it with Unreal CG in a single pass/renderer.** It also supports translucent materials and transparent backgrounds, making it suitable for virtual production and mixed-reality style workflows.

> Plugin module: `GPUCompositionGraphicsPipeline`

---

## Features

- Ingests a background video feed (via `UMediaTexture`) and composites it with rendered Unreal CG.
- Single-pass / single-renderer composition for the combined output.
- Support for translucent materials and transparent backgrounds.
- Adjustable blend weights for the CG overlay and the virtual world overlay.
- "Show only" actor filtering to isolate specific subjects in the composition.
- Optional output straight to the viewport.
- Fully **Blueprint-callable** API via an engine subsystem.

---

## Requirements

- Unreal Engine 5.x (Tested on 5.5, 5.6,5.7. Linux is supported for SM5)
- The **Media Framework** plugin (for `UMediaTexture` input)

---

## Installation

1. Clone or download this repository into your project's `Plugins` folder:

   ```bash
   cd <YourProject>/Plugins
   git clone https://github.com/Saish-in3d/MonoComposure.git
   ```

2. Regenerate your project files (right-click the `.uproject` → *Generate Visual Studio project files*).
3. Build the project, then enable **GPUCompositionGraphicsPipeline** under *Edit → Plugins* if it isn't already.

---

## Repository Structure

| Path | Description |
| --- | --- |
| `Config/` | Plugin configuration files. |
| `Content/` | Plugin assets (materials, render targets, etc.). |
| `Resources/` | Plugin resources (icons, etc.). |
| `Shaders/` | Composition shader source. |
| `Source/GPUCompositionGraphicsPipeline/` | C++ source for the compositor module. |
| `GPUCompositionGraphicsPipeline.uplugin` | Plugin descriptor. |

---

## Quick Start

The core of the plugin is the `UCompGraphicsSubsystem` engine subsystem, accessible from both C++ and Blueprints.

```cpp
// Grab the subsystem
UCompGraphicsSubsystem* Compositor = GEngine->GetEngineSubsystem<UCompGraphicsSubsystem>();

// Configure the output (resolution, format, etc.)
FCompositorOutputSpecs Specs;
Compositor->ConfigureCompositorOutput(Specs);

// Feed in the background footage
Compositor->SetInputMediaTexture(MyMediaTexture, /*bTransparent=*/ false);

// Initialize and run
Compositor->InitilizeCompositor();
Compositor->StartCompositor();

// Use the composited result
UTextureRenderTarget2D* Result = Compositor->GetCompositionOutputRenderTarget();
```

**Typical lifecycle:**

```
InitilizeCompositor()  ->  ConfigureCompositorOutput()  ->  SetInputMediaTexture()
        ->  StartCompositor()  ->  (read GetCompositionOutputRenderTarget)
        ->  EndCompositor()  ->  DeinitializeCompositor()
```

---

## API Reference — `UCompGraphicsSubsystem`

All functions are `BlueprintCallable` under the **GPU Composition** category unless noted otherwise.

### Lifecycle

| Function | Description |
| --- | --- |
| `void InitilizeCompositor()` | Initializes the compositor and its internal resources. |
| `void StartCompositor()` | Starts the composition process. |
| `void EndCompositor()` | Stops the composition process. |
| `void DeinitializeCompositor()` | Tears down the compositor and releases its resources. |
| `bool GetShouldCompositorRun()` | Returns whether the compositor is currently set to run. |

### Input & Output

| Function | Description |
| --- | --- |
| `void SetInputMediaTexture(UMediaTexture* inMediaTexture, bool TransparentMediaTexture)` | Sets the background media texture and whether it should be treated as transparent. |
| `void ConfigureCompositorOutput(FCompositorOutputSpecs inCompositorOutputSpecs)` | Configures the output specs (resolution, format, etc.). |
| `void SetCompositionResolution(float X, float Y)` | Sets the resolution of the composited output. |
| `UTextureRenderTarget2D* GetCompositionOutputRenderTarget()` | Returns the render target containing the final composited output. |

### Blend / Overlay Controls

| Function | Description |
| --- | --- |
| `void SetCGOverlayonBackgroundFootageValue(float Value)` | Sets the weight of the CG overlay drawn over the background footage. |
| `float GetCGOverlayonBackgroundFootageValue()` | Returns the current CG-over-footage overlay value. |
| `void SetVirtualWorldOverlayValue(float Value)` | Sets the weight of the virtual world overlay. |
| `float GetVirtualWorldOverlayValue()` | Returns the current virtual world overlay value. |

> Both overlay values default to `1.0`.

### Show-Only Actors

Restrict which actors are rendered into the composition — useful for isolating specific subjects from the rest of the scene.

| Function | Description |
| --- | --- |
| `void SetShowOnlyActors(TArray<AActor*> inActorsArray)` | Replaces the show-only set with the supplied array of actors. |
| `void AddShowOnlyActor(AActor* inActor)` | Adds a single actor to the show-only set. |
| `void RemoveShowOnlyActor(AActor* inActor)` | Removes a single actor from the show-only set. |
| `void ResetShowOnlyActors()` | Clears the show-only set. |

### Viewport Output

| Function | Description |
| --- | --- |
| `void ToggleCompOutputOnViewport(bool inShallCompOutputOnViewport)` | Enables or disables drawing the composited output to the viewport. |
| `bool GetShallCompOutputOnViewport()` | Returns whether the composited output is drawn to the viewport. |

---

## Contributing

Issues and pull requests are welcome. For larger changes, please open an issue first to discuss what you'd like to change.

---

## License

`This project is licensed under the MIT License.`

`Note that use of this plugin also requires Unreal Engine and is subject to the Unreal Engine EULA. The MIT License covers only the MonoComposure source code in this repository.`

---

## Author

**Saish Sakharkar** ([@Saish-in3d](https://github.com/Saish-in3d))
