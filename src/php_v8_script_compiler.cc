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

#include "php_v8_script_compiler.h"
#include "php_v8_cached_data.h"
#include "php_v8_script.h"
#include "php_v8_script_origin.h"
#include "php_v8_unbound_script.h"
#include "php_v8_source.h"
#include "php_v8_function.h"
#include "php_v8_string.h"
#include "php_v8_value.h"
#include "php_v8_isolate.h"
#include "php_v8_a.h"
#include "php_v8.h"
#include "zend_smart_str.h"

zend_class_entry* php_v8_script_compiler_class_entry;
#define this_ce php_v8_script_compiler_class_entry


static v8::ScriptCompiler::Source * php_v8_build_source(zval *source_string_zv, zval *origin_zv, zval *cached_data_zv, v8::Isolate *isolate) {
    PHP_V8_VALUE_FETCH_INTO(source_string_zv, php_v8_source_string);

    v8::Local<v8::String> local_source_string = php_v8_value_get_local_as<v8::String>(php_v8_source_string);

    v8::ScriptOrigin *origin = NULL;

    if (!ZVAL_IS_NULL(origin_zv)) {
        origin = php_v8_create_script_origin_from_zval(origin_zv, isolate);
    }

    v8::ScriptCompiler::CachedData *cached_data = NULL;

    if (!ZVAL_IS_NULL(cached_data_zv)) {
        PHP_V8_FETCH_CACHED_DATA_INTO(cached_data_zv, php_v8_cached_data);
        cached_data = php_v8_cached_data->cached_data;
    }

    v8::ScriptCompiler::Source *source;

    if (origin) {
        source = new v8::ScriptCompiler::Source(local_source_string, *origin, cached_data);
    } else {
        source = new v8::ScriptCompiler::Source(local_source_string, cached_data);
    }

    return source;
}

static PHP_METHOD(ScriptCompiler, getCachedDataVersionTag)
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }

    php_v8_init();
    script_compiler_tag version = php_v8_startup_data_get_current_tag();
    uint64_t tag = (uint64_t) version.magic << (sizeof(version.tag) * 8) | version.tag;

    RETURN_DOUBLE(tag);
}

static PHP_METHOD(ScriptCompiler, compileUnboundScript)
{
    zval rv;

    zval *php_v8_context_zv;
    zval *php_v8_source_zv;
    zend_long options = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "oo|l", &php_v8_context_zv, &php_v8_source_zv, &options) == FAILURE) {
        return;
    }

    php_v8_init();

    PHP_V8_CHECK_COMPILER_OPTIONS_RANGE(options, "Invalid Script compiler options given. See V8\\ScriptCompiler OPTION_* class constants for available options.")

    PHP_V8_CONTEXT_FETCH_WITH_CHECK(php_v8_context_zv, php_v8_context);

    zval *source_string_zv = PHP_V8_SOURCE_READ_SOURCE_STRING(php_v8_source_zv);
    zval *origin_zv        = PHP_V8_SOURCE_READ_ORIGIN(php_v8_source_zv);
    zval *cached_data_zv   = PHP_V8_SOURCE_READ_CACHED_DATA(php_v8_source_zv);

    PHP_V8_VALUE_FETCH_WITH_CHECK(source_string_zv, php_v8_source_string);
    PHP_V8_DATA_ISOLATES_CHECK(php_v8_source_string, php_v8_context);

    if (!ZVAL_IS_NULL(cached_data_zv)) {
        PHP_V8_FETCH_CACHED_DATA_WITH_CHECK(cached_data_zv, php_v8_cached_data);
    }

    PHP_V8_ENTER_STORED_ISOLATE(php_v8_context);
    PHP_V8_ENTER_CONTEXT(php_v8_context);

    v8::ScriptCompiler::Source * source = php_v8_build_source(source_string_zv, origin_zv, cached_data_zv, isolate);

    if (source->GetResourceOptions().IsModule()) {
        PHP_V8_THROW_EXCEPTION("Unable to compile module as unbound script")
        return;
    }

    if (source->GetCachedData() == NULL && (options == v8::ScriptCompiler::CompileOptions::kConsumeParserCache || options == v8::ScriptCompiler::CompileOptions::kConsumeCodeCache)) {
        PHP_V8_THROW_EXCEPTION("Unable to consume cache when it's not set");
        return;
    }

    PHP_V8_TRY_CATCH(isolate);
    PHP_V8_INIT_ISOLATE_LIMITS_ON_CONTEXT(php_v8_context);

    v8::MaybeLocal<v8::UnboundScript> maybe_unbound_script = v8::ScriptCompiler::CompileUnboundScript(isolate, source, static_cast<v8::ScriptCompiler::CompileOptions>(options));

    PHP_V8_MAYBE_CATCH(php_v8_context, try_catch);
    PHP_V8_THROW_VALUE_EXCEPTION_WHEN_EMPTY(maybe_unbound_script, "Failed to compile unbound script");

    v8::Local<v8::UnboundScript> local_unbound_script = maybe_unbound_script.ToLocalChecked();

    php_v8_update_source_cached_data(php_v8_source_zv, source);
    php_v8_create_unbound_script(return_value, php_v8_context->php_v8_isolate, local_unbound_script);
}

