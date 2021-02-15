Compiling/running unit tests
------------------------------------

Unit tests will be automatically compiled if dependencies were met in configure
and tests weren't explicitly disabled.

After configuring, they can be run with 'make check'.

To run the cryptoyen_modernd tests manually, launch src/test/test_cryptoyen_modern .

To add more cryptoyen_modernd tests, add `BOOST_AUTO_TEST_CASE` functions to the existing
.cpp files in the test/ directory or add new .cpp files that
implement new BOOST_AUTO_TEST_SUITE sections.

To run the cryptoyen_modern-qt tests manually, launch src/qt/test/cryptoyen_modern-qt_test

To add more cryptoyen_modern-qt tests, add them to the `src/qt/test/` directory and
the `src/qt/test/test_main.cpp` file.
