# Typing module tests for ByteVM Python

# Function parameter annotations
def func_with_params(x: int, y: str, z: float = 1.0) -> bool:
    return True

# Return annotation
def func_return_list() -> list[int]:
    return [1, 2, 3]

# Variable annotations
x: int = 42
y: str = "hello"
z: list[int] = [1, 2, 3]
w: dict[str, int] = {"a": 1}

# Nested generics
nested: dict[str, tuple[int, float]] = {"key": (1, 2.0)}

# typing module access
import typing

t_optional: typing.Optional[int] = None
t_union: typing.Union[int, str] = "test"
t_list: typing.List[float] = [1.0, 2.0]
t_dict: typing.Dict[str, int] = {"a": 1}
t_tuple: typing.Tuple[int, str] = (1, "hello")
t_set: typing.Set[int] = {1, 2, 3}
t_frozenset: typing.FrozenSet[str] = frozenset(["a", "b"])
t_callable: typing.Callable[[int], str] = str

# TypeVar and Generic
T = typing.TypeVar('T')
U = typing.TypeVar('U')

class Container(typing.Generic[T]):
    def __init__(self, value: T):
        self.value: T = value
    def get(self) -> T:
        return self.value

# Annotated
annotated: typing.Annotated[int, "metadata"] = 42

# ClassVar and Final
class MyClass:
    class_var: typing.ClassVar[int] = 10
    instance_var: int = 20
    CONSTANT: typing.Final[str] = "const"

# NoReturn
def never_returns() -> typing.NoReturn:
    raise SystemExit

# Text and AnyStr
text_val: typing.Text = "unicode"
any_str_val: typing.AnyStr = "bytes or str"

# __annotations__ introspection
print("func_with_params annotations:", func_with_params.__annotations__)
print("func_return_list annotations:", func_return_list.__annotations__)
print("x annotations:", __annotations__)

# Verify typing forms evaluate correctly
print("typing.List[int]:", typing.List[int])
print("typing.Optional[str]:", typing.Optional[str])
print("typing.Union[int, str]:", typing.Union[int, str])

# Verify function annotations are stored
assert func_with_params.__annotations__["x"] == int
assert func_with_params.__annotations__["y"] == str
assert func_with_params.__annotations__["return"] == bool

# Verify variable annotations
assert __annotations__["x"] == int
assert __annotations__["y"] == str

# Verify typing forms
assert str(typing.List[int]) == "typing.List"
assert str(typing.Optional[str]) == "typing.Optional"

print("All typing tests passed!")