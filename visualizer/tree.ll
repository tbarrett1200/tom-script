; ModuleID = 'test'
source_filename = "test"
target datalayout = "e-m:o-i64:64-f80:128-n8:16:32:64-S128"

define i64 @add(i64 %a, i64 %b) {
entry:
  %0 = add i64 %b, %a
  ret i64 %0
}

define i64 @foo() {
entry:
  %calltmp = call i64 @add(i64 10, i64 5)
  %0 = add i64 %calltmp, 10
  ret i64 %0
}

define i64 @bar() {
entry:
  ret i64 0
}

define i64 @main() {
entry:
  ret i64 5
}
