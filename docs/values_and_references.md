# Values and References
There are two main categories of data that are worked with when programming in
Bulat: values and references.

## Values
A value is the most fundamental data item that one works with in Bulat.
Values are rather self explanatory. All types, whether they be integers,
real numbers, strings, booleans, or any user defined types, can be stored as
a value. Each value is distinct from all other values in that they all have
their own copy of whatever data that they are storing.

### Assigning Values
Values are able to be stored using the assignment operator `=`. Every time a
value is assigned, its data is copied.

```
let a: i64 = 5 // stores the value `5` into `a`
let b: i64 = a // copies the value of `a` into `b`
```

### Comparing Values
The comparison operator `==` can be used to compare all value types. By
default, the comparison operator compares the data stored by two values

```
let a: i64 = 1
let b: i64 = 2
let c: i64 = 2

a == b // evaluates to `false`
a == c // evaluates to `true`
```

## References
A reference is a pointer to a value. While values are simple to use, they are
not always very efficient. For example, when assigning a large value such as a
string or an array, the entire value is copied. If this is done often, it can
slow down your program. Therefore, it is best practice to reference, rather than
assign/copy non trivial values.

### Referencing a Value
A reference stores the location of a value. To reference a value is to
retrieve its location. This can be done using the referencing operator `&`.

```
let a: i32 = 5    // stores the value `5` into `a`
let b: &i32 = &a  // stores the location of `a` into `b`
```

### Dereferencing a Value
In some cases, it may be desirable to create a new value from a reference.
The dereferencing operator `*` creates a new value by copying a referenced
value.

```
let a: i32 = 5    // stores the value `5` into `a`
let b: &i32 = &a  // stores the location of `a` into `b`
let c: i32 = *a   // copies the value referenced by `b` into `c`
```

### Assigning References
References are themselves values, and so they too can be copied using the
assignment operator `=`.

```
let a: i32 = 5    // stores the value `5` into `a`
let b: &i32 = &a  // stores a reference to `a` into `b`
let c: &i32 = b   // copies the reference from `b` into `c`
```


### Comparing References
References are themselves values, and so they too can be compared using the
comparison operator `==`. In the case of references, this checks if
the references point to the same value.

```
let a: i32 = 5    // stores the value `5` into `a`
let b: i32 = 5    // stores the value `5` into `b`
let c: &i32 = &a  // stores a reference to `a` into `c`
let d: &i32 = b   // copies the reference from `c` into `d`
let e: &i32 = &a  // stores a reference to `a` into `e`
let f: &i32 = &b  // stores a reference to `b` into `f`

c == d // evaluates to `true`
c == e // evaluates to `true`
c == f // evaluates to `false`
```
