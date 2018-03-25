# Calling Convention
Interpreted TomScript uses a stack-only calling convention. Although using
registers has huge benefits in compiled code, there is already a large overhead
in interpreted code, so the speed benefits of emulating registers is negligible.
Also, using registers complicates the interpretation process and creates the
need for register allocation algorithms. Thus, I stuck with stack only function
calls for simplicity.

# The Stack Frame
Note: while x86 stacks grow from top to bottom in memory, the TomScript calling
convention grows from bottom to top.
* return value
* arguments
* last base
* local variables
* temporary variables

# Function Calling Process
``` c++
stack.push(nullptr);
traverse(call.params);
traverse(call.decl.stmt);
stack.pop(call.decl.params.size())
```
