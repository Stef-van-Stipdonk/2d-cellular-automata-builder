# cellular automaton rule 30

Cellular automaton for rule 30 in C, using SDL2.
## Functions:
In the rule.txt file, write your rules.  
Rules are as follows:  
<bit><bit><bit>-<bit>  
The 3 leading bits represent the state needed to execute the center rule.  
So 000-1, would result in the center bit becoming 1 aka true.
Each rule takes up one line, as an example:
111-0
110-0
101-0
100-1
011-1
010-1
001-1
000-0

## Dependencies
 - SDL2
 - My own base layer for arena allocator use(base.h)
