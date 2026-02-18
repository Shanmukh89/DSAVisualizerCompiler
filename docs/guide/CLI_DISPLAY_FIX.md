# CLI Display Fix

## Problem
Windows console was displaying garbled characters instead of box-drawing characters:
- `ΓòÉ` instead of `═`
- `Γöî` instead of `┌`
- `Γöé` instead of `│`

## Solution
Replaced Unicode box-drawing characters with simple ASCII characters for universal compatibility:

**Before (Unicode):**
```
┌───────┬───────┐
│  [ 2]  │  [ 4]  │
└───────┴───────┘
```

**After (ASCII):**
```
+-------+-------+
|  [ 2]  |  [ 4]  |
+-------+-------+
```

## Improvements

1. **Simple ASCII Boxes**: Uses `+`, `-`, `|` characters that work everywhere
2. **Windows Console Support**: Enabled UTF-8 and ANSI escape sequence support on Windows
3. **Clear Visual Indicators**: Uses `*` for highlighted indices instead of colors
4. **Removed ANSI Colors**: Replaced with clear text labels like `[OK]`, `[SORTED]`, `[PUSH]`, `[POP]`

## Display Format

```
=============================================================
  Bubble Sort Visualization
=============================================================

-> Comparing: 2 and 4
  2 <= 4 -> No swap needed

+-------+-------+-------+-------+-------+
|  [ 2]  |  [ 4]  |  [12]  |  [ 3]  |  [ 4]  |
+-------+-------+-------+-------+-------+
   * 0*    * 1*     2       3       4

[Press any key to continue...]
```

## Compatibility

- ✅ Works on Windows (PowerShell, CMD, Windows Terminal)
- ✅ Works on Linux/Mac terminals
- ✅ No encoding issues
- ✅ Clear and readable output

