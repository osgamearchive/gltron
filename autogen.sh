#!/bin/sh

libtoolize
aclocal

autoconf
automake --add-missing

echo you are ready to run ./configure now
