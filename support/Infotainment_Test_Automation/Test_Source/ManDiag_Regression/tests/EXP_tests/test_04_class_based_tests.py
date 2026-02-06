import pytest


# Purpose: Show class-based test organization.

@pytest.mark.exp
@pytest.mark.classdemo
class TestCalculator:
    """EXP-ClassDemo: Arithmetic operations in class structure"""

    def test_add(self):
        assert 2 + 3 == 5

    def test_subtract(self):
        assert 10 - 4 == 6

    @pytest.mark.parametrize("x,y,result", [(2, 2, 4), (3, 3, 6), (4, 5, 9)])
    def test_add_param(self, x, y, result):
        assert x + y == result
