#!/usr/bin/env sh

cd ../src
flex uni_lex.lex
bison -rall -d parser.y

cd ../build
echo "current dir:" $(pwd)
cmake . && \
make clean -j 3 && \
make -j 3 && \
valgrind --tool=memcheck --leak-check=full \
         --show-reachable=yes --track-origins=yes \
         ./systhemer -vwt && \
valgrind --tool=memcheck --leak-check=full \
         --show-reachable=yes --track-origins=yes \
         ./systhemer -vw ../files/default4.uth
cd ../src
echo $(pwd)
