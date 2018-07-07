/*
 * This file is part of the phpv8/php-v8 PHP extension.
 *
 * Copyright (c) 2015-2018 Bogdan Padalko <thepinepain@gmail.com>
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

#include "php_v8_unbound_script.h"
#include "php_v8_script.h"
#include "php_v8_string.h"
#include "php_v8_value.h"
#include "php_v8_context.h"
#include "php_v8.h"

zend_class_entry* php_v8_unbound_script_class_entry;
#define this_ce php_v8_unbound_script_class_entry

static zend_object_handlers php_v8_unbound_script_object_handlers;


php_v8_unbound_script_t * php_v8_create_unbound_script(zval *return_value, php_v8_isolate_t *php_v8_isolate, v8::Local<v8::UnboundScript> unbound_script) {
    zval isolate_zv;
    assert(!unbound_script.IsEmpty());

    object_init_ex(return_value, this_ce);

    ZVAL_OBJ(&isolate_zv, &php_v8_isolate->std);

    PHP_V8_FETCH_UNBOUND_SCRIPT_INTO(return_value, php_v8_unbound_script);
    PHP_V8_UNBOUND_SCRIPT_STORE_ISOLATE(return_value, &isolate_zv)
    PHP_V8_STORE_POINTER_TO_ISOLATE(php_v8_unbound_script, php_v8_isolate);

    php_v8_unbound_script->persistent->Reset(php_v8_isolate->isolate, unbound_script);

    return php_v8_unbound_script;
}


static void php_v8_unbound_script_free(zend_object *object)
{
    php_v8_unbound_script_t *php_v8_unbound_script = php_v8_unbound_script_fetch_object(object);

    if (php_v8_unbound_script->persistent) {
        if (PHP_V8_IS_UP_AND_RUNNING() && PHP_V8_ISOLATE_HAS_VALID_HANDLE(php_v8_unbound_script)) {
            php_v8_unbound_script->persistent->Reset();
        }
        delete php_v8_unbound_script->persistent;
    }

    zend_object_std_dtor(&php_v8_unbound_script->std);
}

static zend_object * php_v8_unbound_script_ctor(zend_class_entry *ce)
{
    php_v8_unbound_script_t *php_v8_unbound_script;

    php_v8_unbound_script = (php_v8_unbound_script_t *) ecalloc(1, sizeof(php_v8_unbound_script_t) + zend_object_properties_size(ce));

    zend_object_std_init(&php_v8_unbound_script->std, ce);
    object_properties_init(&php_v8_unbound_script->std, ce);

    php_v8_unbound_script->persistent = new v8::Persistent<v8::UnboundScript>();

    php_v8_unbound_script->std.handlers = &php_v8_unbound_script_object_handlers;

    return &php_v8_unbound_script->std;
}

static PHP_METHOD(UnboundScript, __construct)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_THROW_EXCEPTION("V8\\UnboundScript::__construct() should not be called. Use other methods which yield V8\\UnboundScript object.")
}

static PHP_METHOD(UnboundScript, getIsolate)
{
    zval rv;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_FETCH_UNBOUND_SCRIPT_WITH_CHECK(getThis(), php_v8_unbound_script);

    RETVAL_ZVAL(PHP_V8_UNBOUND_SCRIPT_READ_ISOLATE(getThis()), 1, 0);
}

static PHP_METHOD(UnboundScript, bindToContext)
{
    zval *php_v8_context_zv;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "o", &php_v8_context_zv) == FAILURE) {
        return;
    }

    PHP_V8_FETCH_UNBOUND_SCRIPT_WITH_CHECK(getThis(), php_v8_unbound_script);
    PHP_V8_CONTEXT_FETCH_WITH_CHECK(php_v8_context_zv, php_v8_context);

    PHP_V8_DATA_ISOLATES_CHECK(php_v8_unbound_script, php_v8_context);

    PHP_V8_ENTER_STORED_ISOLATE(php_v8_context);
    PHP_V8_ENTER_CONTEXT(php_v8_context);

    v8::Local<v8::UnboundScript> local_unbound_script = php_v8_unbound_script_get_local(php_v8_unbound_script);
    v8::Local<v8::Script> local_script = local_unbound_script->BindToCurrentContext();

    php_v8_create_script(return_value, local_script, php_v8_context);
}

static PHP_METHOD(UnboundScript, getId)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_FETCH_UNBOUND_SCRIPT_WITH_CHECK(getThis(), php_v8_unbound_script);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_unbound_script);

    v8::Local<v8::UnboundScript> local_unbound_script = php_v8_unbound_script_get_local(php_v8_unbound_script);

    if (v8::UnboundScript::kNoScriptId == local_unbound_script->GetId()) {
        RETURN_NULL();
    }

    RETURN_LONG(static_cast<zend_long>(local_unbound_script->GetId()));
}

static PHP_METHOD(UnboundScript, getScriptName)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_FETCH_UNBOUND_SCRIPT_WITH_CHECK(getThis(), php_v8_unbound_script);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_unbound_script);

    v8::Local<v8::UnboundScript> local_unbound_script = php_v8_unbound_script_get_local(php_v8_unbound_script);

    php_v8_get_or_create_value(return_value, local_unbound_script->GetScriptName(), php_v8_unbound_script->php_v8_isolate);
}

static PHP_METHOD(UnboundScript, getSourceURL)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_FETCH_UNBOUND_SCRIPT_WITH_CHECK(getThis(), php_v8_unbound_script);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_unbound_script);

    v8::Local<v8::UnboundScript> local_unbound_script = php_v8_unbound_script_get_local(php_v8_unbound_script);

    php_v8_get_or_create_value(return_value, local_unbound_script->GetSourceURL(), php_v8_unbound_script->php_v8_isolate);
}

static PHP_METHOD(UnboundScript, getSourceMappingURL)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    PHP_V8_FETCH_UNBOUND_SCRIPT_WITH_CHECK(getThis(), php_v8_unbound_script);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_unbound_script);

    v8::Local<v8::UnboundScript> local_unbound_script = php_v8_unbound_script_get_local(php_v8_unbound_script);

    php_v8_get_or_create_value(return_value, local_unbound_script->GetSourceMappingURL(), php_v8_unbound_script->php_v8_isolate);
}

static PHP_METHOD(UnboundScript, getLineNumber)
{
    zend_long code_pos = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "l", &code_pos) == FAILURE) {
        return;
    }

    PHP_V8_CHECK_CODE_POS_RANGE(code_pos, "Value is out of range");

    PHP_V8_FETCH_UNBOUND_SCRIPT_WITH_CHECK(getThis(), php_v8_unbound_script);
    PHP_V8_ENTER_STORED_ISOLATE(php_v8_unbound_script);

    v8::Local<v8::UnboundScript> local_unbound_script = php_v8_unbound_script_get_local(php_v8_unbound_script);

    RETURN_LONG(static_cast<zend_long>(local_unbound_script->GetLineNumber(static_cast<int>(code_pos))));
}


PHP_V8_ZEND_BEGIN_ARG_WITH_CONSTRUCTOR_INFO_EX(arginfo___construct, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_getIsolate, ZEND_RETURN_VALUE, 0, V8\\Isolate, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_bindToContext, ZEND_RETURN_VALUE, 1, V8\\Script, 0)
                ZEND_ARG_OBJ_INFO(0, context, V8\\Context, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_getId, ZEND_RETURN_VALUE, 0, IS_LONG, 1)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_getScriptName, ZEND_RETURN_VALUE, 0, V8\\Value, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_getSourceURL, ZEND_RETURN_VALUE, 0, V8\\Value, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_getSourceMappingURL, ZEND_RETURN_VALUE, 0, V8\\Value, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_getLineNumber, ZEND_RETURN_VALUE, 1, IS_LONG, 0)
                ZEND_ARG_TYPE_INFO(0, code_pos, IS_LONG, 0)
ZEND_END_ARG_INFO()


static const zend_function_entry php_v8_unbound_script_methods[] = {
    PHP_V8_ME(UnboundScript, __construct,         ZEND_ACC_PRIVATE | ZEND_ACC_CTOR)
    PHP_V8_ME(UnboundScript, getIsolate,          ZEND_ACC_PUBLIC)
    PHP_V8_ME(UnboundScript, bindToContext,       ZEND_ACC_PUBLIC)
    PHP_V8_ME(UnboundScript, getId,               ZEND_ACC_PUBLIC)
    PHP_V8_ME(UnboundScript, getScriptName,       ZEND_ACC_PUBLIC)
    PHP_V8_ME(UnboundScript, getSourceURL,        ZEND_ACC_PUBLIC)
    PHP_V8_ME(UnboundScript, getSourceMappingURL, ZEND_ACC_PUBLIC)
    PHP_V8_ME(UnboundScript, getLineNumber,       ZEND_ACC_PUBLIC)

    PHP_FE_END
};


PHP_MINIT_FUNCTION(php_v8_unbound_script)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_V8_NS, "UnboundScript", php_v8_unbound_script_methods);
    this_ce = zend_register_internal_class(&ce);
    this_ce->create_object = php_v8_unbound_script_ctor;

    zend_declare_property_null(this_ce, ZEND_STRL("isolate"), ZEND_ACC_PRIVATE);

    memcpy(&php_v8_unbound_script_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

    php_v8_unbound_script_object_handlers.offset    = XtOffsetOf(php_v8_unbound_script_t, std);
    php_v8_unbound_script_object_handlers.free_obj  = php_v8_unbound_script_free;
    php_v8_unbound_script_object_handlers.clone_obj = NULL;

    return SUCCESS;
}
