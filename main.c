#include <stdio.h>

void test_json_parse_string();
void test_json_parse_null();
void test_json_parse_bool();
void test_json_parse_int();
void test_json_parse_float();
void test_json_parse_object();
void test_json_parse_array();

int main() {
    test_json_parse_string();
    test_json_parse_null();
    test_json_parse_bool();
    test_json_parse_int();
    test_json_parse_float();
    test_json_parse_object();
    test_json_parse_array();

    printf("All tests passed!\n");
    return 0;
}