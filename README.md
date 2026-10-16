# MyGameEngine
> *MyGameEngine* is tentative title and will probably change later in the development

Project of a pretty low-level game engine.
Ideally in the future I'll get rid of all dependencies entirely (including CRT & STL).

`Engine/` is the root for engine itself  
`ExampleGame/` is the root for example game project

## Game assets

The Engine component provides `AssetArchive` for opening one `.mga` archive and `AssetManager` for mounting several
archives and finding an asset by its archive-relative UTF-8 path. `Find` returns a non-owning `Bytes` view; keep the
archive or manager alive while using that view. When paths occur in multiple mounted archives, the most recently
mounted archive wins.

Declare an asset with `MyGameEngine_AddAsset` after creating the game target:

```cmake
MyGameEngine_AddAsset(
  TARGET ExampleGame
  ARCHIVE game
  FILE "Assets/Textures/hero.png"
  AS "textures/hero.png"
)
```

Calls with the same archive name contribute to one archive; different names create additional `.mga` files. The
archive is generated as part of the game build and copied beside the executable. `AS` is optional and defaults to the
source filename. Generated assets work as build graph outputs, or can name an explicit producer target:

```cmake
add_custom_command(OUTPUT "${generated_asset}"
  COMMAND asset_generator "${generated_asset}"
  DEPENDS asset_generator_source
  VERBATIM)
add_custom_target(GenerateAsset DEPENDS "${generated_asset}")
MyGameEngine_AddAsset(TARGET ExampleGame ARCHIVE game FILE "${generated_asset}"
  AS "generated/item.bin" GENERATED_BY GenerateAsset)
```

The current archive format is intentionally small and has no compression or checksums. All integers are little-endian.
It starts with a 32-bit asset count. Each record contains a 32-bit UTF-8 filename byte length, that many filename
bytes, a 64-bit payload offset, and a 64-bit payload size. Raw payloads follow the complete record table. Paths are
relative, use `/`, and are case-sensitive. The writer sorts entries by path and rejects duplicate names.

Engine is split into "components". Component can enable some optional features (additional graphic APIs, support for
controller, VR support, etc.). Your game executable target must be linking to `MyGameEngine` target. By doing so, two
components are enabled by default: `Engine` and `Launch`. Note that components may have dependencies, and they will be
enabled too (i.e. almost every component depends on `Core` component).
