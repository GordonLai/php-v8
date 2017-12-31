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

#include "php_v8_exceptions.h"
#include "php_v8_try_catch.h"
#include "php_v8_value.h"
#include "php_v8.h"

zend_class_entry* php_v8_generic_exception_class_entry;
zend_class_entry* php_v8_try_catch_exception_class_entry;
zend_class_entry* php_v8_termination_exception_class_entry;
zend_class_entry* php_v8_resource_limit_exception_class_entry;
zend_class_entry* php_v8_time_limit_exception_class_entry;
zend_class_entry* php_v8_memory_limit_exception_class_entry;

zend_class_entry* php_v8_value_exception_class_entry;

void php_v8_create_try_catch_exception(zval *return_value, php_v8_isolate_t *php_v8_isolate, php_v8_context_t *php_v8_context, v8::TryCatch *try_catch);

void php_v8_throw_try_catch_exception(php_v8_isolate_t *php_v8_isolate, php_v8_context_t *php_v8_context, v8::TryCatch *try_catch) {
    zval exception_zv;

    php_v8_create_try_catch_exception(&exception_zv, php_v8_isolate, php_v8_context, try_catch);

    zend_throw_exception_object(&exception_zv);
}

void php_v8_throw_try_catch_exception(php_v8_context_t *php_v8_context, v8::TryCatch *try_catch) {
    php_v8_throw_try_catch_exception(php_v8_context->php_v8_isolate, php_v8_context, try_catch);
}

void php_v8_create_try_catch_exception(zval *return_value, php_v8_isolate_t *php_v8_isolate, php_v8_context_t *php_v8_context, v8::TryCatch *try_catch)
{
    zval isolate_zv;
    zval context_zv;

    zval try_catch_zv;
    zend_class_entry* ce = NULL;
    const char *message = NULL;

    PHP_V8_DECLARE_LIMITS(php_v8_isolate);
    PHP_V8_DECLARE_ISOLATE(php_v8_isolate);

    if ((try_catch == NULL) || (try_catch->Exception()->IsNull() && try_catch->Message().IsEmpty() && !try_catch->CanContinue() && try_catch->HasTerminated())) {
        if (limits->time_limit_hit) {
            ce = php_v8_time_limit_exception_class_entry;
            message = "Time limit exceeded";
        } else if (limits->memory_limit_hit) {
            ce = php_v8_memory_limit_exception_class_entry;
            message = "Memory limit exceeded";
        } else {
            ce = php_v8_termination_exception_class_entry;
            message = "Execution terminated";
        }

        object_init_ex(return_value, ce);
        zend_update_property_string(php_v8_try_catch_exception_class_entry, return_value, ZEND_STRL("message"), message);
    } else {
        ce = php_v8_try_catch_exception_class_entry;

        PHP_V8_CONVERT_FROM_V8_STRING_TO_STRING_NODECL(isolate, message, try_catch->Exception());

        object_init_ex(return_value, ce);
        zend_update_property_string(php_v8_try_catch_exception_class_entry, return_value, ZEND_STRL("message"), message);
    }

    ZVAL_OBJ(&isolate_zv, &php_v8_isolate->std);
    ZVAL_OBJ(&context_zv, &php_v8_context->std);

    PHP_V8_TRY_CATCH_EXCEPTION_STORE_ISOLATE(return_value, &isolate_zv);
    PHP_V8_TRY_CATCH_EXCEPTION_STORE_CONTEXT(return_value, &context_zv);

    php_v8_try_catch_create_from_try_catch(&try_catch_zv, php_v8_isolate, php_v8_context, try_catch);
    PHP_V8_TRY_CATCH_EXCEPTION_STORE_TRY_CATCH(return_value, &try_catch_zv);

    zval_ptr_dtor(&try_catch_zv);
}


static PHP_METHOD(ExceptionsTryCatch, __construct)
{
    zval rv;

    zval *isolate_zv = NULL;
    zval *context_zv = NULL;
    zval *try_catch_zv = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "ooo", &isolate_zv, &context_zv, &try_catch_zv) == FAILURE) {
        return;
    }

    PHP_V8_ISOLATE_FETCH_WITH_CHECK(isolate_zv, php_v8_isolate);
    PHP_V8_CONTEXT_FETCH_WITH_CHECK(context_zv, php_v8_context);

    PHP_V8_DATA_ISOLATES_CHECK_USING(php_v8_context, php_v8_isolate);

    PHP_V8_ISOLATE_FETCH_WITH_CHECK(PHP_V8_TRY_CATCH_READ_ISOLATE(try_catch_zv), php_v8_try_catch_isolate);
    // this is redundant, we do check in TryCatch constructor
    //PHP_V8_CONTEXT_FETCH_WITH_CHECK(PHP_V8_TRY_CATCH_READ_CONTEXT(try_catch_zv), php_v8_try_catch_context);

    PHP_V8_ISOLATES_CHECK(php_v8_try_catch_isolate, php_v8_isolate);
    // this is redundant, we do check in TryCatch constructor
    //PHP_V8_DATA_ISOLATES_CHECK_USING(php_v8_try_catch_context, php_v8_isolate); // thi

    PHP_V8_TRY_CATCH_EXCEPTION_STORE_ISOLATE(getThis(), isolate_zv);
    PHP_V8_TRY_CATCH_EXCEPTION_STORE_CONTEXT(getThis(), context_zv);
    PHP_V8_TRY_CATCH_EXCEPTION_STORE_TRY_CATCH(getThis(), try_catch_zv);
}

