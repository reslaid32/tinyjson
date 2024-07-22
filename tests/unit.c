#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "json.h"

void test_json_parse_string() {
    JPoolManager manager;
    json_pool_manager_init(&manager, 1);
    
    const char* json_str = "\"hello world\"";
    char* result = json_parse_string(&manager, &json_str);
    assert(result != NULL);
    assert(strcmp(result, "hello world") == 0);

    json_pool_manager_free_pools(&manager);
    // printf("test_json_parse_string passed\n");
}

void test_json_parse_null() {
    const char* json_str = "null";
    int result = json_parse_null(&json_str);
    assert(result == 1);

    // printf("test_json_parse_null passed\n");
}

void test_json_parse_bool() {
    const char* json_str = "true";
    bool value;
    int result = json_parse_bool(&json_str, &value);
    assert(result == 1);
    assert(value == true);

    json_str = "false";
    result = json_parse_bool(&json_str, &value);
    assert(result == 1);
    assert(value == false);

    // printf("test_json_parse_bool passed\n");
}

void test_json_parse_int() {
    const char* json_str = "12345";
    int64_t value;
    int result = json_parse_int(&json_str, &value);
    assert(result == 1);
    assert(value == 12345);

    json_str = "-12345";
    result = json_parse_int(&json_str, &value);
    assert(result == 1);
    assert(value == -12345);

    // printf("test_json_parse_int passed\n");
}

void test_json_parse_float() {
    const char* json_str = "123.45";
    double value;
    int result = json_parse_float(&json_str, &value);
    assert(result == 1);
    assert(value == 123.45);

    json_str = "-123.45";
    result = json_parse_float(&json_str, &value);
    assert(result == 1);
    assert(value == -123.45);

    // printf("test_json_parse_float passed\n");
}

void test_json_parse_object() {
    JPoolManager manager;
    json_pool_manager_init(&manager, 1);

    const char* json_str = "{\"key1\": \"value1\", \"key2\": 42}";
    JObject obj = { .property_count = 0 };
    int result = json_parse_object(&manager, &obj, &json_str);
    assert(result == 1);
    assert(obj.property_count == 2);
    assert(strcmp(obj.properties[0].key, "key1") == 0);
    assert(strcmp(obj.properties[0].value.V.string_value, "value1") == 0);
    assert(strcmp(obj.properties[1].key, "key2") == 0);
    assert(obj.properties[1].value.V.integer_value == 42);

    json_pool_manager_free_pools(&manager);
    // printf("test_json_parse_object passed\n");
}

void test_json_parse_array() {
    JPoolManager manager;
    json_pool_manager_init(&manager, 1);

    const char* json_str = "[\"value1\", 42]";
    JArray array = { .element_count = 0 };
    int result = json_parse_array(&manager, &array, &json_str);
    assert(result == 1);
    assert(array.element_count == 2);
    assert(strcmp(array.elements[0].V.string_value, "value1") == 0);
    assert(array.elements[1].V.integer_value == 42);

    json_pool_manager_free_pools(&manager);
    // printf("test_json_parse_array passed\n");
}