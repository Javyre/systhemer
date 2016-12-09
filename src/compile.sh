#!/usr/bin/env sh

cd ../src
flex uni_lex.lex
bison -d parser.y

cd ../build
echo "current dir:" $(pwd)
cmake ../src && \
make clean -j 3 && \
make -j 3 && \
valgrind --tool=memcheck --leak-check=full \
         --show-reachable=yes --track-origins=yes \
         ./systhemer -vwt && \
valgrind --tool=memcheck --leak-check=full \
         --show-reachable=yes --track-origins=yes \
         ./systhemer -vw
cd ../src
echo $(pwd)
