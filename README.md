# Resource Loader Demo

A small C++20 demo that loads JSON resources from a Minecraft-like datapack layout, registers loaders by registry name, and prints loaded resource details.

## Features

- Datapack-style path parsing: `data/<namespace>/<registry>/<path>.json`
- Registry-based dispatch with type-safe loaders
- Minimal JSON parser (`MiniJson`) with objects/arrays/strings/numbers
- Example resource type (`ExampleConcrete`) loaded from JSON

## Project Layout

- `demo.cpp`: Demo entry point
- `ResourceLoader.*`: Scans and parses datapack paths, dispatches to registries
- `ResourceManager.*`: Registry storage and dispatch
- `Registry.*`: Typed registry with loader callback
- `JsonResource.*`: Base class for JSON-backed resources
- `MiniJson.hpp`: Minimal JSON parser
- `ExampleConcrete.*`: Example resource type implementation
- `demo_pack/`: Sample datapack content

## Datapack Layout

Files are discovered under `demo_pack/data` and parsed as:

```
data/<namespace>/<registry>/<path>.json
```

Example:

```
demo_pack/data/city/buildings/apartment.json
```

Becomes:

- namespace: `city`
- registry: `buildings`
- path: `apartment`

## Build

Requirements:

- CMake 3.16+
- C++20 compiler (MSVC, Clang, or GCC)

### Windows (PowerShell)

```
cmake -S . -B build
cmake --build build --config Debug
```

### macOS / Linux

```
cmake -S . -B build
cmake --build build
```

## Run

From the repo root so the demo can find `demo_pack`:

### Windows (PowerShell)

```
.\build\Debug\resource_loader_demo.exe
```

### macOS / Linux

```
./build/resource_loader_demo
```

## Example Output

Output will print each loaded resource and a summary count, for example:

```
city:apartment | name='Apartment' | floors=8 | tags=[residential, highrise] | file=.../apartment.json
city:supermarket | name='Supermarket' | floors=1 | tags=[retail] | file=.../supermarket.json
Loaded 2 building resources.
```

## How It Works

1. `ResourceManager` stores registries by name.
2. `ResourceLoader` scans `demo_pack/data`, parses paths into `(registry, key, file)`.
3. Each discovered file is dispatched to the matching registry.
4. The registry loader calls `loadJsonResource<T>`, which parses JSON and populates the resource.

## Extending

To add a new resource type:

1. Create a class derived from `rl::JsonResource` with `loadFromJson`.
2. Register a new `rl::Registry<YourType>` with a loader function.
3. Add JSON files under `demo_pack/data/<namespace>/<registry>/`.

## License

MIT. See [LICENSE](LICENSE).
