# Render presets

These files store the render/slider parameters used by the visual demo.

## In the app

- `F9` — open the preset browser
- `Up/Down` — select a preset
- `Enter` — load it
- `s` — save the current settings over the selected preset
- `n` — create a new preset (`preset_<name>.txt`)
- `r` — reload the list
- `Esc` / `F9` — close the browser
- `D` — quick-save the current settings into the currently loaded preset

`presets/default.txt` contains the built-in defaults. `screen-1794842753.txt`
contains the settings from the screenshot.

## From the command line

```bash
cd /home/br11k/git/Bonsai
make run-visual PARAMS=presets/screen-1794842753.txt
```

or directly:

```bash
./build/bonsai2 -i model3_child_compact.tipsy \
  --dev 0 --renderdev 0 \
  --params presets/screen-1794842753.txt \
  --presetdir presets -T 1000
```

The file format is the one written by the app's `D` key: a `render_params`
header followed by one `name value` pair per slider, in the same order as
`SmokeRenderer::initParams()`.
