# Banner-themed screen cards

The README shows OLED screens inside 720×480 cards derived from the main Mini Clock banner's
background: the navy glow, scanlines, faint clock face, pixel font, and blue colours.
The banner and these cards share one background and palette; no separate artwork or firmware UI is
introduced.

Each card is a different 720×480 crop of the shared 1280×640 background, with a title, subtitle,
bezel, and footer added **outside** the OLED viewport. The empty cards are saved in
[`backgrounds/`](backgrounds/).

| Card | Original OLED asset | Output |
|---|---|---|
| Digital | [Digital preview](../scr_clock_main.gif) | [Digital card](scr_clock_main.gif) |
| Menu | [Menu preview](../scr_clock_menu.gif) | [Menu card](scr_clock_menu.gif) |
| Analog | [Analog preview](../scr_clock_analog.gif) | [Analog card](scr_clock_analog.gif) |
| Setting | [Setting preview](../scr_clock_setting.png) | [Setting card](scr_clock_setting.png) |
| Ringing | [Ringing preview](../scr_clock_ringing.gif) | [Ringing card](scr_clock_ringing.gif) |

The OLED viewport is exactly **512×256** at **(104, 128)**: the original 128×64 pixels enlarged 4×
with nearest-neighbour sampling. No artwork, text, or tint is placed over the screen, and the source
previews are unchanged. GIF frame order, per-frame durations, and looping are preserved.

The README's Objects table intentionally uses **unframed** previews: the same 512×256 OLED image on
a plain 512×288 black canvas, in [`../../objects/table/`](../../objects/table/).
