# MyGameEngine
> *MyGameEngine* is tentative title and will probably change later in the development

Project of a pretty low-level game engine.
Ideally in the future I'll get rid of all dependencies entirely (including CRT & STL).

`Engine/` is the root for engine itself  
`ExampleGame/` is the root for example game project

Engine is split into "components". Component can enable some optional features (additional graphic APIs, support for
controller, VR support, etc.). Your game executable target must be linking to `MyGameEngine` target. By doing so, two
components are enabled by default: `Engine` and `Launch`. Note that components may have dependencies and they will be
enabled too (i.e. almost every component depends on `Core` component).
