# Levenshtein
A quick implementation of the Levenshtein distance in C++ with a CLI 

The dependency on C++ 14 is pretty much unnecessary, I may rewrite it as ANSI C if I get around to it.

I think there's still a bunch of bugs, I'll iron them out later. I use this to provide suggestions in my Bash scripts, it doesn't need to be perfect at the moment.

## Compilation

Release:   
`gcc -o levenshtein --std=c++14 -Iinclude src/main.cpp src/levenshtein.cpp -fno-exceptions -fno-rtti -O2`

Debug:  
`gcc -o levenshtein --std=c++14 -Iinclude src/main.cpp src/levenshtein.cpp -fno-exceptions -fno-rtti -Og -DDEBUG`

## CLI usage

```bash
levenshtein [COMMAND] [SOURCE] [TARGETS...]
```

Exemples: 
```bash
levenshtein source target1 target2 target3 # Print the list of Levenshtein distances between source and target1, source and target2, source and target3 ... 
levenshtein --distance source target1 target2 target3 # Same 
levenshtein --cutoff 2 source target1 target2 target3 # Print all the targets with a Levenshtein distance <= cutoff
levenshtein --best source target1 target2 target3 # Print the targets with the smallest Levenshtein distance to the source
``` 
