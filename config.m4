dnl $Id$
dnl config.m4 for extension pdoner

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(pdoner, for pdoner support,
dnl Make sure that the comment is aligned:
[  --with-pdoner             Include pdoner support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(pdoner, whether to enable pdoner support,
dnl Make sure that the comment is aligned:
[  --enable-pdoner           Enable pdoner support])

if test "$PHP_PDONER" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-pdoner -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/pdoner.h"  # you most likely want to change this
  dnl if test -r $PHP_PDONER/$SEARCH_FOR; then # path given as parameter
  dnl   PDONER_DIR=$PHP_PDONER
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for pdoner files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PDONER_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PDONER_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the pdoner distribution])
  dnl fi

  dnl # --with-pdoner -> add include path
  dnl PHP_ADD_INCLUDE($PDONER_DIR/include)

  dnl # --with-pdoner -> check for lib and symbol presence
  dnl LIBNAME=pdoner # you may want to change this
  dnl LIBSYMBOL=pdoner # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PDONER_DIR/$PHP_LIBDIR, PDONER_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PDONERLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong pdoner lib version or lib not found])
  dnl ],[
  dnl   -L$PDONER_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PDONER_SHARED_LIBADD)

  PHP_NEW_EXTENSION(pdoner, 
	pdoner.c							\
	, $ext_shared)
fi
