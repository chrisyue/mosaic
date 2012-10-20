dnl $Id$
dnl config.m4 for extension mosaic

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(mosaic, for mosaic support,
dnl Make sure that the comment is aligned:
dnl [  --with-mosaic             Include mosaic support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(mosaic, whether to enable mosaic support,
dnl Make sure that the comment is aligned:
[  --enable-mosaic           Enable mosaic support])

if test "$PHP_MOSAIC" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-mosaic -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/mosaic.h"  # you most likely want to change this
  dnl if test -r $PHP_MOSAIC/$SEARCH_FOR; then # path given as parameter
  dnl   MOSAIC_DIR=$PHP_MOSAIC
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for mosaic files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       MOSAIC_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$MOSAIC_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the mosaic distribution])
  dnl fi

  dnl # --with-mosaic -> add include path
  dnl PHP_ADD_INCLUDE($MOSAIC_DIR/include)

  dnl # --with-mosaic -> check for lib and symbol presence
  dnl LIBNAME=mosaic # you may want to change this
  dnl LIBSYMBOL=mosaic # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $MOSAIC_DIR/lib, MOSAIC_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_MOSAICLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong mosaic lib version or lib not found])
  dnl ],[
  dnl   -L$MOSAIC_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(MOSAIC_SHARED_LIBADD)

  ifdef([PHP_ADD_EXTENSION_DEP], [PHP_ADD_EXTENSION_DEP(gd)])

  PHP_NEW_EXTENSION(mosaic, mosaic.c\
          libqrencode/qrencode.c\
          libqrencode/qrinput.c\
          libqrencode/qrspec.c\
          libqrencode/bitstream.c\
          libqrencode/rscode.c\
          libqrencode/mask.c, $ext_shared)
fi
