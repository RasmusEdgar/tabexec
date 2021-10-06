# Tabexec
[//]:  

Launcher for AmigaOS 3.x, with tab and auto completion.

Tabexec is meant to be used as a process launcher.

Tabexec is inspired by Dmenu by suckless.org.

## Configuration and ToolTypes
[//]:  

Tabexec is configured through the AmigaOS ToolType system.

### Paths to scan
[//]:  

Set paths with info ToolType PATHS, like so:

```
PATHS=SYS:C:,SYS:,SYS:Utilities
```

Default PATHS TT is set to: SYS:C:,SYS:Tools

### Stack size
[//]:  

The stacksize for launched programs can be changed with the
PSTACK TT.

Example:
```
PSTACK=8192
```

Default stack size is: 16384

### Colors
[//]:  

The colors of the tabexec window can be changed with the following TTs.

Change the front and backgrounbd colors of the window:
```
FPEN=<integer>
BPEN=<integer>
```

Change the front and background color of the seperator:
```
FPEN_SEP=<integer>
BPEN_SEP=<integer>
```

### Fonts
[//]:  

At the moment tabexec looks best when using the same system default font and screen font.

The fonts will probably be configurable in a later release.

## Installation
[//]:  

Copy tabexec and tabexec.info to somewhere in your path.

Example:
```
copy tabexec C:
copy tabexec.info C:
```
Or launch from where you see fit.

The files tabexec and tabexec.info should be kept together.

## Usage
[//]:  

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

## Bugs
[//]:  

There are probably bugs. Report them on github: https://github.com/RasmusEdgar/tabexec

Tested and is stable on AmigaOS 3.1.4 and 3.2 on both fs-uae and Amiga 1200.

Tested and is stable on AmigaOS 3.9 on fs-uae.

### Known bugs
[//]:  

It crashes sometimes on a specific distribution of AmigaOS 3.9 running on V1200.

Reason not yet determined.
