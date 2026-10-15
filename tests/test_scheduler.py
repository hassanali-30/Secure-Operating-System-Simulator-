import subprocess
import unittest
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]


class SchedulerTests(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.binary = ROOT / "os_test_bin"
        subprocess.run(
            ["g++", "-std=c++17", "-Wall", "-Wextra", "-pedantic", "OS.cpp", "-o", str(cls.binary)],
            cwd=ROOT,
            check=True,
        )

    @classmethod
    def tearDownClass(cls):
        cls.binary.unlink(missing_ok=True)

    def run_scheduler(self, input_text):
        return subprocess.run(
            [str(self.binary)],
            input=input_text,
            text=True,
            capture_output=True,
            check=True,
        ).stdout

    def test_arrival_during_slice_runs_before_requeued_process(self):
        output = self.run_scheduler("2\n0 6\n1 1\n")
        self.assertIn("2\t1\t1\t2\t3", output)

    def test_arrival_at_slice_boundary_runs_before_requeued_process(self):
        output = self.run_scheduler("2\n0 6\n3 1\n")
        self.assertIn("2\t3\t1\t0\t1", output)


if __name__ == "__main__":
    unittest.main()
