#!/usr/bin/env bash
# Compile and run every feature sample. Requires g++-14 (for C++23 samples).
# Usage: ./build.sh          # build + run all
#        ./build.sh cpp17    # build + run one version only
set -u

CXX=${CXX:-g++-14}
ROOT="$(cd "$(dirname "$0")" && pwd)"
BUILD="$ROOT/build"
mkdir -p "$BUILD"

pass=0; fail=0
run_one() { # std  src  out  [extra flags...]
    local std="$1" src="$2" out="$3"; shift 3
    local bin="$BUILD/$out"
    if "$CXX" -std="$std" -O2 -Wall "$@" "$src" -o "$bin" 2>"$BUILD/$out.log"; then
        if "$bin" >/dev/null 2>>"$BUILD/$out.log"; then
            echo "  PASS  $out"; pass=$((pass+1)); return
        fi
    fi
    echo "  FAIL  $out (see build/$out.log)"; fail=$((fail+1))
}

build_cpp11() { for f in "$ROOT"/cpp11/*.cpp; do run_one c++11 "$f" "$(basename "$f" .cpp)"; done; }
build_cpp14() { for f in "$ROOT"/cpp14/*.cpp; do run_one c++14 "$f" "$(basename "$f" .cpp)"; done; }
build_cpp17() { for f in "$ROOT"/cpp17/*.cpp; do run_one c++17 "$f" "$(basename "$f" .cpp)"; done; }

build_cpp20() {
    for f in "$ROOT"/cpp20/*.cpp; do
        local name; name="$(basename "$f" .cpp)"
        [ "$name" = "modules" ] && continue
        run_one c++20 "$f" "$name"
    done
    # modules: compile the interface unit first, then the consumer.
    ( cd "$BUILD" \
      && "$CXX" -std=c++20 -fmodules-ts -c -x c++ "$ROOT/cpp20/greet.cppm" -o greet.o \
      && "$CXX" -std=c++20 -fmodules-ts "$ROOT/cpp20/modules.cpp" greet.o -o modules ) \
      2>"$BUILD/modules.log"
    if [ -x "$BUILD/modules" ] && "$BUILD/modules" >/dev/null 2>>"$BUILD/modules.log"; then
        echo "  PASS  modules"; pass=$((pass+1))
    else
        echo "  FAIL  modules (see build/modules.log)"; fail=$((fail+1))
    fi
}

build_cpp23() {
    for f in "$ROOT"/cpp23/*.cpp; do
        local name; name="$(basename "$f" .cpp)"
        if [ "$name" = "mdspan" ]; then
            run_one c++23 "$f" "$name" -I"$ROOT/cpp23"
        else
            run_one c++23 "$f" "$name"
        fi
    done
}

declare -A versions=( [cpp11]=build_cpp11 [cpp14]=build_cpp14 [cpp17]=build_cpp17 \
                      [cpp20]=build_cpp20 [cpp23]=build_cpp23 )

targets=( cpp11 cpp14 cpp17 cpp20 cpp23 )
[ $# -gt 0 ] && targets=( "$@" )

for v in "${targets[@]}"; do
    echo "== $v =="
    "${versions[$v]}"
done

echo "-----------------------------"
echo "Total: $pass passed, $fail failed"
exit $((fail > 0))
