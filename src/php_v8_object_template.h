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

#ifndef PHP_V8_OBJECT_TEMPLATE_H
#define PHP_V8_OBJECT_TEMPLATE_H

typedef struct _php_v8_object_template_t php_v8_object_template_t;

#include "php_v8_template.h"
#include "php_v8_exceptions.h"
#include "php_v8_template.h"
#include "php_v8_isolate.h"
#include <v8.h>

extern "C" {
#include "php.h"

#ifdef ZTS
#include "TSRM.h"
#endif
}

extern zend_class_entry* php_v8_object_template_class_entry;


inline php_v8_object_template_t * php_v8_object_template_fetch_object(zend_object *obj);


#define PHP_V8_OBJECT_TEMPLATE_FETCH(zv) php_v8_object_template_fetch_object(Z_OBJ_P(zv))
#define PHP_V8_OBJECT_TEMPLATE_FETCH_INTO(pzval, into) php_v8_object_template_t *(into) = PHP_V8_OBJECT_TEMPLATE_FETCH((pzval));

#define PHP_V8_EMPTY_OBJECT_TEMPLATE_MSG "ObjectTemplate" PHP_V8_EMPTY_HANDLER_MSG_PART
#define PHP_V8_CHECK_EMPTY_OBJECT_TEMPLATE_HANDLER(val) PHP_V8_CHECK_EMPTY_HANDLER((val), PHP_V8_EMPTY_OBJECT_TEMPLATE_MSG)

#define PHP_V8_FETCH_OBJECT_TEMPLATE_WITH_CHECK(pzval, into) \
    PHP_V8_OBJECT_TEMPLATE_FETCH_INTO(pzval, into); \
    PHP_V8_CHECK_EMPTY_OBJECT_TEMPLATE_HANDLER(into);


#define PHP_V8_OBJECT_TEMPLATE_CREATE_FROM_TEMPLATE(to_zval, to_php_v8_val, from_zval, from_php_v8_val) \
  object_init_ex((to_zval), php_v8_object_template_class_entry); \
  PHP_V8_TEMPLATE_STORE_ISOLATE((to_zval), PHP_V8_TEMPLATE_READ_ISOLATE(from_zval)); \
  PHP_V8_OBJECT_TEMPLATE_FETCH_INTO((to_zval), (to_php_v8_val)); \
  PHP_V8_COPY_POINTER_TO_ISOLATE((to_php_v8_val), (from_php_v8_val));


struct _php_v8_object_template_t {
    php_v8_isolate_t *php_v8_isolate;

    uint32_t isolate_handle;

    bool is_weak;
    v8::Persistent<v8::ObjectTemplate> *persistent;
    phpv8::PersistentData *persistent_data;

    zval *gc_data;
    int gc_data_count;

    phpv8::TemplateNode *node;

    zend_object std;
};

inline php_v8_object_template_t * php_v8_object_template_fetch_object(zend_object *obj) {
    return (php_v8_object_template_t *)((char *)obj - XtOffsetOf(php_v8_object_template_t, std));
}

inline v8::Local<v8::ObjectTemplate> php_v8_object_template_get_local(php_v8_object_template_t *php_v8_object_template) {
    return v8::Local<v8::ObjectTemplate>::New(php_v8_object_template->php_v8_isolate->isolate, *php_v8_object_template->persistent);
}


PHP_MINIT_FUNCTION(php_v8_object_template);

#endif //PHP_V8_OBJECT_TEMPLATE_H
