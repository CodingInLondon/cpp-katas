// C++20: Modules - module interface unit for the 'greet' module.
// This is the exported interface consumed by modules.cpp.
// It must be compiled before the consumer (see build.sh).
export module greet;

export int add(int a, int b) { return a + b; }
export int multiply(int a, int b) { return a * b; }
