/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wei Chen <farwish@foxmail.com>                               |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"

#include "php_pdoner.h"
#include "pdoner_conf.h"


static int le_pdoner;


zend_class_entry *errs_ce;

/* {{{ ARG_INFO
*/
ZEND_BEGIN_ARG_INFO_EX(errs_get_arginfo, 0, 0, 0)
	ZEND_ARG_INFO(0, code)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(errs_set_arginfo, 0, 0, 2)
	ZEND_ARG_INFO(0, code)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
/* }}} */

/* {{{ proto int pd_random_id([int $salt = 0])
   a random id based unix timestamp */
PHP_FUNCTION(pd_random_id)
{
	time_t t;
	long *salt = 0;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &salt) == FAILURE) {
		return;
	}

	t = (long)time(NULL) + (long)salt;

	RETURN_LONG(t);
}
/* }}} */

/* {{{ proto string pd_implode_json(array $arr [, string $glue = ','])
* TODO malloc and strcat optimize.
*/
PHP_FUNCTION(pd_implode_json)
{
	zval *pieces, *glue = NULL;
	zval function_name, *params[2] = {0};
	zval *retval = NULL;

	if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|z", &pieces, &glue) == FAILURE ) {
		RETURN_FALSE;
	}

	ZVAL_STRING(&function_name, "implode", 0);

	params[0] = pieces;

	if (glue == NULL) {
		// ZVAL_STRING() used when glue was a zval, not zval *	\
			so under line will cause segmentfalut.
		// ZVAL_STRING(glue, ",", 1);

		MAKE_STD_ZVAL(glue);
		Z_STRVAL_P(glue) = ",";
		Z_STRLEN_P(glue) = strlen(",");
		Z_TYPE_P(glue) = IS_STRING;
	}

	params[1] = glue;

	if ( call_user_function(EG(function_table), NULL, &function_name, return_value, 2, params TSRMLS_CC) == FAILURE ) {
		if (return_value) {
			zval_dtor(return_value);
		}
		RETURN_FALSE;
	}

	/*
	// this way will segmentfalut when pass the second param.
	if (glue != NULL) {
		ZVAL_NULL(glue);
		zval_ptr_dtor(&glue);
	}
	*/

	// when passed the second param glue, if you forget dtor, segmentfault.
	if (ZEND_NUM_ARGS() == 1) {
		ZVAL_NULL(glue);
		zval_ptr_dtor(&glue);
	}
	
	zval op1, op2;
	ZVAL_STRINGL(&op1, "[", 1, 1);
	ZVAL_STRINGL(&op2, "]", 1, 1);

	/*
	// concat; way1
	zval result;

	concat_function(&result, &op1, return_value TSRMLS_CC);
	concat_function(&result, &result, &op2 TSRMLS_CC);

 	// 1. zval_ptr_dtor(&return_value) was wrong.
	// 2. forget zval_dtor(return_value) will cause memory leaks.
	zval_dtor(return_value);

	// copy result to return_value;
	// if "zval result" is not zero-terminated, use ZVAL_ZVAL() instead.	\
		PHP Warning:  String is not zero-terminated.
	ZVAL_COPY_VALUE(return_value, &result);
	*/
	
	// concat; way2
	concat_function(&op1, &op1, return_value TSRMLS_CC);
	concat_function(&op1, &op1, &op2 TSRMLS_CC);
	zval_dtor(return_value);
	ZVAL_ZVAL(return_value, &op1, 0, 1);

	zval_dtor(&op1);
	zval_dtor(&op2);
}
/* }}} */

/* {{{ proto public Errs::__construct(void) */
PHP_METHOD(errs, __construct)
{
	zval *msg;
	MAKE_STD_ZVAL(msg);	
	array_init(msg);

	add_index_string(msg, PDONER_ERRS_SUCC, "成功", 1);
	add_index_string(msg, PDONER_ERRS_FAIL, "失败", 1);
	add_index_string(msg, PDONER_ERRS_EXCEP, "异常", 1);
	add_index_string(msg, PDONER_ERRS_UNKNOW, "未知", 1);

	zend_update_static_property(errs_ce, ZEND_STRL(PDONER_ERRS_PROPERTY_NAME_MSG), msg TSRMLS_CC);

	zval_ptr_dtor(&msg);
}
/* }}} */

/* {{{ proto public static Errs::get([int $code])
*/
PHP_METHOD(errs, get)
{
	zval code;
	zval *msg = NULL;

	if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "|l", &code) == FAILURE ) {
		return;
	}

	// read property
	msg = zend_read_static_property(errs_ce, ZEND_STRL(PDONER_ERRS_PROPERTY_NAME_MSG), 0 TSRMLS_CC);

	// no instance
	if (Z_ARRVAL_P(msg) == NULL) {
		zend_error(E_WARNING, "please new Errs instance before use, for property will initialized in construct!\n");
		RETURN_NULL();
	}

	// no param, return all
	if (ZEND_NUM_ARGS() == 0) {
		RETURN_ZVAL(msg, 0, 1);
	}

	// foreach to get value
	HashTable *hTable = Z_ARRVAL_P(msg);
	zval **pData;

	char *key;
	uint key_len;
	ulong idx;

	while ( zend_hash_get_current_data(hTable, (void **) &pData) == SUCCESS ) {
		zend_hash_get_current_key_ex(hTable, &key, &key_len, &idx, 0, NULL);

		if (idx == Z_LVAL(code)) {
			RETURN_STRING(Z_STRVAL_PP(pData), 1);
			break;
		}
		zend_hash_move_forward(hTable);
	}

	RETURN_NULL();

	/*
	for (zend_hash_internal_pointer_reset(hTable);
		 zend_hash_has_more_elements(hTable) == SUCCESS;
		zend_hash_move_forward(hTable)) {
		if ( zend_hash_get_current_data(hTable, (void **) &pData) == FAILURE ) {
			continue;
		}

		switch (zend_hash_get_current_key_ex(hTable, &key, &key_len, &idx, 0, NULL));

		if (idx == Z_LVAL(code)) {
			php_printf("%s\n", Z_STRVAL_PP(pData));
		}
	}
	*/
}
/* }}} */

