/*
  +----------------------------------------------------------------------+
  | Mosaic                                                               |
  +----------------------------------------------------------------------+
  | Copyright (c) 2012 Chris Yue                                         |
  +----------------------------------------------------------------------+
  | Redistribution and use in source and binary forms, with or without   |
  | modification, are permitted provided that the conditions mentioned   |
  | in the accompanying LICENSE file are met (BSD, revised).             |
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

static zend_class_entry* qr_ce;

/* {{{ mosaic_functions[]
 *
 * Every user visible function must have an entry in mosaic_functions[].
 */
static const zend_function_entry mosaic_functions[] = {
	PHP_FE(qr_encode, NULL)
	PHP_FE_END	/* Must be the last line in mosaic_functions[] */
};
/* }}} */

static const zend_function_entry mosaic_methods[] = {
    PHP_ME(qr, __construct, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, encode,      NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, setText,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, getText,     NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, setModuleSize, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, getModuleSize, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, setMargin,   NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, getMargin,   NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, setVersion,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, getVersion,  NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, setErrorCorrectionLevel, NULL, ZEND_ACC_PUBLIC)
    PHP_ME(qr, getErrorCorrectionLevel, NULL, ZEND_ACC_PUBLIC)
    PHP_FE_END
};

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

#define prop_text "text"
static const int   prop_text_len = sizeof(prop_text) - 1;

#define prop_msize "moduleSize"
static const int   prop_msize_len = sizeof(prop_msize) - 1;

#define prop_margin "margin"
static const int   prop_margin_len = sizeof(prop_margin) - 1;

#define prop_version "version"
static const int   prop_version_len = sizeof(prop_version) - 1;

