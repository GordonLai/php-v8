<?php declare(strict_types=1);

/**
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


namespace V8;


/**
 * A JavaScript string value (ECMA-262, 4.3.17).
 */
class StringValue extends NameValue
{
    const MAX_LENGTH = (1 << 30) - 25; // It's (1 << 28) - 16 on x32 platform, but we don't run on x32.

    /**
     * @param Isolate $isolate
     * @param string  $data
     */
    public function __construct(Isolate $isolate, $data = '')
    {
    }

    /**
     * @return string
     */
    public function value(): string
    {
    }

    /**
     * @return int
     */
    public function length(): int
    {
    }

    /**
     * @return int
     */
    public function utf8Length(): int
    {
    }

    /**
     * @return bool
     */
    public function isOneByte(): bool
    {
    }

    /**
     * @return bool
     */
    public function containsOnlyOneByte(): bool
    {
    }
}
