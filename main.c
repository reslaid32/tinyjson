#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <tinyjson/json.h>

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

void test_json_serialization() {
    JObject obj;
    obj.property_count = 2;
    obj.properties[0].key = "name";
    obj.properties[0].value.T = JSON_VALUE_TYPE_STRING;
    obj.properties[0].value.V.string_value = "Alice";
    obj.properties[1].key = "age";
    obj.properties[1].value.T = JSON_VALUE_TYPE_INTEGER;
    obj.properties[1].value.V.integer_value = 30;

    char buffer[256];

    int len = json_serialize_object_to_string(buffer, sizeof(buffer), &obj, 0);
    assert(len > 0);
    assert(strcmp(buffer, "{\"name\": \"Alice\", \"age\": 30}") == 0);

    JArray array;
    array.element_count = 2;
    array.elements[0].T = JSON_VALUE_TYPE_STRING;
    array.elements[0].V.string_value = "hello";
    array.elements[1].T = JSON_VALUE_TYPE_INTEGER;
    array.elements[1].V.integer_value = 42;

    len = json_serialize_array_to_string(buffer, sizeof(buffer), &array, 0);
    assert(len > 0);
    assert(strcmp(buffer, "[\"hello\", 42]") == 0);

    JValue value;
    value.T = JSON_VALUE_TYPE_STRING;
    value.V.string_value = "This is a \"test\" with \\ escape";

    len = json_serialize_value_to_string(buffer, sizeof(buffer), &value, 0);
    assert(len > 0);
    assert(strcmp(buffer, "\"This is a \\\"test\\\" with \\\\ escape\"") == 0);
}

// void test_json_serialization_to_file() {
//     JObject obj;
//     obj.property_count = 2;
//     obj.properties[0].key = "name";
//     obj.properties[0].value.T = JSON_VALUE_TYPE_STRING;
//     obj.properties[0].value.V.string_value = "Alice";
//     obj.properties[1].key = "age";
//     obj.properties[1].value.T = JSON_VALUE_TYPE_INTEGER;
//     obj.properties[1].value.V.integer_value = 30;

//     json_serialize_object_to_file(stdout, &obj, 0);
//     printf("\n");

//     JArray array;
//     array.element_count = 2;
//     array.elements[0].T = JSON_VALUE_TYPE_STRING;
//     array.elements[0].V.string_value = "hello";
//     array.elements[1].T = JSON_VALUE_TYPE_INTEGER;
//     array.elements[1].V.integer_value = 42;

//     json_serialize_array_to_file(stdout, &array, 0);
//     printf("\n");

//     JValue value;
//     value.T = JSON_VALUE_TYPE_STRING;
//     value.V.string_value = "This is a \"test\" with \\ escape";

//     json_serialize_value_to_file(stdout, &value, 0);
//     printf("\n");
// }

void test_json_object_add_property() {
    JObject obj = { .property_count = 0 };
    JValue value;
    value.T = JSON_VALUE_TYPE_STRING;
    value.V.string_value = "value";

    int result = json_object_add_property(&obj, "key", &value);

    assert(result == 1);
    assert(obj.property_count == 1);
    assert(strcmp(obj.properties[0].key, "key") == 0);
    assert(strcmp(JVALUE_GET(obj.properties[0].value, string), "value") == 0);
}

void test_json_array_add_element() {
    JArray array = { .element_count = 0 };
    JValue value;
    value.T = JSON_VALUE_TYPE_INTEGER;
    value.V.integer_value = 42;

    int result = json_array_add_element(&array, &value);

    assert(result == 1);
    assert(array.element_count == 1);
    assert(JVALUE_GET(array.elements[0], integer) == 42);
}

void test_json_array_get_element() {
    JArray array = { .element_count = 1 };
    JValue value;
    value.T = JSON_VALUE_TYPE_BOOLEAN;
    value.V.boolean_value = true;
    array.elements[0] = value;

    JValue* retrieved_value;
    int result = json_array_get_element(&array, 0, &retrieved_value);

    assert(result == 1);
    assert(retrieved_value->T == JSON_VALUE_TYPE_BOOLEAN);
    assert(retrieved_value->V.boolean_value == true);
}

void test_json_object_get_property_by_index() {
    JObject obj = { .property_count = 1 };
    JProperty property;
    property.key = "key";
    property.value.T = JSON_VALUE_TYPE_REAL;
    property.value.V.real_value = 3.14;
    obj.properties[0] = property;

    JProperty* retrieved_property;
    int result = json_object_get_property_by_index(&obj, 0, &retrieved_property);

    assert(result == 1);
    assert(strcmp(retrieved_property->key, "key") == 0);
    assert(retrieved_property->value.T == JSON_VALUE_TYPE_REAL);
    assert(retrieved_property->value.V.real_value == 3.14);
}

void test_json_object_get_property() {
    JObject obj = { .property_count = 1 };
    JProperty property;
    property.key = "key";
    property.value.T = JSON_VALUE_TYPE_NULL;
    obj.properties[0] = property;

    JProperty* retrieved_property;
    int result = json_object_get_property(&obj, "key", &retrieved_property);

    assert(result == 1);
    assert(strcmp(retrieved_property->key, "key") == 0);
    assert(retrieved_property->value.T == JSON_VALUE_TYPE_NULL);
}

void test_json_array_remove_element() {
    JArray array = { .element_count = 1 };
    JValue value;
    value.T = JSON_VALUE_TYPE_BOOLEAN;
    value.V.boolean_value = true;
    array.elements[0] = value;

    JValue* retrieved_value;
    int result = json_array_get_element(&array, 0, &retrieved_value);

    assert(result == 1);
    assert(retrieved_value->T == JSON_VALUE_TYPE_BOOLEAN);
    assert(retrieved_value->V.boolean_value == true);

    result = json_array_remove_element(&array, 0);
    assert(result == 1);
    assert(array.element_count == 0);
}

void test_json_object_remove_property_by_index() {
    JObject obj = { .property_count = 1 };
    JProperty property;
    property.key = "key";
    property.value.T = JSON_VALUE_TYPE_REAL;
    property.value.V.real_value = 3.14;
    obj.properties[0] = property;

    JProperty* retrieved_property;
    int result = json_object_get_property_by_index(&obj, 0, &retrieved_property);

    assert(result == 1);
    assert(strcmp(retrieved_property->key, "key") == 0);
    assert(retrieved_property->value.T == JSON_VALUE_TYPE_REAL);
    assert(retrieved_property->value.V.real_value == 3.14);

    result = json_object_remove_property_by_index(&obj, 0);
    assert(result == 1);
    assert(obj.property_count == 0);
}

void test_json_object_remove_property() {
    JObject obj = { .property_count = 1 };
    JProperty property;
    property.key = "key";
    property.value.T = JSON_VALUE_TYPE_NULL;
    obj.properties[0] = property;

    JProperty* retrieved_property;
    int result = json_object_get_property(&obj, "key", &retrieved_property);

    assert(result == 1);
    assert(strcmp(retrieved_property->key, "key") == 0);
    assert(retrieved_property->value.T == JSON_VALUE_TYPE_NULL);

    result = json_object_remove_property(&obj, "key");
    assert(result == 1);
    assert(obj.property_count == 0);
}

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