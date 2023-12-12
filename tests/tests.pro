TEMPLATE = subdirs
SUBDIRS = main tests runner

main.file = maincode.pro
tests.file = testcode.pro
tests.depends = main
runner.file = testrunner.pro
runner.depends = tests
