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

#include "php_v8_primitive.h"
#include "php_v8_value.h"
#include "php_v8.h"

zend_class_entry* php_v8_primitive_class_entry;
#define this_ce php_v8_primitive_class_entry


PHP_V8_ZEND_BEGIN_ARG_WITH_RETURN_VOID_INFO_EX(arginfo_value, 0)
ZEND_END_ARG_INFO()


static const zend_function_entry php_v8_primitive_methods[] = {
        PHP_V8_ABSTRACT_ME(PrimitiveValue, value)
        PHP_FE_END
};


PHP_MINIT_FUNCTION(php_v8_primitive)
{
    zend_class_entry ce;
    INIT_NS_CLASS_ENTRY(ce, PHP_V8_NS, "PrimitiveValue", php_v8_primitive_methods);
    this_ce = zend_register_internal_class_ex(&ce, php_v8_value_class_entry);
    this_ce->ce_flags |= ZEND_ACC_EXPLICIT_ABSTRACT_CLASS;

    return SUCCESS;
}
