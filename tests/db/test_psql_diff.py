#!/usr/bin/env python3
# type: ignore[attr-defined]

import difflib
import os
import sys
import tempfile
import unittest

sys.path.append(os.getcwd())
import test_db_defines


def Diff(left_file: str, right_file: str) -> bool:
    with open(left_file, "r") as lhs:
        with open(right_file, "r") as rhs:
            diffs = difflib.unified_diff(lhs.readlines(), rhs.readlines())
            is_diff = False
            for a_line in diffs:
                print(a_line)
                is_diff = True

    return is_diff


class TestPsqlDiff(unittest.TestCase):
    @classmethod
    def setUpClass(cls) -> None:
        cls.PSQL_OUT_FILENAME = "psql.out.txt"
        cls.PSQLXX_OUT_FILENAME = "psqlxx.out.txt"

    def test_DefaultNoAlignAreIdentical(self) -> None:
        with tempfile.TemporaryDirectory() as tmp_dir_name:
            psql_file = os.path.join(tmp_dir_name, self.PSQL_OUT_FILENAME)
            psql_command = f'psql "{test_db_defines.SHARED_DB_VIEWER_CONNECTION_STRING}" -l -A -o {psql_file}'
            self.assertEqual(0, os.system(psql_command))

            psqlxx_file = os.path.join(tmp_dir_name, self.PSQLXX_OUT_FILENAME)
            psqlxx_command = f'{test_db_defines.PSQLXX_EXE} --connection-string "{test_db_defines.SHARED_DB_VIEWER_CONNECTION_STRING}" -l -A -o {psqlxx_file}'
            self.assertEqual(0, os.system(psqlxx_command))

            self.assertFalse(Diff(psql_file, psqlxx_file))


if __name__ == "__main__":
    unittest.main()