#define prop_eclevel "errorCorrectionLevel"
static const int   prop_eclevel_len = sizeof(prop_eclevel) - 1;

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(mosaic)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
    REGISTER_LONG_CONSTANT("QR_ECLEVEL_L", 0, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QR_ECLEVEL_M", 1, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QR_ECLEVEL_Q", 2, CONST_CS|CONST_PERSISTENT);
    REGISTER_LONG_CONSTANT("QR_ECLEVEL_H", 3, CONST_CS|CONST_PERSISTENT);

    zend_class_entry ce;
    INIT_CLASS_ENTRY(ce, "Qr", mosaic_methods);
    qr_ce = zend_register_internal_class(&ce TSRMLS_CC);

    zend_declare_property_null(qr_ce, prop_text, prop_text_len, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(qr_ce, prop_msize, prop_msize_len, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(qr_ce, prop_margin, prop_margin_len, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(qr_ce, prop_version, prop_version_len, ZEND_ACC_PROTECTED TSRMLS_CC);
    zend_declare_property_null(qr_ce, prop_eclevel, prop_eclevel_len, ZEND_ACC_PROTECTED TSRMLS_CC);

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


gdImagePtr qr_encode(char* text, int s, int margin, int v, int l)
{
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

    QRcode* qrcode = QRcode_encodeString8bit(text, v, level);

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
    return im;
}

PHP_FUNCTION(qr_encode) /* {{{ */
{
    char* text;
    int text_len;
    int s = 5; /* module size in pixel */
    int margin = 10; /* width (pixel) */
    int v = 3; /* version */
    int l = 2; /* error correction level */

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|llll", &text, &text_len, &s, &margin, &v, &l) == FAILURE) {
        return ;
    }

    gdImagePtr im = qr_encode(text, s, margin, v, l);
    ZEND_REGISTER_RESOURCE(return_value, im, zend_fetch_list_dtor_id("gd"));
}
/* }}} */

PHP_METHOD(qr, __construct)
{
    char* text;
    int text_len;
    int s = 5; /* module size in pixel */
    int margin = 10; /* width (pixel) */
    int v = 3; /* version */
    int l = 2; /* error correction level */

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|llll", &text, &text_len, &s, &margin, &v, &l) == FAILURE) {
        return ;
    }

    zend_update_property_stringl(qr_ce, getThis(), prop_text, prop_text_len, text, text_len TSRMLS_CC);
    zend_update_property_long(qr_ce, getThis(), prop_msize, prop_msize_len, s TSRMLS_CC);
    zend_update_property_long(qr_ce, getThis(), prop_margin, prop_margin_len, margin TSRMLS_CC);
    zend_update_property_long(qr_ce, getThis(), prop_version, prop_version_len, v TSRMLS_CC);
    zend_update_property_long(qr_ce, getThis(), prop_eclevel, prop_eclevel_len, l TSRMLS_CC);
}

PHP_METHOD(qr, setText)
{
    char* text;
    int text_len;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &text, &text_len) == FAILURE) {
        return ;
    }
    zend_update_property_stringl(qr_ce, getThis(), prop_text, prop_text_len, text, text_len TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(qr, getText)
{
    RETURN_STRING(Z_STRVAL_P(zend_read_property(qr_ce, getThis(), prop_text, prop_text_len, 0 TSRMLS_CC)), 1);
}

PHP_METHOD(qr, setModuleSize)
{
    int size;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &size) == FAILURE) {
        return ;
    }
    zend_update_property_long(qr_ce, getThis(), prop_msize, prop_msize_len, size TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(qr, getModuleSize)
{
    RETURN_LONG(Z_LVAL_P(zend_read_property(qr_ce, getThis(), prop_msize, prop_msize_len, 0 TSRMLS_CC)));
}

PHP_METHOD(qr, setMargin)
{
    int margin;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &margin) == FAILURE) {
        return ;
    }
    zend_update_property_long(qr_ce, getThis(), prop_margin, prop_margin_len, margin TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(qr, getMargin)
{
    RETURN_LONG(Z_LVAL_P(zend_read_property(qr_ce, getThis(), prop_margin, prop_margin_len, 0 TSRMLS_CC)));
}

PHP_METHOD(qr, setVersion)
{
    int version;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &version) == FAILURE) {
        return ;
    }
    zend_update_property_long(qr_ce, getThis(), prop_version, prop_version_len, version TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(qr, getVersion)
{
    RETURN_LONG(Z_LVAL_P(zend_read_property(qr_ce, getThis(), prop_version, prop_version_len, 0 TSRMLS_CC)));
}

PHP_METHOD(qr, setErrorCorrectionLevel)
{
    int eclevel;

    if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &eclevel) == FAILURE) {
        return ;
    }
    zend_update_property_long(qr_ce, getThis(), prop_eclevel, prop_eclevel_len, eclevel TSRMLS_CC);
    RETURN_ZVAL(getThis(), 1, 0);
}

PHP_METHOD(qr, getErrorCorrectionLevel)
{
    RETURN_LONG(Z_LVAL_P(zend_read_property(qr_ce, getThis(), prop_eclevel, prop_eclevel_len, 0 TSRMLS_CC)));
}
    
PHP_METHOD(qr, encode)
{
    gdImagePtr im = qr_encode(Z_STRVAL_P(zend_read_property(qr_ce, getThis(), prop_text, prop_text_len, 0 TSRMLS_CC)), 
            Z_LVAL_P(zend_read_property(qr_ce, getThis(), prop_msize, prop_msize_len, 0 TSRMLS_CC)), 
            Z_LVAL_P(zend_read_property(qr_ce, getThis(), prop_margin, prop_margin_len, 0 TSRMLS_CC)),
            Z_LVAL_P(zend_read_property(qr_ce, getThis(), prop_version, prop_version_len, 0 TSRMLS_CC)),
            Z_LVAL_P(zend_read_property(qr_ce, getThis(), prop_eclevel, prop_eclevel_len, 0 TSRMLS_CC)));
    ZEND_REGISTER_RESOURCE(return_value, im, zend_fetch_list_dtor_id("gd"));
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