static PHP_METHOD(ExceptionsTryCatch, getIsolate)
{
    zval rv;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    RETVAL_ZVAL(PHP_V8_TRY_CATCH_EXCEPTION_READ_ISOLATE(getThis()), 1, 0);
}

static PHP_METHOD(ExceptionsTryCatch, getContext)
{
    zval rv;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    RETVAL_ZVAL(PHP_V8_TRY_CATCH_EXCEPTION_READ_CONTEXT(getThis()), 1, 0);
}

static PHP_METHOD(ExceptionsTryCatch, getTryCatch)
{
    zval rv;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    RETVAL_ZVAL(PHP_V8_TRY_CATCH_EXCEPTION_READ_TRY_CATCH(getThis()), 1, 0);
}


PHP_V8_ZEND_BEGIN_ARG_WITH_CONSTRUCTOR_INFO_EX(arginfo___construct, 3)
    ZEND_ARG_OBJ_INFO(0, isolate, V8\\Isolate, 0)
    ZEND_ARG_OBJ_INFO(0, context, V8\\Context, 0)
    ZEND_ARG_OBJ_INFO(0, try_catch, V8\\TryCatch, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_getIsolate, ZEND_RETURN_VALUE, 0, V8\\Isolate, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_getContext, ZEND_RETURN_VALUE, 0, V8\\Context, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_getTryCatch, ZEND_RETURN_VALUE, 0, V8\\TryCatch, 0)
ZEND_END_ARG_INFO()



static const zend_function_entry php_v8_exception_methods[] = {
        PHP_FE_END
};

static const zend_function_entry php_v8_try_catch_exception_methods[] = {
        PHP_V8_ME(ExceptionsTryCatch, __construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
        PHP_V8_ME(ExceptionsTryCatch, getIsolate,  ZEND_ACC_PUBLIC)
        PHP_V8_ME(ExceptionsTryCatch, getContext,  ZEND_ACC_PUBLIC)
        PHP_V8_ME(ExceptionsTryCatch, getTryCatch, ZEND_ACC_PUBLIC)

        PHP_FE_END
};


static const zend_function_entry php_v8_termination_exception_methods[] = {
        PHP_FE_END
};

static const zend_function_entry php_v8_resource_limit_exception_methods[] = {
        PHP_FE_END
};

static const zend_function_entry php_v8_time_limit_exception_methods[] = {
        PHP_FE_END
};

static const zend_function_entry php_v8_memory_limit_exception_methods[] = {
        PHP_FE_END
};

static const zend_function_entry php_v8_value_exception_methods[] = {
        PHP_FE_END
};


PHP_MINIT_FUNCTION(php_v8_exceptions) {
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, "V8\\Exceptions", "Exception", php_v8_exception_methods);
    php_v8_generic_exception_class_entry = zend_register_internal_class_ex(&ce, zend_exception_get_default());

    INIT_NS_CLASS_ENTRY(ce, "V8\\Exceptions", "TryCatchException", php_v8_try_catch_exception_methods);
    php_v8_try_catch_exception_class_entry = zend_register_internal_class_ex(&ce, php_v8_generic_exception_class_entry);

    zend_declare_property_null(php_v8_try_catch_exception_class_entry, ZEND_STRL("isolate"),   ZEND_ACC_PRIVATE);
    zend_declare_property_null(php_v8_try_catch_exception_class_entry, ZEND_STRL("context"),   ZEND_ACC_PRIVATE);
    zend_declare_property_null(php_v8_try_catch_exception_class_entry, ZEND_STRL("try_catch"), ZEND_ACC_PRIVATE);


    INIT_NS_CLASS_ENTRY(ce, "V8\\Exceptions", "TerminationException", php_v8_termination_exception_methods);
    php_v8_termination_exception_class_entry = zend_register_internal_class_ex(&ce, php_v8_try_catch_exception_class_entry);

    INIT_NS_CLASS_ENTRY(ce, "V8\\Exceptions", "ResourceLimitException", php_v8_resource_limit_exception_methods);
    php_v8_resource_limit_exception_class_entry = zend_register_internal_class_ex(&ce, php_v8_termination_exception_class_entry);

    INIT_NS_CLASS_ENTRY(ce, "V8\\Exceptions", "TimeLimitException", php_v8_time_limit_exception_methods);
    php_v8_time_limit_exception_class_entry = zend_register_internal_class_ex(&ce, php_v8_resource_limit_exception_class_entry);

    INIT_NS_CLASS_ENTRY(ce, "V8\\Exceptions", "MemoryLimitException", php_v8_memory_limit_exception_methods);
    php_v8_memory_limit_exception_class_entry = zend_register_internal_class_ex(&ce, php_v8_resource_limit_exception_class_entry);

    INIT_NS_CLASS_ENTRY(ce, "V8\\Exceptions", "ValueException", php_v8_value_exception_methods);
    php_v8_value_exception_class_entry = zend_register_internal_class_ex(&ce, php_v8_generic_exception_class_entry);

    return SUCCESS;
}
