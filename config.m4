dnl $Id$
dnl config.m4 for extension lz4

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(lz4, for lz4 support,
dnl Make sure that the comment is aligned:
dnl [  --with-lz4             Include lz4 support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(lz4, whether to enable lz4 support,
dnl Make sure that the comment is aligned:
[  --enable-lz4           Enable lz4 support])

if test "$PHP_LZ4" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-lz4 -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/lz4.h"  # you most likely want to change this
  dnl if test -r $PHP_LZ4/$SEARCH_FOR; then # path given as parameter
  dnl   LZ4_DIR=$PHP_LZ4
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for lz4 files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       LZ4_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$LZ4_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the lz4 distribution])
  dnl fi

  dnl # --with-lz4 -> add include path
  dnl PHP_ADD_INCLUDE($LZ4_DIR/include)

  dnl # --with-lz4 -> check for lib and symbol presence
  dnl LIBNAME=lz4 # you may want to change this
  dnl LIBSYMBOL=lz4 # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $LZ4_DIR/$PHP_LIBDIR, LZ4_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_LZ4LIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong lz4 lib version or lib not found])
  dnl ],[
  dnl   -L$LZ4_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(LZ4_SHARED_LIBADD)

  PHP_NEW_EXTENSION(lz4, lz4.c lz4/lz4.c lz4/lz4lib/lz4.c, $ext_shared)
fi
