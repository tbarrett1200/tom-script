NAME=`basename $1 .cpp | cut -f 1 -d '.'` # removes all dirs and last ext  ${${${1##*/}%.*}
TEST="test/src/${NAME}Test.cpp"   # creates test path from NAME
FLAGS="-lpthread -lgtest -lgtest_main -std=c++14 -I include"

if [ -e $TEST ]; then
  clang++ $TEST $FLAGS -o /tmp/test && /tmp/test
else
  echo no test found for ${NAME}.cpp
fi