static PHP_METHOD(ScriptCompiler, compile)
{
    zval rv;

    zval *php_v8_context_zv;
    zval *php_v8_source_zv;
    zend_long options = 0;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "oo|l", &php_v8_context_zv, &php_v8_source_zv, &options) == FAILURE) {
        return;
    }

    php_v8_init();

    PHP_V8_CHECK_COMPILER_OPTIONS_RANGE(options, "Invalid Script compiler options given. See V8\\ScriptCompiler OPTION_* class constants for available options.")

    PHP_V8_CONTEXT_FETCH_WITH_CHECK(php_v8_context_zv, php_v8_context);

    zval *source_string_zv = PHP_V8_SOURCE_READ_SOURCE_STRING(php_v8_source_zv);
    zval *origin_zv        = PHP_V8_SOURCE_READ_ORIGIN(php_v8_source_zv);
    zval *cached_data_zv   = PHP_V8_SOURCE_READ_CACHED_DATA(php_v8_source_zv);

    PHP_V8_VALUE_FETCH_WITH_CHECK(source_string_zv, php_v8_source_string);
    PHP_V8_DATA_ISOLATES_CHECK(php_v8_source_string, php_v8_context);

    if (!ZVAL_IS_NULL(cached_data_zv)) {
        PHP_V8_FETCH_CACHED_DATA_WITH_CHECK(cached_data_zv, php_v8_cached_data);
    }

    PHP_V8_ENTER_STORED_ISOLATE(php_v8_context);
    PHP_V8_ENTER_CONTEXT(php_v8_context);

    v8::ScriptCompiler::Source * source = php_v8_build_source(source_string_zv, origin_zv, cached_data_zv, isolate);

    if (source->GetResourceOptions().IsModule()) {
        PHP_V8_THROW_EXCEPTION("Unable to compile module as script")
        return;
    }

    if (source->GetCachedData() == NULL && (options == v8::ScriptCompiler::CompileOptions::kConsumeParserCache || options == v8::ScriptCompiler::CompileOptions::kConsumeCodeCache)) {
        PHP_V8_THROW_EXCEPTION("Unable to consume cache when it's not set");
        return;
    }

    PHP_V8_TRY_CATCH(isolate);
    PHP_V8_INIT_ISOLATE_LIMITS_ON_CONTEXT(php_v8_context);

    v8::MaybeLocal<v8::Script> maybe_script = v8::ScriptCompiler::Compile(context, source, static_cast<v8::ScriptCompiler::CompileOptions>(options));

    PHP_V8_MAYBE_CATCH(php_v8_context, try_catch);
    PHP_V8_THROW_VALUE_EXCEPTION_WHEN_EMPTY(maybe_script, "Failed to compile script");

    v8::Local<v8::Script> local_script = maybe_script.ToLocalChecked();

    php_v8_update_source_cached_data(php_v8_source_zv, source);
    php_v8_create_script(return_value, local_script, php_v8_context);
}

