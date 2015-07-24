/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2014 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_lz4.h"
#include "lz4/lz4.h"

/* If you declare any globals in php_lz4.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(lz4)
*/

/* True global resources - no need for thread safety here */
static int le_lz4;

/* {{{ lz4_functions[]
 *
 * Every user visible function must have an entry in lz4_functions[].
 */
const zend_function_entry lz4_functions[] = {
	PHP_FE(confirm_lz4_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(lz4_compress, 	NULL)
	PHP_FE(lz4_uncompress, 	NULL)
	PHP_FE_END	/* Must be the last line in lz4_functions[] */
};
/* }}} */

/* {{{ lz4_module_entry
 */
zend_module_entry lz4_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"lz4",
	lz4_functions,
	PHP_MINIT(lz4),
	PHP_MSHUTDOWN(lz4),
	PHP_RINIT(lz4),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(lz4),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(lz4),
#if ZEND_MODULE_API_NO >= 20010901
	PHP_LZ4_VERSION,
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_LZ4
ZEND_GET_MODULE(lz4)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("lz4.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_lz4_globals, lz4_globals)
    STD_PHP_INI_ENTRY("lz4.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_lz4_globals, lz4_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_lz4_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_lz4_init_globals(zend_lz4_globals *lz4_globals)
{
	lz4_globals->global_value = 0;
	lz4_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(lz4)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(lz4)
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
PHP_RINIT_FUNCTION(lz4)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(lz4)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(lz4)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "lz4 support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_lz4_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_lz4_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "lz4", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */

/* {{{ lz4_compress */
PHP_FUNCTION(lz4_compress)
{
	char *source_str = NULL, *dest_str = NULL;
	long source_str_len = 0, dest_str_len = 0, max_dest_str_len = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source_str, &source_str_len) == FAILURE) {
		RETURN_FALSE;
	}

	max_dest_str_len = lz4_max_compressed_length(source_str_len);
	dest_str_len = max_dest_str_len;
	dest_str = (char *)emalloc(max_dest_str_len * sizeof(char));
	if (!dest_str) {
		zend_error(E_WARNING, "lz4_compress : emalloc memory error.");
		RETURN_FALSE;
	}
	if (lz4_compress(source_str, source_str_len, dest_str, &dest_str_len) == LZ4_OK) {
		RETVAL_STRINGL(dest_str, dest_str_len, 1);
	} else {
		RETURN_FALSE;
	}
	efree(dest_str);
}
/* }}} */

/* {{{ lz4_uncompress */
PHP_FUNCTION(lz4_uncompress)
{
	char *source_str = NULL, *dest_str = NULL;
	long source_str_len = 0, dest_str_len = 0, max_uncompress_len = 0;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &source_str, &source_str_len) == FAILURE) {
		RETURN_FALSE;
	}

	if (lz4_uncompressed_length(source_str, source_str_len, &max_uncompress_len) != LZ4_OK) {
		zend_error(E_WARNING, "lz4_uncompress : dest_str length error.");
		RETURN_FALSE;
	}
	dest_str_len = max_uncompress_len;
	dest_str = (char *)emalloc(max_uncompress_len * sizeof(char));
	if (!dest_str) {
		zend_error(E_WARNING, "lz4_uncompress : emalloc memory error.");
		RETURN_FALSE;
	}
	if (lz4_uncompress(source_str, source_str_len, dest_str, &dest_str_len) == LZ4_OK) {
		RETVAL_STRINGL(dest_str, dest_str_len, 1);
	} else {
		RETURN_FALSE;
	}
	efree(dest_str);
}
/* }}} */

/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
