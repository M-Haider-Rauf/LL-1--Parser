# LL-1--Parser
Implementation of an LL(1) Parser. Given an LL(1) grammar and a string, It will check if the string belongs to grammar, and
print the derivation rules. The algorithms are taken from the "Dragon Book" on Compilers. 

# Rules
Since I wanted to focus on the parsing part, and not lexing there are some rules for defining grammars and strings:
* Grammars are written like S -> A | b | c
* Symbols starting with a capital letter are non-terminals.
* All other symbols are considered terminals.
* \# indicates empty string
* $ indicates end-of-input
* all symbols should be separated by spaces like S -> A | b B a
* input should be separated by spaces like ( id + id )

# Screenshots
![img1](https://i.ibb.co/fQTxdvT/Screenshot-192.png) ![img2](https://i.ibb.co/xYGRwFZ/Screenshot-193.png)
![img3](https://i.ibb.co/M9mHHg8/Screenshot-195.png)



