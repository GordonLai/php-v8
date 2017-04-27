<?php

/**
 * This file is part of the pinepain/php-v8 PHP extension.
 *
 * Copyright (c) 2015-2017 Bogdan Padalko <pinepain@gmail.com>
 *
 * Licensed under the MIT license: http://opensource.org/licenses/MIT
 *
 * For the full copyright and license information, please view the
 * LICENSE file that was distributed with this source or visit
 * http://opensource.org/licenses/MIT
 */


namespace V8;

class CallbackInfo
{
    /**
     * @return \V8\Isolate
     */
    public function GetIsolate(): Isolate
    {
    }

    /**
     * @return \V8\Context
     */
    public function GetContext(): Context
    {
    }

    /**
     * Returns the receiver. This corresponds to the "this" value.
     *
     * @return \V8\ObjectValue
     */
    public function This(): ObjectValue
    {
    }

    /**
     * If the callback was created without a Signature, this is the same
     * value as This(). If there is a signature, and the signature didn't match
     * This() but one of its hidden prototypes, this will be the respective
     * hidden prototype.
     *
     * Note that this is not the prototype of This() on which the accessor
     * referencing this callback was found (which in V8 internally is often
     * referred to as holder [sic]).
     *
     * @return \V8\ObjectValue
     */
    public function Holder(): ObjectValue
    {
    }

    /**
     * The ReturnValue for the call
     *
     * @return \V8\ReturnValue
     */
    public function GetReturnValue(): ReturnValue
    {
    }
}
