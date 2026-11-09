# MyGameEngine
> *MyGameEngine* is tentative title and will probably change later in the development

Project of a pretty low-level game engine.
Ideally in the future I'll get rid of all dependencies entirely (including CRT & STL).

`Engine/` is the root for engine itself  
`ExampleGame/` is the root for example game project

Engine is split into "components". Component can enable some optional features (additional graphic APIs, support for
controller, VR support, etc.).

There's 3 main components:
  - `Core` component contains some common code not related to an engine or anything else. This component can be treated
    as some kind of extension to STL, or just as auxiliary library.
  - `Engine` component contains the engine code. This is the biggest component as it's literally the engine.
  - `Launch` component contains entry point wrapper that calls game's main function. This component doesn't really do
    much but still may be useful.

These 3 components are enabled by default and cannot be disabled.
