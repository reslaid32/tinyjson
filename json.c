// json.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "json.h"

void json_pool_manager_init(JPoolManager* manager, size_t pool_count) {
    manager->pools = (JMemoryPool*)malloc(pool_count * sizeof(JMemoryPool));
    if (!manager->pools) {
        fprintf(stderr, "Failed to allocate memory for pools\n");
        exit(1);
    }
    manager->pool_count = pool_count;
    manager->current_pool = 0;
    for (size_t i = 0; i < pool_count; ++i) {
        manager->pools[i].used = 0;
    }
    _jdbg_print("[POOL] Manager initialized with %zu pools\n", pool_count);
}

void* json_pool_alloc(JPoolManager* manager, size_t size) {
    if (manager->current_pool >= manager->pool_count || size > JSON_MAX_POOL_SIZE) {
        fprintf(stderr, "Pool allocation failed: requested size %zu\n", size);
        return NULL;
    }
    
    JMemoryPool* pool = &manager->pools[manager->current_pool];
    if (pool->used + size > JSON_MAX_POOL_SIZE) {
        manager->current_pool++;
        if (manager->current_pool >= manager->pool_count) {
            fprintf(stderr, "Pool allocation failed: no more pools available\n");
            return NULL;
        }
        pool = &manager->pools[manager->current_pool];
        pool->used = 0;
    }
    
    void* result = pool->data + pool->used;
    pool->used += size;
    _jdbg_print("[POOL] Allocated %zu bytes from pool %zu, used %zu/%d\n", size, manager->current_pool, pool->used, JSON_MAX_POOL_SIZE);
    return result;
}

int json_pool_manager_free_pool(JPoolManager* manager, size_t index) {
    if (index > manager->pool_count) {
        return 0;
    }
    free(manager->pools + index);
    return 1;
}

void json_pool_manager_free_pools(JPoolManager* manager) {
    free(manager->pools);
}

void json_skip_whitespace(const char** str) {
    while (isspace(**str)) {
        (*str)++;
    }
}

char* json_parse_string(JPoolManager* manager, const char** str) {
    json_skip_whitespace(str);
    if (**str != '"') {
        return NULL;
    }
    (*str)++;
    const char* start = *str;
    while (**str && **str != '"') {
        (*str)++;
    }
    size_t length = *str - start;
    char* result = (char*)json_pool_alloc(manager, length + 1);
    if (result) {
        strncpy(result, start, length);
        result[length] = '\0';
    }
    (*str)++;
    _jdbg_print("[JSON] Parsed string: %s\n", result);
    return result;
}

int json_parse_null(const char** str) {
    json_skip_whitespace(str);
    if (strncmp(*str, "null", 4) == 0) {
        *str += 4;
        return 1;
    }
    return 0;
}

int json_parse_bool(const char** str, bool* value) {
    json_skip_whitespace(str);
    if (strncmp(*str, "true", 4) == 0) {
        *str += 4;
        *value = true;
        return 1;
    }
    if (strncmp(*str, "false", 5) == 0) {
        *str += 5;
        *value = false;
        return 1;
    }
    return 0;
}

int json_parse_int(const char** str, int64_t* value) {
    json_skip_whitespace(str);
    char* end;
    *value = strtoll(*str, &end, 10);
    if (*str == end) {
        return 0;
    }
    *str = end;
    _jdbg_print("[JSON] Parsed int: %lld\n", (long long)*value);
    return 1;
}

int json_parse_float(const char** str, double* value) {
    json_skip_whitespace(str);
    char* end;
    *value = strtod(*str, &end);
    if (*str == end) {
        return 0;
    }
    *str = end;
    _jdbg_print("[JSON] Parsed float: %f\n", *value);
    return 1;
}

int json_parse_property(JPoolManager* manager, JObject* obj, const char** str) {
    json_skip_whitespace(str);
    char* key = json_parse_string(manager, str);
    if (!key) return 0;
    
    json_skip_whitespace(str);
    if (**str != ':') return 0;
    (*str)++;
    
    json_skip_whitespace(str);
    if (obj->property_count < JSON_MAX_PROPERTIES) {
        JProperty* prop = &obj->properties[obj->property_count];
        prop->key = key;
        
        if (!json_parse_value(manager, &prop->value, str)) return 0;

        _jdbg_print("[JSON] Parsed JProperty: %s\n", prop->key);
        obj->property_count++;
    }
    return 1;
}

