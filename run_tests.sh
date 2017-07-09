#! /bin/bash

rm -rf tests/build/
mkdir tests/build
(cd tests/build && cmake .. && make && ./string_view_tests)
