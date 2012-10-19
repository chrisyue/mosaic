/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2012 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Chris Yue                                                    |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_mosaic.h"

#include "libqrencode/qrencode.h"
#include "gd/php_gd.h"
#include "gd/libgd/gd.h"

/* If you declare any globals in php_mosaic.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(mosaic)
*/

/* True global resources - no need for thread safety here */
static int le_mosaic;

/* {{{ mosaic_functions[]
 *
 * Every user visible function must have an entry in mosaic_functions[].
 */
const zend_function_entry mosaic_functions[] = {
	PHP_FE(qr_encode, NULL)
	PHP_FE_END	/* Must be the last line in mosaic_functions[] */
};
/* }}} */

#if ZEND_MODULE_API_NO >= 220050617
const zend_module_dep mosaic_deps[] = { /* {{{ */
    zend_module_required("gd")
    { NULL, NULL, NULL }
};
/* }}} */
#endif

/* {{{ mosaic_module_entry
 */
zend_module_entry mosaic_module_entry = {
#if ZEND_MODULE_API_NO >= 220050617
	STANDARD_MODULE_HEADER_EX, NULL,
    mosaic_deps,
#elif ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"mosaic",
	mosaic_functions,
	PHP_MINIT(mosaic),
	PHP_MSHUTDOWN(mosaic),
	PHP_RINIT(mosaic),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(mosaic),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(mosaic),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MOSAIC
ZEND_GET_MODULE(mosaic)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("mosaic.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_mosaic_globals, mosaic_globals)
    STD_PHP_INI_ENTRY("mosaic.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_mosaic_globals, mosaic_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_mosaic_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_mosaic_init_globals(zend_mosaic_globals *mosaic_globals)
{
	mosaic_globals->global_value = 0;
	mosaic_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mosaic)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
    REGISTER_LONG_CONSTANT("QR_ECLEVEL_L", 0, CONST_CS);
    REGISTER_LONG_CONSTANT("QR_ECLEVEL_M", 1, CONST_CS);
    REGISTER_LONG_CONSTANT("QR_ECLEVEL_Q", 2, CONST_CS);
    REGISTER_LONG_CONSTANT("QR_ECLEVEL_H", 3, CONST_CS);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(mosaic)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(mosaic)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(mosaic)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(mosaic)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "mosaic support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

PHP_FUNCTION(qr_encode)
{
    zval* z_raw_p;
    int s = 5; /* module size in pixel */
    int margin = 10; /* width (pixel) */
    int v = 3; /* version */
    int l = 2; /* error correction level */

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|llll", &z_raw_p, &s, &margin, &v, &l) == FAILURE) {
        return ;
    }

    QRecLevel level;

    switch (l) {
        case 1:
            level = QR_ECLEVEL_M;
            break;
        case 2:
            level = QR_ECLEVEL_Q;
            break;
        case 3:
            level = QR_ECLEVEL_H;
            break;
        default:
            level = QR_ECLEVEL_L;
    }

    if (Z_TYPE_P(z_raw_p) == IS_STRING) {
        QRcode* qrcode = QRcode_encodeString8bit(Z_STRVAL_P(z_raw_p), v, level);

        int size = qrcode->width * s + 2 * margin;
        gdImagePtr im = gdImageCreate(size, size);
        int black = gdImageColorAllocate(im, 0, 0, 0);
        int white = gdImageColorAllocate(im, 255, 255, 255);
        gdImageFill(im, 0, 0, white);

        int x, y, posx, posy;
        for (y = 0; y < qrcode->width; y++) {
            for (x = 0; x < qrcode->width; x++) {
                if (*(qrcode->data + y * qrcode->width + x) & 1) {
                    posx = x * s + margin;
                    posy = y * s + margin;
                    gdImageFilledRectangle(im, posx, posy, posx + s, posy + s, black);
                } 
            }
        }
        ZEND_REGISTER_RESOURCE(return_value, im, zend_fetch_list_dtor_id("gd"));
    }
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