int json_parse_object(JPoolManager* manager, JObject* obj, const char** str) {
    json_skip_whitespace(str);
    if (**str != '{') {
        return 0;
    }
    (*str)++;
    
    while (**str && **str != '}') {
        if (!json_parse_property(manager, obj, str)) {
            return 0;
        }
        json_skip_whitespace(str);
        if (**str == ',') {
            (*str)++;
        }
    }
    if (**str != '}') {
        return 0;
    }
    (*str)++;
    _jdbg_print("[JSON] Parsed object with %zu properties\n", obj->property_count);
    return 1;
}

int json_parse_array(JPoolManager* manager, JArray* array, const char** str) {
    json_skip_whitespace(str);
    if (**str != '[') {
        return 0;
    }
    (*str)++;
    
    while (**str && **str != ']') {
        if (array->element_count < JSON_MAX_ARRAY_ELEMENTS) {
            JValue* element = &array->elements[array->element_count];
            if (!json_parse_value(manager, element, str)) return 0;
            array->element_count++;
        }
        json_skip_whitespace(str);
        if (**str == ',') {
            (*str)++;
        }
    }
    if (**str != ']') {
        return 0;
    }
    (*str)++;
    _jdbg_print("[JSON] Parsed array with %zu elements\n", array->element_count);
    return 1;
}

int json_parse_value(JPoolManager* manager, JValue* value, const char** str) {
    json_skip_whitespace(str);

    if (**str == '"') {
        value->T = JSON_VALUE_TYPE_STRING;
        value->V.string_value = json_parse_string(manager, str);
        if (!value->V.string_value) return 0;
    } else if (json_parse_null(str)) {
        value->T = JSON_VALUE_TYPE_NULL;
    } else if (json_parse_bool(str, &value->V.boolean_value)) {
        value->T = JSON_VALUE_TYPE_BOOLEAN;
    } else if (isdigit(**str) || (**str == '-' && isdigit(*(*str + 1)))) {
        const char* temp = *str;
        int is_float = 0;
        while (*temp && (isdigit(*temp) || *temp == '.' || *temp == 'e' || *temp == 'E' || *temp == '-' || *temp == '+')) {
            if (*temp == '.' || *temp == 'e' || *temp == 'E') {
                is_float = 1;
            }
            temp++;
        }
        if (is_float) {
            value->T = JSON_VALUE_TYPE_REAL;
            if (!json_parse_float(str, &value->V.real_value)) return 0;
        } else {
            value->T = JSON_VALUE_TYPE_INTEGER;
            if (!json_parse_int(str, &value->V.integer_value)) return 0;
        }
    } else if (**str == '{') {
        value->T = JSON_VALUE_TYPE_OBJECT;
        value->V.object_value = (JObject*)json_pool_alloc(manager, sizeof(JObject));
        value->V.object_value->property_count = 0;
        if (!json_parse_object(manager, value->V.object_value, str)) return 0;
    } else if (**str == '[') {
        value->T = JSON_VALUE_TYPE_ARRAY;
        value->V.array_value = (JArray*)json_pool_alloc(manager, sizeof(JArray));
        value->V.array_value->element_count = 0;
        if (!json_parse_array(manager, value->V.array_value, str)) return 0;
    } else {
        return 0;
    }
    return 1;
}

void json_serialize_object(FILE* file, JObject* obj) {
    fprintf(file, "{");
    for (size_t i = 0; i < obj->property_count; ++i) {

        fprintf(file, "\"%s\": ", obj->properties[i].key);
        json_serialize_value(file, &obj->properties[i].value);
        if (i < obj->property_count - 1) {
            fprintf(file, ", ");
        }
    }
    fprintf(file, "}");
}

void json_serialize_array(FILE* file, JArray* array) {
    fprintf(file, "[");
    for (size_t i = 0; i < array->element_count; ++i) {

        json_serialize_value(file, &array->elements[i]);
        if (i < array->element_count - 1) {
            fprintf(file, ", ");
        }
    }
    fprintf(file, "]");
}

void json_serialize_value(FILE* file, JValue* value) {
    switch (value->T) {
        case JSON_VALUE_TYPE_NULL:
            fprintf(file, "null");
            break;
        case JSON_VALUE_TYPE_BOOLEAN:
            fprintf(file, value->V.boolean_value ? "true" : "false");
            break;
        case JSON_VALUE_TYPE_INTEGER:
            fprintf(file, "%lld", (long long)value->V.integer_value);
            break;
        case JSON_VALUE_TYPE_REAL:
            fprintf(file, "%f", value->V.real_value);
            break;
        case JSON_VALUE_TYPE_STRING:
            fprintf(file, "\"%s\"", value->V.string_value);
            break;
        case JSON_VALUE_TYPE_OBJECT:
            json_serialize_object(file, value->V.object_value);
            break;
        case JSON_VALUE_TYPE_ARRAY:
            json_serialize_array(file, value->V.array_value);
            break;
    }
}