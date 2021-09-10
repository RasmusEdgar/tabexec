# Tabexec

Launcher for AmigaOS 3.x, with tab and auto completion.

Tabexec is meant to be used as a process launcher.

Tabexec is inspired by Dmenu by suckless.org.

## Configuration and ToolTypes

Tabexec is configured through the AmigaOS ToolType system.

### Paths to scan

Set paths with info ToolType PATHS, like so:

```
PATHS=C:,System:System,System:Utilities
```

Default PATHS TT is set to: C:,System:Tools

### Stack size

The stacksize for launched programs can be changed with the
PSTACK TT.

Example:
```
PSTACK=8192
```

Default stack size is: 32768

### Colors

The colors of the tabexec window can be changed with the following TTs.

Change the front and backgrounbd coloris of the window:
```
FPEN=<integer>
BPEN=<integer>
```

Change the front and background color of the seperator:
```
FPEN_SEP=<integer>
BPEN_SEP=<integer>
```

## Installation

Copy tabexec and tabexec.info to somewhere in your path.

```
copy tabexec C:
copy tabexec.info C:
```

The files tabexec and tabexec.info should be kept together.

## Usage

It is a good companion to dintwm (a tiling wm also found on aminet),
but is perfectly usable a standalone launcher.

Bind it to a shortcut key, launch it with the mouse or dedicate a CMD entry
to it in dintwm.

Tabexec makes most sense used as a shortcut launched app.

Tabexec gadget grabs input.

In short:

* Launch tabexec
* Start typing command
* Tab suggestions, continously tabbing will move through suggestions
* Ctrl-j and Ctrl-l moves back and forth through suggestions
* Return launches command
* Esc exits

Info files are filtered out of suggestions.
