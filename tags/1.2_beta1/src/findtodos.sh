#!/bin/sh
grep -n '// TODO:' *.{cpp,h}
grep -n '// FIXME:' *.{cpp,h}