static PHP_METHOD(ScriptCompiler, compileFunctionInContext)
{
    zval rv;

    zval *php_v8_context_zv;
    zval *php_v8_source_zv;
    zval *arguments_zv = NULL;
    zval *context_extensions_zv = NULL;

    int arguments_count = 0;
    v8::Local<v8::String> *arguments = NULL;
    int context_extensions_count = 0;
    v8::Local<v8::Object> *context_extensions = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "oo|aa", &php_v8_context_zv, &php_v8_source_zv, &arguments_zv, &context_extensions_zv) == FAILURE) {
        return;
    }

    php_v8_init();

    PHP_V8_CONTEXT_FETCH_WITH_CHECK(php_v8_context_zv, php_v8_context);

    zval *source_string_zv = PHP_V8_SOURCE_READ_SOURCE_STRING(php_v8_source_zv);
    zval *origin_zv        = PHP_V8_SOURCE_READ_ORIGIN(php_v8_source_zv);
    zval *cached_data_zv   = PHP_V8_SOURCE_READ_CACHED_DATA(php_v8_source_zv);

    PHP_V8_VALUE_FETCH_WITH_CHECK(source_string_zv, php_v8_source_string);
    PHP_V8_DATA_ISOLATES_CHECK(php_v8_source_string, php_v8_context);

    if (!ZVAL_IS_NULL(cached_data_zv)) {
        PHP_V8_FETCH_CACHED_DATA_WITH_CHECK(cached_data_zv, php_v8_cached_data);
    }

    PHP_V8_ENTER_STORED_ISOLATE(php_v8_context);
    PHP_V8_ENTER_CONTEXT(php_v8_context);

    v8::ScriptCompiler::Source * source = php_v8_build_source(source_string_zv, origin_zv, cached_data_zv, isolate);

    if (source->GetResourceOptions().IsModule()) {
        PHP_V8_THROW_VALUE_EXCEPTION("Compiling module as a function in context is not supported");
        return;
    }

    if (arguments_zv != NULL && !php_v8_function_unpack_string_args(arguments_zv, 3, isolate, &arguments_count, &arguments)) {
        return;
    }

    if (context_extensions_zv != NULL && !php_v8_function_unpack_object_args(context_extensions_zv, 4, isolate, &context_extensions_count, &context_extensions)) {
        return;
    }

    PHP_V8_TRY_CATCH(isolate);
    PHP_V8_INIT_ISOLATE_LIMITS_ON_CONTEXT(php_v8_context);

    v8::MaybeLocal<v8::Function> maybe_function;
    maybe_function = v8::ScriptCompiler::CompileFunctionInContext(context,
                                                                  source,
                                                                  static_cast<size_t>(arguments_count),
                                                                  arguments,
                                                                  static_cast<size_t>(context_extensions_count),
                                                                  context_extensions);

    if (arguments) {
        efree(arguments);
    }

    if (context_extensions) {
        efree(context_extensions);
    }

    PHP_V8_MAYBE_CATCH(php_v8_context, try_catch);
    PHP_V8_THROW_VALUE_EXCEPTION_WHEN_EMPTY(maybe_function, "Failed to compile function in context");

    v8::Local<v8::Function> local_function = maybe_function.ToLocalChecked();

    php_v8_get_or_create_value(return_value, local_function, php_v8_context->php_v8_isolate);
}

