from __future__ import annotations

import subprocess
import sys
import zipfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent
SCRIPTS = ROOT / "scripts"
sys.path.insert(0, str(SCRIPTS))

import build_mod

FINAL_BUILDS = (
    "NTSC",
    "DUCKSTATION",
    "IOP_PS2_NTSC",
    "DECKARD_PS2_NTSC",
)


def _build_platform(build: str) -> None:
    subprocess.check_call(
        [sys.executable, str(SCRIPTS / "build_mod.py"), build],
        cwd=ROOT,
    )


def _zip_platform(build: str) -> Path:
    bin_name, cue_name = build_mod.ISO_OUT[build]
    dist = Path(build_mod.DIST_DIR)
    zip_dir = dist / "zips"
    zip_dir.mkdir(parents=True, exist_ok=True)

    bin_path = dist / bin_name
    cue_path = dist / cue_name
    missing = [str(path) for path in (bin_path, cue_path) if not path.is_file()]
    if missing:
        raise FileNotFoundError("Missing build output(s): " + ", ".join(missing))

    zip_path = zip_dir / f"{bin_path.stem}.zip"
    if zip_path.exists():
        zip_path.unlink()

    with zipfile.ZipFile(zip_path, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=1) as zf:
        zf.write(bin_path, arcname=bin_path.name)
        zf.write(cue_path, arcname=cue_path.name)

    return zip_path


def main() -> None:
    for i, build in enumerate(FINAL_BUILDS):
        print(f"{i+1}/{len(FINAL_BUILDS)}: Building {build}...")
        
        _build_platform(build)
        print(f"\nZipping Build {build}...\n")
        zip_path = _zip_platform(build)
        print(f"Zipped: {zip_path}\n")
    print("Done.")


if __name__ == "__main__":
    main()
    
