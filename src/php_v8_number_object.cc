/*
 * This file is part of the pinepain/php-v8 PHP extension.
 *
 * Copyright (c) 2015-2018 Bogdan Padalko <pinepain@gmail.com>
 *
 * Licensed under the MIT license: http://opensource.org/licenses/MIT
 *
 * For the full copyright and license information, please view the
 * LICENSE file that was distributed with this source or visit
 * http://opensource.org/licenses/MIT
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_v8_number_object.h"
#include "php_v8_value.h"
#include "php_v8_context.h"
#include "php_v8_object.h"
#include "php_v8.h"

zend_class_entry *php_v8_number_object_class_entry;
#define this_ce php_v8_number_object_class_entry


static PHP_METHOD(NumberObject, __construct) {
    zval rv;
    zval *php_v8_context_zv;

    double value;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "od", &php_v8_context_zv, &value) == FAILURE) {
        return;
    }

    PHP_V8_OBJECT_CONSTRUCT(getThis(), php_v8_context_zv, php_v8_context, php_v8_value);

    v8::Local<v8::NumberObject> local_number_obj = v8::NumberObject::New(isolate, value).As<v8::NumberObject>();

    PHP_V8_THROW_VALUE_EXCEPTION_WHEN_EMPTY(local_number_obj, "Failed to create NumberObject value");

    php_v8_object_store_self_ptr(php_v8_value, local_number_obj);

    php_v8_value->persistent->Reset(isolate, local_number_obj);
}

static PHP_METHOD(NumberObject, valueOf) {
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_VALUE_FETCH_WITH_CHECK(getThis(), php_v8_value);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_value);
    //PHP_V8_ENTER_STORED_CONTEXT(php_v8_value);

    RETURN_DOUBLE(php_v8_value_get_local_as<v8::NumberObject>(php_v8_value)->ValueOf());
}


PHP_V8_ZEND_BEGIN_ARG_WITH_CONSTRUCTOR_INFO_EX(arginfo___construct, 2)
                ZEND_ARG_OBJ_INFO(0, context, V8\\Context, 0)
                ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_valueOf, ZEND_RETURN_VALUE, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()


static const zend_function_entry php_v8_number_object_methods[] = {
        PHP_V8_ME(NumberObject, __construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        PHP_V8_ME(NumberObject, valueOf,     ZEND_ACC_PUBLIC)

        PHP_FE_END
};



PHP_MINIT_FUNCTION(php_v8_number_object) {
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, PHP_V8_NS, "NumberObject", php_v8_number_object_methods);
    this_ce = zend_register_internal_class_ex(&ce, php_v8_object_class_entry);

    return SUCCESS;
}
