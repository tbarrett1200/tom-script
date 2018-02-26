# TupleType
```Swift
()
(Type, Type)
(foo: Type, bar: Type)
(foo: Type, Type)
(foo: Type, _:Type)

```

(Type1) -> (Type1, Type2) -> (Type1, Type2, Type3) -> Int

# FunctionType
```Swift
() -> ()
(Type, Type) -> Type
(Type) -> (Type, Type)
(Type) -> (Type) -> Type
```

# FunctionParams
```Swift
() -> ()
(foo bar: Type) -> Type
(_ bar: Type) -> Type
(bar: Type) -> Type

```

# TypeElement
```Swift
foo bar: Type
_ bar: Type
bar: Type
_: Type
Type
```

# TypeList
```Swift
TypeElement, TypeList
```

# TupleType
```Swift
(TypeList)
```

# FunctionType
```Swift
TupleType -> Type
```

# Type
```Swift
FunctionType | TupleType | TypeIdentifier

if (token == identifier) -> TypeIdentifier

```
