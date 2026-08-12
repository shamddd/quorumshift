#!/usr/bin/env python3
"""
Simulation script for Distributed Storage Simulator.
Executes animated fault-tolerance sequence via Python control plane.
"""

import sys
from pathlib import Path

# Add python directory to sys.path
sys.path.insert(0, str(Path(__file__).resolve().parent.parent / "python"))

from dss_sim.controller import ClusterController


def main() -> None:
    print("Launching Distributed Storage Simulator Python Interface...")
    controller = ClusterController()
    output = controller.demo()
    print(output)


if __name__ == "__main__":
    main()
