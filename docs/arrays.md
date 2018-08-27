# Arrays
Arrays can be used to store many values of the same type. The size of an array
must be known when declaring it.
```
var array: [i64, 5] = [1, 2, 3, 4, 5]
```
# Accessing Array Members
The subscript operator can be used to access a member of the array.
```
var first_element: i64 = array[0]
var second_element: i64 = array[1]
var third_element: i64 = array[2]
```

# Referencing An Array
It is inefficient to copy the value of an array, it is often a much better
choice to reference, rather than copy an array.
```
var array_val: [i64, 5] = [1, 2, 3, 4, 5]
var array_ref: &[i64, 5] = &array_val
```

# Variable Length Arrays
When writing a function to process arrays, it is useful to be able to operate on
arrays whose size is not known at compile time. Array reference types can be
written without a size parameter to accomplish this. For bounds safety, this
should always be accompanied with a size parameter which holds the runtime size
of the array
```
func index_of(list: &[i64], size: i64, element: i64) -> i64 {
  var index: i64 = 0
  while index < size {
    if list[index] == element then return index
    index = index + 1
  }
   return -1
}
```
