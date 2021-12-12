#!/bin/zsh

for folder in Day*/;
do
    ( # These parentheses mean everything is run in a subshell, with its own current working directory.
        cd $folder
        echo "________${folder:0:-1}________"
        echo "[Compiling Solution]"
        g++-11 -std=c++20 -Wall -Wextra -Werror -Wpedantic solution.cpp -o solution
        echo "[Running]"
        ./solution < input.txt
    )
done