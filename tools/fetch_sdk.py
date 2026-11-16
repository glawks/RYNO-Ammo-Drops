#!/usr/bin/env python3
"""Download and verify the OpenRAC SDK pinned in sdk.lock (stdlib only)."""
from __future__ import annotations
import hashlib
import json
from pathlib import Path
import urllib.request
import zipfile
from io import BytesIO

root = Path(__file__).resolve().parent.parent
lock = json.loads((root / "sdk.lock").read_text())
version = lock["version"]
out = root / ".sdk" / f"openrac-mod-sdk-{version}"
if (out / "cmake" / "OpenRACModConfig.cmake").is_file():
    print(f"SDK already available at {out}")
else:
    print(f"Downloading OpenRAC SDK {version}...")
    data = urllib.request.urlopen(lock["url"], timeout=120).read()
    digest = hashlib.sha256(data).hexdigest()
    if digest != lock["sha256"]:
        raise ValueError(f"SDK checksum mismatch: {digest}")
    out.parent.mkdir(exist_ok=True)
    with zipfile.ZipFile(BytesIO(data)) as archive:
        prefix = f"openrac-mod-sdk-{version}/"
        for info in archive.infolist():
            p = Path(info.filename)
            if not info.filename.startswith(prefix) or ".." in p.parts or p.is_absolute():
                raise ValueError(f"Unsafe SDK entry: {info.filename}")
        archive.extractall(out.parent)
    print(f"SDK ready at {out}")
