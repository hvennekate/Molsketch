TEMPLATE = subdirs
SUBDIRS = main test

main.file = maincode.pro
test.file = testcode.pro
test.depends = main
