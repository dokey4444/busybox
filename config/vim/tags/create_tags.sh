#!/bin/bash

rm -f *tags

CTAGS='ctags'
# ${CTAGS} -f phptags ./php_funclist

CTAGS_CPP=${CTAGS}' --c++-kinds=+p --fields=+iaS --extra=+q+f --tag-relative=yes --language-force=C++'
${CTAGS_CPP} -f cpptags -R /usr/include/c++
${CTAGS_CPP} -f cppexttags -R ~/.vim/cpp_src

CTAGS_C=${CTAGS}' -I __THROW -I __attribute_pure__ -I __nonnull -I __attribute__ -I __restrict=restrict -I  __const=const -I ((1, -I 3))'
${CTAGS_C} -f ctags /usr/include/*.h
${CTAGS_C} -f gnutags -R /usr/include/x86_64-linux-gnu
${CTAGS_C} -f kerneltags -R /usr/include/linux
