#!/bin/sh

aclocal
autoconf
autoheader
automake --foreign --add-missing

echo you are ready to run ./configure now
