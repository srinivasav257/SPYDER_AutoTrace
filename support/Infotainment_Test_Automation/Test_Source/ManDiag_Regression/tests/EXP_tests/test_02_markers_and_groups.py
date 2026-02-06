import pytest

@pytest.mark.exp
@pytest.mark.math
def test_modulus():
    """EXP-MathOps: Verify modulus operation"""
    assert 10 % 3 == 1

@pytest.mark.exp
@pytest.mark.logic
def test_boolean_and():
    """EXP-Logic: Validate AND logical operation"""
    assert (True and False) is False

@pytest.mark.exp
@pytest.mark.logic
def test_boolean_or():
    """EXP-Logic: Validate OR logical operation"""
    assert (True or False) is True

@pytest.mark.exp
@pytest.mark.math
def test_power_operation():
    """EXP-MathOps: Validate power operation"""
    assert pow(2, 3) == 8
