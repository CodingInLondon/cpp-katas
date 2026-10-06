#!/usr/bin/env python3
"""
render_landscape.py — Modern C++ "Top 5 Features" landscape A4 PDF renderer.

Turns the cpp-top-features markdown into the styled landscape PDF
(photo cover, horizontal timeline, two-column feature grid, dark code panels,
full-width treatment for long code samples, summary table on its own page,
and every C++ standard starting on a fresh page).

Usage:
    python3 render_landscape.py INPUT.md [-o OUTPUT.pdf] [--cover IMAGE] [--doc-version V]

Examples:
    python3 render_landscape.py cpp-top-features.md
    python3 render_landscape.py new.md -o build/modern-cpp.pdf
    python3 render_landscape.py new.md --cover assets/other-photo.jpg
    python3 render_landscape.py new.md -o docs/new-v2.pdf   # cover shows "v2"

Fonts (bundled in ./fonts) and the default cover (./assets/cover.jpg) are
resolved relative to this script, so you can run it from any directory.
Dependencies: weasyprint, markdown-it-py, pygments  (see requirements.txt).
"""
import re, html, argparse, sys, pathlib
from markdown_it import MarkdownIt
from pygments import highlight
from pygments.lexers import CppLexer
from pygments.formatters import HtmlFormatter
from weasyprint import HTML

HERE = pathlib.Path(__file__).resolve().parent
FONT_DIR = HERE / "fonts"
DEFAULT_COVER = HERE / "assets" / "cover.jpg"
FONT_FILES = {
    "playfair": "PlayfairDisplay.ttf",
    "franklin": "LibreFranklin.ttf",
    "mono_regular": "JetBrainsMono-Regular.ttf",
    "mono_bold": "JetBrainsMono-Bold.ttf",
}
LONG_CODE_LINES = 16   # code blocks with >= this many lines become full-width

# --------------------------------------------------------------------------- #
# Markdown -> HTML helpers
# --------------------------------------------------------------------------- #
def build_timeline(src: str) -> str:
    """Render a mermaid 'timeline' block as a horizontal standards timeline."""
    title, items = "", []
    for ln in src.split("\n"):
        s = ln.strip()
        if not s or s == "timeline":
            continue
        if s.startswith("title "):
            title = s[6:].strip(); continue
        mm = re.match(r"(\S+?):\s*(.+)", s)
        if not mm:
            continue
        year, rest = mm.group(1).strip(), mm.group(2).strip()
        name, desc = (rest.split(" - ", 1) if " - " in rest else (rest, ""))
        items.append((year, name.strip(), desc.strip()))
    cells = "".join(f"""
        <div class="tlh-cell">
          <div class="tlh-year">{html.escape(y)}</div>
          <div class="tlh-line"><span class="tlh-node"></span></div>
          <div class="tlh-name">{html.escape(n)}</div>
          <div class="tlh-desc">{html.escape(d)}</div>
        </div>""" for y, n, d in items)
    cap = f'<div class="tlh-title">{html.escape(title)}</div>' if title else ""
    return f'<figure class="timeline-h">{cap}<div class="tlh-track">{cells}</div></figure>'


_fmt = HtmlFormatter(cssclass="codehilite")

def _make_md():
    md = MarkdownIt("commonmark", {"typographer": False}).enable("table")

    def render_fence(tokens, idx, options, env):
        tok = tokens[idx]
        if (tok.info or "").strip().lower() == "mermaid":
            return build_timeline(tok.content)
        out = highlight(tok.content, CppLexer(), _fmt)
        # Tall code wraps badly in a half-width grid cell and gets split across
        # pages; flag it so its list item can span the full width.
        if tok.content.count("\n") >= LONG_CODE_LINES:
            out = out.replace('class="codehilite"', 'class="codehilite long"', 1)
        return out

    md.renderer.rules["fence"] = render_fence
    return md


def strip_tags(s): return re.sub(r"<[^>]+>", "", s).strip()

