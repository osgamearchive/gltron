#!/bin/sh

aclocal
autoconf
automake --foreign --add-missing

echo you are ready to run ./configure now
