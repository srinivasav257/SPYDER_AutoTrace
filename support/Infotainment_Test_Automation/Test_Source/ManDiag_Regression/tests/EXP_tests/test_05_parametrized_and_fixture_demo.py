import pytest


# Purpose: Demonstrate fixtures + parameterization.

@pytest.fixture
def sample_numbers():
    """Fixture providing a list of numbers"""
    return [1, 2, 3, 4, 5]


@pytest.mark.exp
@pytest.mark.order(1)
def test_sum(sample_numbers):
    """EXP-Fixture: Verify sum of fixture data"""
    assert sum(sample_numbers) == 15


@pytest.mark.exp
@pytest.mark.parametrize("a,b,expected", [(1, 2, 3), (2, 3, 5), (10, 5, 15)])
def test_param_addition(a, b, expected):
    """EXP-Param: Verify parameterized addition"""
    assert a + b == expected


@pytest.mark.exp
@pytest.mark.parametrize("value", [0, 1, 2, 3])
def test_even_check(value):
    """EXP-Param: Verify even/odd detection"""
    assert (value % 2 == 0) or (value % 2 == 1)
