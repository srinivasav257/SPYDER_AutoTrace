import pytest
import math

# Purpose: Demonstrate simple assertions and logging.

@pytest.mark.exp
@pytest.mark.order(1)
def test_addition():
    """EXP-Arithmetic: Validate addition operation"""
    a, b = 5, 3
    result = a + b
    assert result == 8, f"Expected 8 but got {result}"

@pytest.mark.exp
@pytest.mark.order(2)
def test_subtraction():
    """EXP-Arithmetic: Validate subtraction operation"""
    a, b = 10, 4
    assert a - b == 6

@pytest.mark.exp
@pytest.mark.order(3)
def test_multiplication():
    """EXP-Arithmetic: Validate multiplication operation"""
    assert 5 * 4 == 20

@pytest.mark.exp
@pytest.mark.order(4)
def test_division():
    """EXP-Arithmetic: Validate division operation"""
    assert 10 / 2 == 5

@pytest.mark.exp
@pytest.mark.order(5)
def test_square_root():
    """EXP-Arithmetic: Validate square root operation"""
    assert math.isclose(math.sqrt(16), 4)
