# pl0-compiler

A simple pl/0 language compiler for learning lexical analysis, syntax analysis and sematic analysis, not finished. Homework of CSU compiler course.

input: sample2.txt

```
const hey = 100;
var t, a0, a1, i;
begin
   i := 1;
   a0 := 0;
   a1 := 1;
   while i < hey do
   begin
      t := a1;
      a1 := a0 + a1;
      a0 := t;
      i := i + 1;
   end
end
```

run:

```
pl0_compiler sample2.txt
```

result:
```
( <3:const, const>  <12:id, hey>  <14:op, =>  <13:int, 100>  <16:semi, ;> 
 <4:var, var>  <12:id, t>  <15:comma, ,>  <12:id, a0>  <15:comma, ,>  <12:id, a1>  <15:comma, ,>  <12:id, i>  <16:semi, ;> 

(( <1:begin, begin> 
(( <12:id, i>  <14:op, :=> ((( <13:int, 1> ))))) <16:semi, ;> 
(( <12:id, a0>  <14:op, :=> ((( <13:int, 0> ))))) <16:semi, ;> 
(( <12:id, a1>  <14:op, :=> ((( <13:int, 1> ))))) <16:semi, ;> 
(( <7:while, while> ((((( <12:id, i> ))) <14:op, <> ((( <12:id, hey> )))))
 <8:do, do> 
( <1:begin, begin> 
(( <12:id, t>  <14:op, :=> ((( <12:id, a1> ))))) <16:semi, ;> 
(( <12:id, a1>  <14:op, :=> ((( <12:id, a0> )) <14:op, +> (( <12:id, a1> ))))) <16:semi, ;> 
(( <12:id, a0>  <14:op, :=> ((( <12:id, t> ))))) <16:semi, ;> 
(( <12:id, i>  <14:op, :=> ((( <12:id, i> )) <14:op, +> (( <13:int, 1> ))))) <16:semi, ;> 
 <2:end, end> 
)))
 <2:end, end> 
)))
add 1 0 v0
add v0 0 i
add 0 0 v1
add v1 0 a0
add 1 0 v2
add v2 0 a1
l0: add i 0 v3
add 100 0 v4
jl v3 v4 l1
add a1 0 v6
add v6 0 t
add a0 0 v7
add a1 0 v8
add v7 v8 v7
add v7 0 a1
add t 0 v9
add v9 0 a0
add i 0 v10
add 1 0 v11
add v10 v11 v10
add v10 0 i
jeq 0 0 l0
l1: 

```
