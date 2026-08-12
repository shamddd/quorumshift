"""
Integration tests for Distributed Storage Simulator via Python control plane.
"""

from pathlib import Path

import pytest
from dss_sim.controller import ClusterController


@pytest.fixture
def controller() -> ClusterController:
    root_dir = Path(__file__).resolve().parent.parent
    bin_path = str(root_dir / "build" / "dss-cli")
    return ClusterController(bin_path)


def test_cli_demo_execution(controller: ClusterController) -> None:
    output = controller.demo()
    assert "DISTRIBUTED STORAGE SIMULATOR — FAULT TOLERANCE DEMO" in output
    assert "LEADER_ELECTED" in output
    assert "REPLICATION_SUCCESS" in output
    assert "NODE_FAILURE" in output
    assert "FAILOVER" in output
    assert "NEW_LEADER_ELECTED" in output
    assert "NODE_RECOVERED" in output
    assert "Demo Execution Completed Successfully!" in output


def test_cli_write_command(controller: ClusterController) -> None:
    write_ok = controller.write("test_key_99", "test_val_99")
    assert write_ok is True
