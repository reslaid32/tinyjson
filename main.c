#include <stdio.h>

void test_json_parse_string();
void test_json_parse_null();
void test_json_parse_bool();
void test_json_parse_int();
void test_json_parse_float();
void test_json_parse_object();
void test_json_parse_array();
void test_json_serialization();
// void test_json_serialization_to_file();
void test_json_object_add_property();
void test_json_array_add_element();
void test_json_array_get_element();
void test_json_object_get_property_by_index();
void test_json_object_get_property();

int main() {
    test_json_parse_string();
    test_json_parse_null();
    test_json_parse_bool();
    test_json_parse_int();
    test_json_parse_float();
    test_json_parse_object();
    test_json_parse_array();
    test_json_serialization();
    // test_json_serialization_to_file();
    test_json_object_add_property();
    test_json_array_add_element();
    test_json_array_get_element();
    test_json_object_get_property_by_index();
    test_json_object_get_property();

    printf("All tests passed!\n");
    return 0;
}