def build_cols(content: str) -> str:
    """Feature list -> grid of pairs; a flagged 'wide' (long-code) item becomes
    its own full-width single-column list so its use-case stays attached."""
    if 'class="wide"' not in content:
        return content.replace('<ol>', '<ol class="feat">', 1)
    lis = re.findall(r'<li\b[^>]*>.*?</li>', content, flags=re.S)
    widx = next((k for k, l in enumerate(lis) if 'class="wide"' in l), None)
    if widx is None:
        return content.replace('<ol>', '<ol class="feat">', 1)
    before, wide, after = lis[:widx], lis[widx], lis[widx + 1:]
    out = ''
    if before: out += '<ol class="feat">' + ''.join(before) + '</ol>'
    out += '<ol class="feat solo">' + wide + '</ol>'
    if after: out += '<ol class="feat">' + ''.join(after) + '</ol>'
    return out


# --------------------------------------------------------------------------- #
# Document assembly
# --------------------------------------------------------------------------- #
def build_html(md_text: str, cover_uri: str, doc_version: str = "") -> str:
    lines = md_text.split("\n")
    cover_h1 = lines[0].lstrip("# ").strip()
    body_md = "\n".join(lines[1:])
    cover_main, cover_dek = (cover_h1.split(" - ", 1) if " - " in cover_h1 else (cover_h1, ""))
    m = re.match(r'(?i)\s*top\s*5\s*features\s*', cover_dek)
    if m:
        cover_dek = cover_dek[m.end():]

    md = _make_md()
    body_html = md.render(body_md)
    # Promote any list item containing a long code block to full width.
    body_html = re.sub(r'<li>((?:(?!</li>).)*?codehilite long(?:(?!</li>).)*?)</li>',
                       r'<li class="wide">\1</li>', body_html, flags=re.S)

    # Group into: centred intro + one section per standard + summary.
    chunks = re.split(r'(<h[12][^>]*>.*?</h[12]>)', body_html, flags=re.S)
    parts = [f'<div class="intro">{chunks[0]}</div>']
    i = 1
    while i < len(chunks):
        heading = chunks[i]; content = chunks[i + 1] if i + 1 < len(chunks) else ""
        i += 2
        if "<h1" in heading:
            heading = heading.replace("<h1>", '<h1 class="section-h">')
            parts.append(f'<section class="std summary">{heading}{content}</section>')
        else:
            parts.append(f'<section class="std">{heading}{build_cols(content)}</section>')
    body_struct = "\n".join(parts)

    fp = {k: (FONT_DIR / v).as_uri() for k, v in FONT_FILES.items()}
    ver_html = (f'<div class="ver">VERSION&nbsp;&nbsp;·&nbsp;&nbsp;<b>{html.escape(doc_version)}</b></div>'
                if doc_version else "")

    return f"""<!DOCTYPE html><html><head><meta charset="utf-8"><style>
* {{ box-sizing:border-box; }}
@font-face {{ font-family:'Playfair'; src:url('{fp["playfair"]}'); font-weight:400 900; }}
@font-face {{ font-family:'Franklin'; src:url('{fp["franklin"]}'); font-weight:100 900; }}
@font-face {{ font-family:'JBMono'; src:url('{fp["mono_regular"]}'); font-weight:400; }}
@font-face {{ font-family:'JBMono'; src:url('{fp["mono_bold"]}'); font-weight:700; }}
:root {{ --green:#8DC63F; --green-deep:#5f7f2b; --ink:#1c1f1a; --muted:#6b7280;
  --rule:#d9ddd2; --codebg:#0e1714; --codeink:#e6e8e3; }}
@page {{ size:A4 landscape; margin:20mm 16mm 14mm 16mm;
  @top-left {{ content:"Modern C++  |  Top 5 Features  |  " string(section, start);
    font-family:'Franklin'; font-size:8pt; letter-spacing:.4pt; color:var(--muted);
    vertical-align:bottom; padding-bottom:7pt; margin-bottom:5mm; border-bottom:.6pt solid var(--rule); width:100%; }}
  @top-right {{ content:""; vertical-align:bottom; padding-bottom:4pt; margin-bottom:5mm; border-bottom:.6pt solid var(--rule);
    width:18pt; height:20pt; }}
  @bottom-right {{ content:counter(page); font-family:'Franklin'; font-size:8.5pt; color:var(--muted); }}
  @bottom-left {{ content:"Modern C++ - Walkthrough"; font-family:'Franklin'; font-size:7.5pt; color:#aab0a3; letter-spacing:.3pt; }} }}
@page cover {{ margin:0; @top-left{{content:none}} @top-right{{content:none}} @bottom-right{{content:none}} @bottom-left{{content:none}} }}
/* no default 8px body margin, so the cover bleeds to all four page edges;
   main keeps the same 8px side inset the content pages were laid out with */
html, body {{ margin:0; }}
main {{ margin:0 8px; }}
body {{ font-family:'Franklin'; font-size:9.5pt; line-height:1.46; color:var(--ink); }}

/* cover (photo background) */
.cover {{ page:cover; position:relative; width:297mm; height:210mm; color:#fff; overflow:hidden; page-break-after:always;
  background-image:
    linear-gradient(100deg, rgba(6,11,8,.90) 0%, rgba(6,11,8,.64) 34%, rgba(6,11,8,.24) 60%, rgba(6,11,8,.38) 100%),
    url('{cover_uri}');
  background-size: cover, cover; background-position: center center, center center; background-repeat: no-repeat, no-repeat; }}
.cover .block {{ position:absolute; top:72mm; left:22mm; right:120mm; }}
.cover .eyebrow {{ font-family:'Franklin'; font-weight:600; letter-spacing:6pt; font-size:11pt;
  color:var(--green); text-transform:uppercase; margin-bottom:6mm; text-shadow:0 1px 6px rgba(0,0,0,.65); }}
.cover h1 {{ font-family:'Playfair'; font-weight:800; font-size:66pt; line-height:.98; margin:0; color:#fff; letter-spacing:-.5pt;
  text-shadow:0 2px 18px rgba(0,0,0,.6),0 1px 3px rgba(0,0,0,.55); }}
.cover .dek {{ font-family:'Franklin'; font-weight:300; font-size:15pt; color:#dde2d6; margin-top:7mm; text-shadow:0 1px 6px rgba(0,0,0,.6); }}
.cover .rule {{ width:54mm; height:2.5pt; background:var(--green); margin-top:10mm; }}
.cover .src {{ position:absolute; bottom:18mm; left:22mm; font-family:'Franklin'; font-size:9pt; letter-spacing:.4pt; color:#cdd3c4; text-shadow:0 1px 5px rgba(0,0,0,.7); }}
.cover .src b, .cover .ver b {{ color:var(--green); font-weight:600; }}
.cover .ver {{ position:absolute; bottom:18mm; right:22mm; font-family:'Franklin'; font-size:9pt; letter-spacing:.4pt; color:#cdd3c4; text-shadow:0 1px 5px rgba(0,0,0,.7); }}

/* intro: centred on its own page */
.intro {{ display:flex; flex-direction:column; justify-content:center; min-height:172mm; margin:0; }}

/* sections: each standard on a fresh page; full-width heading + feature grid */
.std {{ break-inside:auto; }}
section.std {{ break-before:page; counter-reset:item; }}
.feat {{ display:grid; grid-template-columns:1fr 1fr; column-gap:11mm; row-gap:14pt; margin:2pt 0 14pt; }}
.feat:last-child {{ margin-bottom:0; }}
.feat.solo {{ grid-template-columns:1fr; }}
.sec-init {{ string-set: section "Overview"; }}
h2 {{ string-set: section content(); font-family:'Playfair'; font-weight:700; font-size:19pt; color:var(--ink);
  margin:14pt 0 9pt; padding-top:2pt; border-bottom:.6pt solid var(--rule); padding-bottom:7pt; position:relative; break-after:avoid; }}
h2::after {{ content:""; position:absolute; left:0; bottom:-.6pt; width:40pt; height:2.4pt; background:var(--green); }}
h1.section-h {{ string-set: section "Summary"; font-family:'Playfair'; font-weight:800; font-size:22pt; color:var(--ink);
  margin:16pt 0 9pt; padding-bottom:7pt; border-bottom:1.2pt solid var(--ink); break-after:avoid; }}
p {{ margin:6pt 0; }}
hr {{ display:none; }}
strong {{ font-weight:700; color:#14160f; }}
em {{ color:var(--green-deep); font-style:italic; font-weight:600; }}
ol {{ padding:0; margin:2pt 0 6pt; list-style:none; }}
ol > li {{ counter-increment:item; position:relative; padding-left:27pt; margin:0; break-inside:avoid; min-width:0; }}
ol > li::before {{ content:counter(item); position:absolute; left:0; top:1pt; width:18pt; height:18pt;
  background:var(--green); color:#10160a; font-family:'Franklin'; font-weight:700; font-size:9.5pt;
  border-radius:50%; text-align:center; line-height:18pt; }}
ul {{ margin:5pt 0 5pt 16pt; }}
code {{ font-family:'JBMono'; font-size:8.1pt; background:#eef2ea; color:#2f3b22; padding:.6pt 3pt; border-radius:3pt; }}

.codehilite {{ background:var(--codebg); border-radius:7pt; border-left:3pt solid var(--green); margin:7pt 0 9pt;
  padding:8pt 11pt; break-inside:avoid; box-shadow:0 1pt 2pt rgba(0,0,0,.18);
  font-family:'JBMono'; font-size:8pt; line-height:1.5; color:var(--codeink); }}
.codehilite pre {{ margin:0; white-space:pre-wrap; word-break:break-word; }}
.codehilite pre, .codehilite code {{ background:transparent; color:inherit; font-family:inherit; font-size:inherit; padding:0; border:none; border-radius:0; }}
.codehilite .c, .codehilite .c1, .codehilite .cm {{ color:#7d8a78; font-style:italic; }}
.codehilite .cp, .codehilite .cpf {{ color:#b98edb; }}
.codehilite .k, .codehilite .kr, .codehilite .kc {{ color:#8DC63F; font-weight:700; }}
.codehilite .kt {{ color:#5fc9dd; }}
.codehilite .nf {{ color:#61afef; }}
.codehilite .nc, .codehilite .nn {{ color:#e5c07b; }}
.codehilite .nb {{ color:#5fc9dd; }}
.codehilite .s, .codehilite .s1, .codehilite .s2, .codehilite .sc {{ color:#e6b873; }}
.codehilite .mi, .codehilite .mf, .codehilite .mh {{ color:#d8985f; }}
.codehilite .o, .codehilite .p {{ color:#cdd2c6; }}
.codehilite .n {{ color:#e6e8e3; }}

/* horizontal timeline */
.timeline-h {{ margin:30pt 0 32pt; break-inside:avoid; }}
.tlh-title {{ font-family:'Playfair'; font-weight:700; font-size:15pt; color:var(--ink); margin-bottom:14pt; text-align:center; }}
.tlh-track {{ display:table; table-layout:fixed; width:100%; }}
.tlh-cell {{ display:table-cell; width:16.66%; padding:0 6pt; vertical-align:top; text-align:center; }}
.tlh-year {{ font-family:'Playfair'; font-weight:700; font-size:15pt; color:var(--green-deep); margin-bottom:7pt; }}
.tlh-line {{ position:relative; height:0; border-top:2pt solid #cfe3a8; margin:0 0 11pt; }}
.tlh-node {{ position:absolute; left:50%; top:-6.5pt; margin-left:-6.5pt; width:13pt; height:13pt; border-radius:50%;
  background:var(--green); border:2pt solid #fff; box-shadow:0 0 0 1.5pt #cfe3a8; }}
.tlh-name {{ font-family:'Franklin'; font-weight:700; font-size:11pt; color:#14160f; margin-bottom:3pt; }}
.tlh-desc {{ font-family:'Franklin'; font-size:8.4pt; color:#555f4c; line-height:1.38; }}

/* summary table */
table {{ border-collapse:collapse; width:100%; margin:4pt 0 6pt; font-size:9pt; }}
tr {{ break-inside:avoid; }}
thead th {{ background:#2c3a1c; color:#fff; font-family:'Franklin'; font-weight:600; text-align:left;
  padding:7pt 10pt; letter-spacing:.3pt; border-bottom:2pt solid var(--green); }}
tbody td {{ padding:5.5pt 10pt; border-bottom:.5pt solid #e4e7dd; vertical-align:top; }}
tbody tr:nth-child(even) td {{ background:#f6f8f1; }}
tbody td:first-child {{ font-weight:700; color:#2c3a1c; }}
tbody td:nth-child(2) {{ color:#14160f; }}
</style></head>
<body>
<section class="cover">
  <div class="block">
    <div class="eyebrow">{html.escape(cover_main)}</div>
    <h1>Top&nbsp;5 Features</h1>
    <div class="dek">{html.escape(cover_dek)}</div>
    <div class="rule"></div>
  </div>
  <div class="src">SOURCE&nbsp;&nbsp;·&nbsp;&nbsp;<b>github.com/CodingInLondon/cpp-katas</b></div>
  {ver_html}
</section>
<main><span class="sec-init"></span>
{body_struct}
</main></body></html>"""


