import pytest
# Purpose: Show skipping, xfail, and test ordering.

@pytest.mark.exp
@pytest.mark.order(1)
def test_login_page_loads():
    """EXP-DemoFlow: Simulate login page load"""
    assert True

@pytest.mark.exp
@pytest.mark.order(2)
@pytest.mark.skip(reason="Feature not implemented yet")
def test_login_with_invalid_user():
    """EXP-DemoFlow: Validate login failure for invalid user"""
    assert False

@pytest.mark.exp
@pytest.mark.order(3)
@pytest.mark.xfail(reason="Known issue: division by zero not handled")
def test_divide_by_zero():
    """EXP-DemoFlow: Demonstrate expected failure (xfail)"""
    _ = 10 / 0
