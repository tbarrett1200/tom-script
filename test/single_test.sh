NAME=`basename $1 .cpp | cut -f 1 -d '.'` # removes all dirs and last ext  ${${${1##*/}%.*}

if [ -e "test/test" ]; then
  make test
fi

test/test $NAME
