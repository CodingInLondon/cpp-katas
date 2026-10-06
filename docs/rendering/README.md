# Modern C++ — Landscape PDF pipeline

Renders the `cpp-top-features` markdown into the styled **A4 landscape** PDF:
photo cover, horizontal standards timeline, two-column feature grid, dark
syntax-highlighted code panels, a full-width treatment for long code samples,
a summary table on its own page, and every C++ standard starting on a fresh
page. Re-run it on any updated markdown with a single command.

## Contents

```
render_landscape.py     the renderer (markdown -> PDF)
fonts/                  bundled TTF fonts (SIL OFL 1.1)
assets/cover.jpg        default cover photo (swap with --cover)
sample-input.md         the current markdown, as a reference/smoke test
requirements.txt        Python dependencies
setup_fonts.sh          re-download the fonts if they ever go missing
```

## One-time setup

1. System libraries for WeasyPrint (Pango etc.). On Ubuntu/Debian:
   ```bash
   sudo apt install libpango-1.0-0 libpangocairo-1.0-0 libgdk-pixbuf-2.0-0 libffi-dev
   ```
   (If a package name differs on your release, see the official install guide:
   https://doc.courtbouillon.org/weasyprint/stable/first_steps.html)

2. Python packages (a virtualenv is fine):
   ```bash
   pip install -r requirements.txt
   ```

## Usage

```bash
# default output next to the input: cpp-top-features_landscape.pdf
python3 render_landscape.py cpp-top-features.md

# explicit output path
python3 render_landscape.py new.md -o build/modern-cpp.pdf

# different cover photo (any aspect ratio; it is cropped to fill, never stretched)
python3 render_landscape.py new.md --cover assets/other-photo.jpg

# version on the cover (bottom right): taken from an output name ending in
# -vN.pdf, or set explicitly with --doc-version
python3 render_landscape.py new.md -o docs/new-v2.pdf
python3 render_landscape.py new.md -o out.pdf --doc-version v2

# also dump the intermediate HTML for debugging
python3 render_landscape.py new.md --html build/debug.html
```

Smoke test (should print `Wrote ...` and produce a 15-page PDF):
```bash
python3 render_landscape.py sample-input.md -o /tmp/out.pdf
```

Fonts and the default cover are resolved relative to the script, so you can run
it from any working directory.

## What the markdown must contain

The renderer expects the same shape as `sample-input.md`:

- Line 1 is the title: `# Modern C++ - Top 5 features for each version`
  The cover eyebrow is the part before ` - ` ("Modern C++"); the subtitle is
  the part after it with a leading "Top 5 features" removed ("for each version").
  The big cover title "Top 5 Features" is fixed in the template.
- An intro (free prose / bullet lists) followed by a fenced ```` ```mermaid ````
  `timeline` block. Each `YEAR: NAME - description` line becomes one timeline
  cell (the template is sized for six cells).
- One `## C++NN (YEAR)` section per standard, each a numbered list of five
  features. Code goes in ```` ```cpp ```` fences.
- A final `# Summary Table` heading followed by a markdown table.

## Knobs you may want

- **Cover photo** — `--cover PATH` (default `assets/cover.jpg`).
- **Full-width code threshold** — `LONG_CODE_LINES` near the top of
  `render_landscape.py` (default 16). Code blocks with at least this many lines
  are promoted to a full-width single-column item so the sample and its
  use-case stay together on one page instead of splitting.
- **Each standard on its own page** — the rule `section.std { break-before: page; }`.
  Remove `break-before: page` to let standards flow and pack more densely.
- **Colours / fonts / spacing** — all in the single CSS block inside
  `render_landscape.py` (accent `#8DC63F`, ink `#0E1714`, code background
  `#0E1714`, Playfair / Libre Franklin / JetBrains Mono).

## Notes

- Layout engine: WeasyPrint. CSS **grid** (not multi-column) is used for the
  feature pairs because multi-column orphans the heading when a section is
  taller than a page; grid fragments row-by-row and keeps the heading attached.
- Fonts are embedded into the PDF, so the output is self-contained.
- Font licences: Playfair Display, Libre Franklin, and JetBrains Mono are all
  under the SIL Open Font License 1.1.
