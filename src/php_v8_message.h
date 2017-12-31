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

#ifndef PHP_V8_MESSAGE_H
#define PHP_V8_MESSAGE_H

#include "php_v8_isolate.h"
#include <v8.h>

extern "C" {
#include "php.h"

#ifdef ZTS
#include "TSRM.h"
#endif
}

extern zend_class_entry* php_v8_message_class_entry;

extern void php_v8_message_create_from_message(zval *return_value, php_v8_isolate_t *php_v8_isolate, v8::Local<v8::Message> message);


PHP_MINIT_FUNCTION(php_v8_message);

#endif //PHP_V8_MESSAGE_H
