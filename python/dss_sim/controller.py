"""
Distributed Storage Simulator Python Control Plane Module.
"""

__version__ = "1.0.0"

import subprocess
from pathlib import Path
from typing import List, Optional


class ClusterController:
    """Python wrapper interface for controlling the C++ Distributed Storage Simulator binary."""

    def __init__(self, binary_path: Optional[str] = None):
        if binary_path is None:
            # Default to build directory binary relative to project root
            root_dir = Path(__file__).resolve().parent.parent.parent
            binary_path = str(root_dir / "build" / "dss-cli")
        self.binary_path = binary_path

    def run_cmd(self, args: List[str]) -> subprocess.CompletedProcess[str]:
        cmd = [self.binary_path] + args
        return subprocess.run(cmd, capture_output=True, text=True, check=True)

    def demo(self) -> str:
        res = self.run_cmd(["demo"])
        return res.stdout

    def write(self, key: str, value: str) -> bool:
        res = self.run_cmd(["write", "--key", key, "--value", value])
        return "Result: SUCCESS" in res.stdout

    def read(self, key: str) -> Optional[str]:
        res = self.run_cmd(["read", "--key", key])
        if "Value=" in res.stdout:
            val_part = res.stdout.split("Value=")[1].split(" (")[0].strip()
            if val_part != "<null>":
                return val_part
        return None