def main(argv=None):
    ap = argparse.ArgumentParser(description="Render the Modern C++ landscape PDF from markdown.")
    ap.add_argument("input", type=pathlib.Path, help="input markdown file")
    ap.add_argument("-o", "--output", type=pathlib.Path, default=None,
                    help="output PDF path (default: <input>_landscape.pdf)")
    ap.add_argument("--cover", type=pathlib.Path, default=DEFAULT_COVER,
                    help=f"cover background image (default: {DEFAULT_COVER})")
    ap.add_argument("--doc-version", default=None,
                    help="version shown bottom-right on the cover, e.g. v2 "
                         "(default: taken from an output name ending in -vN.pdf)")
    ap.add_argument("--html", type=pathlib.Path, default=None,
                    help="also write the intermediate HTML to this path (debug)")
    args = ap.parse_args(argv)

    # Validate inputs with friendly messages.
    missing = [p for p in [FONT_DIR / f for f in FONT_FILES.values()] if not p.exists()]
    if missing:
        sys.exit("ERROR: missing bundled fonts:\n  " + "\n  ".join(map(str, missing)) +
                 "\nRun ./setup_fonts.sh to re-download them.")
    if not args.input.exists():
        sys.exit(f"ERROR: input markdown not found: {args.input}")
    if not args.cover.exists():
        sys.exit(f"ERROR: cover image not found: {args.cover}")

    out = args.output or args.input.with_name(args.input.stem + "_landscape.pdf")
    out.parent.mkdir(parents=True, exist_ok=True)

    md_text = args.input.read_text(encoding="utf-8")
    doc_version = args.doc_version
    if doc_version is None:
        m = re.search(r"-(v\d+(?:\.\d+)*)$", out.stem)
        doc_version = m.group(1) if m else ""
    doc = build_html(md_text, args.cover.resolve().as_uri(), doc_version)
    if args.html:
        args.html.write_text(doc, encoding="utf-8")

    # base_url lets WeasyPrint resolve any remaining relative refs from here.
    HTML(string=doc, base_url=str(HERE)).write_pdf(str(out))
    print(f"Wrote {out}")


if __name__ == "__main__":
    main()
