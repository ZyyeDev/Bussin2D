## Bussin2D
Bussin2D is a 2D Framework for making games using LUA. It is designed to be lightweight and fast.

## Usage

To run a game in the current directory:

```bash
bussin2d debug

```

To build a standalone executable:

```bash
bussin2d build windows

```


## Example

```lua
function buss.ready()
    buss.window.init(720, 480, "Hello World")
end

function buss.draw()
    -- Draw a blue rectangle
    buss.graphics.drawRect(100, 100, 400, 300, 0, 100, 255, 255)
end
```