static PHP_METHOD(ScriptCompiler, createCodeCache)
{
    zval rv;

    zval *php_v8_unbound_script_zv;
    zval *php_v8_source_string_zv;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "oo", &php_v8_unbound_script_zv, &php_v8_source_string_zv) == FAILURE) {
        return;
    }

    php_v8_init();

    PHP_V8_FETCH_UNBOUND_SCRIPT_WITH_CHECK(php_v8_unbound_script_zv, php_v8_unbound_script);
    PHP_V8_VALUE_FETCH_WITH_CHECK(php_v8_source_string_zv, php_v8_source_string);

    PHP_V8_DATA_ISOLATES_CHECK(php_v8_unbound_script, php_v8_source_string);

    PHP_V8_ENTER_STORED_ISOLATE(php_v8_unbound_script);

    v8::Local<v8::UnboundScript> local_unbound_script = php_v8_unbound_script_get_local(php_v8_unbound_script);
    v8::Local<v8::String> local_source_string = php_v8_value_get_local_as<v8::String>(php_v8_source_string);

    v8::ScriptCompiler::CachedData* cached_data = v8::ScriptCompiler::CreateCodeCache(local_unbound_script, local_source_string);

    if (!cached_data) {
        PHP_V8_THROW_VALUE_EXCEPTION("Failed to create code cache");
        return;
    }

    php_v8_create_cached_data(return_value, cached_data);
}


PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_getCachedDataVersionTag, ZEND_RETURN_VALUE, 0, IS_DOUBLE, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_compileUnboundScript, ZEND_RETURN_VALUE, 2, V8\\UnboundScript, 0)
                ZEND_ARG_OBJ_INFO(0, context, V8\\Context, 0)
                ZEND_ARG_OBJ_INFO(0, source, V8\\ScriptCompiler\\Source, 0)
                ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_compile, ZEND_RETURN_VALUE, 2, V8\\Script, 0)
                ZEND_ARG_OBJ_INFO(0, context, V8\\Context, 0)
                ZEND_ARG_OBJ_INFO(0, source, V8\\ScriptCompiler\\Source, 0)
                ZEND_ARG_TYPE_INFO(0, options, IS_LONG, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_compileFunctionInContext, ZEND_RETURN_VALUE, 2, V8\\FunctionObject, 0)
                ZEND_ARG_OBJ_INFO(0, context, V8\\Context, 0)
                ZEND_ARG_OBJ_INFO(0, source, V8\\ScriptCompiler\\Source, 0)
                ZEND_ARG_TYPE_INFO(0, arguments, IS_ARRAY, 0)
                ZEND_ARG_TYPE_INFO(0, context_extensions, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_createCodeCache, ZEND_RETURN_VALUE, 2, V8\\ScriptCompiler\\CachedData, 0)
                ZEND_ARG_OBJ_INFO(0, unbound_script, V8\\UnboundScript, 0)
                ZEND_ARG_OBJ_INFO(0, source_string, V8\\StringValue, 0)
ZEND_END_ARG_INFO()


static const zend_function_entry php_v8_script_compiler_methods[] = {
    PHP_V8_ME(ScriptCompiler, getCachedDataVersionTag,  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_V8_ME(ScriptCompiler, compileUnboundScript,     ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_V8_ME(ScriptCompiler, compile,                  ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_V8_ME(ScriptCompiler, compileFunctionInContext, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
    PHP_V8_ME(ScriptCompiler, createCodeCache,          ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)

    PHP_FE_END
};


PHP_MINIT_FUNCTION(php_v8_script_compiler)
{
    zend_class_entry ce;

    INIT_NS_CLASS_ENTRY(ce, PHP_V8_NS, "ScriptCompiler", php_v8_script_compiler_methods);
    this_ce = zend_register_internal_class(&ce);

    zend_declare_class_constant_long(this_ce, ZEND_STRL("OPTION_NO_COMPILE_OPTIONS"),      static_cast<zend_long>(v8::ScriptCompiler::CompileOptions::kNoCompileOptions));
    zend_declare_class_constant_long(this_ce, ZEND_STRL("OPTION_CONSUME_CODE_CACHE"),      static_cast<zend_long>(v8::ScriptCompiler::CompileOptions::kConsumeCodeCache));
    zend_declare_class_constant_long(this_ce, ZEND_STRL("OPTION_EAGER_COMPILE"),           static_cast<zend_long>(v8::ScriptCompiler::CompileOptions::kEagerCompile));

    return SUCCESS;
}