/* {{{ proto public static Errs::set(int $code, string $value) 
*/
PHP_METHOD(errs, set)
{
	ulong code;
	char *value;
	uint value_len;

	if ( zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &code, &value, &value_len) == FAILURE ) {
		return;
	}

	// read property
    zval *msg = zend_read_static_property(errs_ce, ZEND_STRL(PDONER_ERRS_PROPERTY_NAME_MSG), 0 TSRMLS_CC);

	// if no instance
	HashTable *hTable = Z_ARRVAL_P(msg);

	if (hTable == NULL) {
		zend_error(E_WARNING, "please new Errs instance before use, for property will initialized in construct!\n");
		RETURN_FALSE;
	}

	// check is index exists
	// int zend_hash_exists(const HashTable *ht, const char *arKey, uint nKeyLength)
	if ( zend_hash_index_exists(hTable, code) ) {
		zend_error(E_WARNING, "the code has exists in using %s::%s()!\n",	\
			get_active_class_name(NULL TSRMLS_CC),	\
			get_active_function_name(TSRMLS_C));	\
		RETURN_FALSE;
	}

	// add value
	add_index_stringl(msg, code, value, value_len, 1);

	// update $msg
	if ( zend_update_static_property(errs_ce, ZEND_STRL(PDONER_ERRS_PROPERTY_NAME_MSG), msg TSRMLS_CC) == SUCCESS ) {
		RETURN_TRUE;	
	}

	RETURN_FALSE;
}
/* }}} */

/* {{{ Errs_methods */
zend_function_entry errs_methods[] = {
	PHP_ME(errs, __construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(errs, get, errs_get_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(errs, set, errs_set_arginfo, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
*/
PHP_MINIT_FUNCTION(pdoner)
{
	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "errs", errs_methods);	
	errs_ce = zend_register_internal_class(&ce TSRMLS_CC);

	zend_declare_class_constant_long(errs_ce, ZEND_STRL(PDONER_ERRS_CONSTANT_NAME_SUCC), PDONER_ERRS_SUCC TSRMLS_CC);
	zend_declare_class_constant_long(errs_ce, ZEND_STRL(PDONER_ERRS_CONSTANT_NAME_FAIL), PDONER_ERRS_FAIL TSRMLS_CC);
	zend_declare_class_constant_long(errs_ce, ZEND_STRL(PDONER_ERRS_CONSTANT_NAME_EXCEP), PDONER_ERRS_EXCEP TSRMLS_CC);
	zend_declare_class_constant_long(errs_ce, ZEND_STRL(PDONER_ERRS_CONSTANT_NAME_UNKNOW), PDONER_ERRS_UNKNOW TSRMLS_CC);

	zend_declare_property_null(errs_ce, ZEND_STRL(PDONER_ERRS_PROPERTY_NAME_MSG), ZEND_ACC_PUBLIC|ZEND_ACC_STATIC TSRMLS_CC);

	/* global const */
	REGISTER_LONG_CONSTANT("PD_ONE_MINUTE", PD_ONE_MINUTE, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PD_ONE_HOUR", PD_ONE_HOUR, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PD_BY_DAY", PD_BY_DAY, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PD_ONE_DAY", PD_ONE_DAY, CONST_CS | CONST_PERSISTENT);

	/* startup component */
	PDONER_STARTUP(conf);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
*/
PHP_MSHUTDOWN_FUNCTION(pdoner)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RINIT_FUNCTION
*/
PHP_RINIT_FUNCTION(pdoner)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_RSHUTDOWN_FUNCTION
*/
PHP_RSHUTDOWN_FUNCTION(pdoner)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(pdoner)
{
	php_info_print_table_start();
	php_info_print_table_row(2, "pdoner support", "enabled");
	php_info_print_table_row(2, "pdoner version", PHP_PDONER_VERSION);
	php_info_print_table_end();

}
/* }}} */

/* {{{ pdoner_functions
*/
const zend_function_entry pdoner_functions[] = {
	PHP_FE(pd_random_id, NULL)
	PHP_FE(pd_implode_json, NULL)
	PHP_FE_END
};
/* }}} */

/* {{{ pdoner_module_entry
*/
zend_module_entry pdoner_module_entry = {
	STANDARD_MODULE_HEADER,
	"pdoner",
	pdoner_functions,
	PHP_MINIT(pdoner),
	PHP_MSHUTDOWN(pdoner),
	PHP_RINIT(pdoner),	
	PHP_RSHUTDOWN(pdoner),
	PHP_MINFO(pdoner),
	PHP_PDONER_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PDONER
ZEND_GET_MODULE(pdoner)
#endif

