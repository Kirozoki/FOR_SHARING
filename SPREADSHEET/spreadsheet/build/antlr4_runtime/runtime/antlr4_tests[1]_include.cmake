if(EXISTS "/home/kirozoki/1.CPP_Projects/S15/l1215t_ed1/build/antlr4_runtime/runtime/antlr4_tests[1]_tests.cmake")
  include("/home/kirozoki/1.CPP_Projects/S15/l1215t_ed1/build/antlr4_runtime/runtime/antlr4_tests[1]_tests.cmake")
else()
  add_test(antlr4_tests_NOT_BUILT antlr4_tests_NOT_BUILT)
endif()
