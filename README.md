# Tabexec
[//]: #del  

Launcher for AmigaOS 3.x, with tab and auto completion.

Tabexec is meant to be used as a process launcher.

Tabexec is inspired by Dmenu by suckless.org.

## Configuration and ToolTypes
[//]: #del  

Tabexec is configured through the AmigaOS ToolType system.

### Paths to scan
[//]: #del  

Set paths with info ToolType PATHS, like so:

```
PATHS=SYS:C:,SYS:,SYS:Utilities
```

Default PATHS TT is set to: SYS:C:,SYS:Tools

### Stack size
[//]: #del  

The stacksize for launched programs can be changed with the
PSTACK TT.

Example:
```
PSTACK=8192
```

Default stack size is: 16384

### Colors
[//]: #del  

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

Change the front and background color of the current highlighted selection:
```
FPEN_HL=<integer>
BPEN_HL=<integer>
```

### Boxed window
[//]: #del  

If you want to run tabexec in a boxed window instead of an inline window, the
following TTs can be set.

Enable boxed window mode:
```
BOX=<any text>
```

Change height of window box:
```
BOX_H=<integer>
```

Center window box:
```
BOX_C=<any text>
```

Change x coodirnate for window box:
```
BOX_X=<integer>
```

Change y coordinate for window box:
```
BOX_Y=<integer>
```

Window box width is set to maximum gadget string width and can not be changed.

### Fonts
[//]: #del  

At the moment tabexec looks best when using the same system default font and screen font.

The fonts will probably be configurable in a later release.

## Installation
[//]: #del  

Copy tabexec and tabexec.info to somewhere in your path.

Example:
```
copy tabexec C:
copy tabexec.info C:
```
Or launch from where you see fit.

The files tabexec and tabexec.info should be kept together.

## Usage
[//]: #del  

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
[//]: #del  

There are probably bugs. Report them on github: https://github.com/RasmusEdgar/tabexec

Tested and is stable on AmigaOS 3.1.4 and 3.2 on both fs-uae and Amiga 1200.

Tested and is stable on AmigaOS 3.9 on fs-uae.

### Known bugs
[//]: #del  

It crashes sometimes on a specific distribution of AmigaOS 3.9 running on V1200.

Reason not yet determined.
