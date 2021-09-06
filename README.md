# tabexec

Launcher for AmigaOS 3.x, with tab and auto completion.

Tabexec is meant to be used as a process launcher.

Tabexec is inspired by Dmenu by suckless.org.

## Configuration and ToolTypes

Tabexec is configured through the AmigaOS ToolType system.

## Paths to scan

Set paths with info ToolType PATHS, like so:

```
PATHS=C:,System:System,System:Utilities
```

Default PATHS TT is set to: C:,System:Tools

## Stack size

The stacksize for launched programs can be changed with the
PSTACK TT.

Example:
```
PSTACK=8192
```

Default stack size is: 32768

## Usage

The files tabexec and tabexec.info should be kept together.

Tabexec makes most sense used as a shortcut launched app.

* Launch tabexec
* Start typing command
* Tab suggestions, continously tabbing will move through suggestions
* Ctrl-j and Ctrl-l moves back and forth through suggestions
* Return launches command
* Esc exits

Info files are filtered out by default.
