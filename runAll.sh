#!/bin/zsh

# Copied from ~/.custom_aliases
alias G++CPPC='g++-11 -std=c++20 -Wall -Wextra -Werror -Wpedantic -nostdinc++ -nodefaultlibs -lc -isystem /usr/local/Cellar/llvm/14.0.6_1/include/c++/v1 -lc++ -lc++abi'
alias CPPC='/usr/local/Cellar/llvm/14.0.6_1/bin/clang++ -std=c++20 -Wall -Wextra -Werror -Wpedantic'

for folder in Day*/;
do
    ( # These parentheses mean everything is run in a subshell, with its own current working directory.
        cd $folder
        echo "________${folder:0:-1}________"
        echo "[Compiling Solution]"
        CPPC -O3 solution.cpp -o solution
        echo "[Running]"
        time ./solution < input.txt
    )
done
