#!/usr/bin/env bash
# Re-download the bundled fonts into ./fonts (they are included already;
# use this only if they go missing or you want to refresh them).
# All three families are SIL Open Font License 1.1.
set -euo pipefail
cd "$(dirname "$0")"
mkdir -p fonts

dl() { echo "  $2"; curl -fsSL "$1" -o "fonts/$2"; }

echo "Downloading fonts..."
dl "https://raw.githubusercontent.com/google/fonts/main/ofl/playfairdisplay/PlayfairDisplay%5Bwght%5D.ttf" "PlayfairDisplay.ttf"
dl "https://raw.githubusercontent.com/google/fonts/main/ofl/librefranklin/LibreFranklin%5Bwght%5D.ttf"       "LibreFranklin.ttf"
dl "https://raw.githubusercontent.com/JetBrains/JetBrainsMono/master/fonts/ttf/JetBrainsMono-Regular.ttf"    "JetBrainsMono-Regular.ttf"
dl "https://raw.githubusercontent.com/JetBrains/JetBrainsMono/master/fonts/ttf/JetBrainsMono-Bold.ttf"       "JetBrainsMono-Bold.ttf"
echo "Done."
