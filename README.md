# tom-script

```Swift
var alpha: Int;
var beta: Int;

func add(a: Int, b: Int) -> void {
  return a+b
}
```

# Diagnostic Error Reporting
```
test:5:0: error: expected ;
func (a: Int, b: Int) -> void {
^
test:8:15: error: expected ;
    return a+c b;
               ^
```

# todo
Create backend. Navigate ParseTree using visitor pattern
