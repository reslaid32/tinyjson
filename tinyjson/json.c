// json.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "json.h"

/**
 * @brief Initialize the memory pool manager.
 * 
 * @param manager Pointer to the pool manager.
 * @param pool_count Number of pools to be managed.
 */
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

/**
 * @brief Allocate memory from the pool.
 * 
 * @param manager Pointer to the pool manager.
 * @param size Size of memory to allocate.
 * @return Pointer to the allocated memory.
 */
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

/**
 * @brief Free a specific pool.
 * 
 * @param manager Pointer to the pool manager.
 * @param index Index of the pool to free.
 * @return Status code (1 on success, 0 on failure).
 */
int json_pool_manager_free_pool(JPoolManager* manager, size_t index) {
    if (index > manager->pool_count) {
        return 0;
    }
    free(manager->pools + index);
    return 1;
}

/**
 * @brief Free all pools managed by the pool manager.
 * 
 * @param manager Pointer to the pool manager.
 */
void json_pool_manager_free_pools(JPoolManager* manager) {
    free(manager->pools);
}

/**
 * @brief Skip whitespace characters in the JSON string.
 * 
 * @param str Pointer to the JSON string pointer.
 */
void json_skip_whitespace(const char** str) {
    while (isspace(**str)) {
        (*str)++;
    }
}

#ifdef _MSC_VER
    #pragma warning(push)
    #pragma warning(disable: 4996)
#endif // _MSC_VER

/**
 * @brief Parse a JSON string value.
 * 
 * @param manager Pointer to the pool manager.
 * @param str Pointer to the JSON string pointer.
 * @return Pointer to the parsed string.
 */
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

#ifdef _MSC_VER
    #pragma warning(pop)
#endif // _MSC_VER

/**
 * @brief Parse a JSON null value.
 * 
 * @param str Pointer to the JSON string pointer.
 * @return Status code (1 on success, 0 on failure).
 */
int json_parse_null(const char** str) {
    json_skip_whitespace(str);
    if (strncmp(*str, "null", 4) == 0) {
        *str += 4;
        return 1;
    }
    return 0;
}

/**
 * @brief Parse a JSON boolean value.
 * 
 * @param str Pointer to the JSON string pointer.
 * @param value Pointer to the boolean value to store the result.
 * @return Status code (1 on success, 0 on failure).
 */
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

/**
 * @brief Parse a JSON integer value.
 * 
 * @param str Pointer to the JSON string pointer.
 * @param value Pointer to the integer value to store the result.
 * @return Status code (1 on success, 0 on failure).
 */
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

/**
 * @brief Parse a JSON floating-point value.
 * 
 * @param str Pointer to the JSON string pointer.
 * @param value Pointer to the double value to store the result.
 * @return Status code (1 on success, 0 on failure).
 */
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

/**
 * @brief Parse a JSON property (key-value pair).
 * 
 * @param manager Pointer to the pool manager.
 * @param obj Pointer to the JSON object to store the property.
 * @param str Pointer to the JSON string pointer.
 * @return Status code (1 on success, 0 on failure).
 */
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

/**
 * @brief Parse a JSON object.
 * 
 * @param manager Pointer to the pool manager.
 * @param obj Pointer to the JSON object to store the parsed data.
 * @param str Pointer to the JSON string pointer.
 * @return Status code (1 on success, 0 on failure).
 */
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

/**
 * @brief Parse a JSON array.
 * 
 * @param manager Pointer to the pool manager.
 * @param array Pointer to the JSON array to store the parsed data.
 * @param str Pointer to the JSON string pointer.
 * @return Status code (1 on success, 0 on failure).
 */
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

/**
 * @brief Parse a generic JSON value.
 * 
 * @param manager Pointer to the pool manager.
 * @param value Pointer to the JSON value to store the parsed data.
 * @param str Pointer to the JSON string pointer.
 * @return Status code (1 on success, 0 on failure).
 */
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

// Helper function to write indentation
static int write_indent(char* buffer, size_t size, int* pos, int indent_level, int indent) {
    if (*pos >= (int)size) return -1;
    
    for (int i = 0; i < indent_level * indent; ++i) {
        if (*pos >= (int)size) return -1;
        buffer[(*pos)++] = ' ';
    }
    return 0;
}

// Helper function to write indentation to file
static int write_indent_to_file(FILE* file, int indent_level, int indent) {
    for (int i = 0; i < indent_level * indent; ++i) {
        if (fputc(' ', file) == EOF) return -1;
    }
    return 0;
}

/**
 * @brief Serialize a JSON object to a string buffer with indentation.
 * 
 * @param buffer Pointer to the buffer to store the serialized string.
 * @param size Size of the buffer.
 * @param obj Pointer to the JSON object to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 * @return The length of the serialized string, or -1 if the buffer is too small.
 */
int json_serialize_object_to_string(char* buffer, size_t size, JObject* obj, int indent) {
    int pos = 0;
    int result = snprintf(buffer + pos, size - pos, "{");
    if (result < 0 || result >= (int)(size - pos)) return -1;
    pos += result;

    if (indent > 0) {
        result = snprintf(buffer + pos, size - pos, "\n");
        if (result < 0 || result >= (int)(size - pos)) return -1;
        pos += result;
    }

    for (size_t i = 0; i < obj->property_count; ++i) {
        if (indent > 0) {
            if (write_indent(buffer, size, &pos, 1, indent) < 0) return -1;
        }
        result = snprintf(buffer + pos, size - pos, "\"%s\": ", obj->properties[i].key);
        if (result < 0 || result >= (int)(size - pos)) return -1;
        pos += result;

        result = json_serialize_value_to_string(buffer + pos, size - pos, &obj->properties[i].value, indent);
        if (result < 0 || result >= (int)(size - pos)) return -1;
        pos += result;

        if (i < obj->property_count - 1) {
            result = snprintf(buffer + pos, size - pos, ", ");
            if (result < 0 || result >= (int)(size - pos)) return -1;
            pos += result;
        }

        if (indent > 0) {
            result = snprintf(buffer + pos, size - pos, "\n");
            if (result < 0 || result >= (int)(size - pos)) return -1;
            pos += result;
        }
    }

    if (indent > 0) {
        if (write_indent(buffer, size, &pos, 0, indent) < 0) return -1;
    }
    result = snprintf(buffer + pos, size - pos, "}");
    if (result < 0 || result >= (int)(size - pos)) return -1;
    pos += result;

    return pos;
}

/**
 * @brief Serialize a JSON array to a string buffer with indentation.
 * 
 * @param buffer Pointer to the buffer to store the serialized string.
 * @param size Size of the buffer.
 * @param array Pointer to the JSON array to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 * @return The length of the serialized string, or -1 if the buffer is too small.
 */
int json_serialize_array_to_string(char* buffer, size_t size, JArray* array, int indent) {
    int pos = 0;
    int result = snprintf(buffer + pos, size - pos, "[");
    if (result < 0 || result >= (int)(size - pos)) return -1;
    pos += result;

    if (indent > 0) {
        result = snprintf(buffer + pos, size - pos, "\n");
        if (result < 0 || result >= (int)(size - pos)) return -1;
        pos += result;
    }

    for (size_t i = 0; i < array->element_count; ++i) {
        if (indent > 0) {
            if (write_indent(buffer, size, &pos, 1, indent) < 0) return -1;
        }

        result = json_serialize_value_to_string(buffer + pos, size - pos, &array->elements[i], indent);
        if (result < 0 || result >= (int)(size - pos)) return -1;
        pos += result;

        if (i < array->element_count - 1) {
            result = snprintf(buffer + pos, size - pos, ", ");
            if (result < 0 || result >= (int)(size - pos)) return -1;
            pos += result;
        }

        if (indent > 0) {
            result = snprintf(buffer + pos, size - pos, "\n");
            if (result < 0 || result >= (int)(size - pos)) return -1;
            pos += result;
        }
    }

    if (indent > 0) {
        if (write_indent(buffer, size, &pos, 0, indent) < 0) return -1;
    }
    result = snprintf(buffer + pos, size - pos, "]");
    if (result < 0 || result >= (int)(size - pos)) return -1;
    pos += result;

    return pos;
}

/**
 * @brief Serialize a generic JSON value to a string buffer with indentation.
 * 
 * @param buffer Pointer to the buffer to store the serialized string.
 * @param size Size of the buffer.
 * @param value Pointer to the JSON value to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 * @return The length of the serialized string, or -1 if the buffer is too small.
 */
int json_serialize_value_to_string(char* buffer, size_t size, JValue* value, int indent) {
    switch (value->T) {
        case JSON_VALUE_TYPE_NULL:
            return snprintf(buffer, size, "null");
        case JSON_VALUE_TYPE_BOOLEAN:
            return snprintf(buffer, size, value->V.boolean_value ? "true" : "false");
        case JSON_VALUE_TYPE_INTEGER:
            return snprintf(buffer, size, "%lld", (long long)value->V.integer_value);
        case JSON_VALUE_TYPE_REAL:
            return snprintf(buffer, size, "%f", value->V.real_value);
        case JSON_VALUE_TYPE_STRING:
            return json_serialize_string_to_buffer(value->V.string_value, buffer, size);
        case JSON_VALUE_TYPE_OBJECT:
            return json_serialize_object_to_string(buffer, size, value->V.object_value, indent);
        case JSON_VALUE_TYPE_ARRAY:
            return json_serialize_array_to_string(buffer, size, value->V.array_value, indent);
        default:
            return -1;
    }
}

/**
 * @brief Serialize a JSON string to a string buffer.
 * 
 * @param str Pointer to the JSON string to serialize.
 * @param buffer Pointer to the buffer to store the serialized string.
 * @param size Size of the buffer.
 * @return The length of the serialized string, or -1 if the buffer is too small.
 */
int json_serialize_string_to_buffer(const char* str, char* buffer, size_t size) {
    int pos = 0;
    int result = snprintf(buffer + pos, size - pos, "\"");
    if (result < 0 || result >= (int)(size - pos)) return -1;
    pos += result;

    while (*str) {
        if (pos >= (int)size) return -1;
        switch (*str) {
            case '\"':
                result = snprintf(buffer + pos, size - pos, "\\\"");
                break;
            case '\\':
                result = snprintf(buffer + pos, size - pos, "\\\\");
                break;
            case '\b':
                result = snprintf(buffer + pos, size - pos, "\\b");
                break;
            case '\f':
                result = snprintf(buffer + pos, size - pos, "\\f");
                break;
            case '\n':
                result = snprintf(buffer + pos, size - pos, "\\n");
                break;
            case '\r':
                result = snprintf(buffer + pos, size - pos, "\\r");
                break;
            case '\t':
                result = snprintf(buffer + pos, size - pos, "\\t");
                break;
            default:
                result = snprintf(buffer + pos, size - pos, "%c", *str);
                break;
        }
        if (result < 0 || result >= (int)(size - pos)) return -1;
        pos += result;
        str++;
    }

    result = snprintf(buffer + pos, size - pos, "\"");
    if (result < 0 || result >= (int)(size - pos)) return -1;
    pos += result;

    return pos;
}

/**
 * @brief Serialize a JSON object to a file with indentation.
 * 
 * @param file File pointer to write the serialized data.
 * @param obj Pointer to the JSON object to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 */
void json_serialize_object_to_file(FILE* file, JObject* obj, int indent) {
    if (fputc('{', file) == EOF) return;

    if (indent > 0) {
        if (fputc('\n', file) == EOF) return;
    }

    for (size_t i = 0; i < obj->property_count; ++i) {
        if (indent > 0) {
            if (write_indent_to_file(file, 1, indent) < 0) return;
        }

        fprintf(file, "\"%s\": ", obj->properties[i].key);
        json_serialize_value_to_file(file, &obj->properties[i].value, indent);

        if (i < obj->property_count - 1) {
            if (fputs(", ", file) == EOF) return;
        }

        if (indent > 0) {
            if (fputc('\n', file) == EOF) return;
        }
    }

    if (indent > 0) {
        if (write_indent_to_file(file, 0, indent) < 0) return;
    }

    if (fputc('}', file) == EOF) return;
}

/**
 * @brief Serialize a JSON array to a file with indentation.
 * 
 * @param file File pointer to write the serialized data.
 * @param array Pointer to the JSON array to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 */
void json_serialize_array_to_file(FILE* file, JArray* array, int indent) {
    if (fputc('[', file) == EOF) return;

    if (indent > 0) {
        if (fputc('\n', file) == EOF) return;
    }

    for (size_t i = 0; i < array->element_count; ++i) {
        if (indent > 0) {
            if (write_indent_to_file(file, 1, indent) < 0) return;
        }

        json_serialize_value_to_file(file, &array->elements[i], indent);

        if (i < array->element_count - 1) {
            if (fputs(", ", file) == EOF) return;
        }

        if (indent > 0) {
            if (fputc('\n', file) == EOF) return;
        }
    }

    if (indent > 0) {
        if (write_indent_to_file(file, 0, indent) < 0) return;
    }

    if (fputc(']', file) == EOF) return;
}

/**
 * @brief Serialize a generic JSON value to a file with indentation.
 * 
 * @param file File pointer to write the serialized data.
 * @param value Pointer to the JSON value to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 */
void json_serialize_value_to_file(FILE* file, JValue* value, int indent) {
    switch (value->T) {
        case JSON_VALUE_TYPE_NULL:
            if (fputs("null", file) == EOF) return;
            break;
        case JSON_VALUE_TYPE_BOOLEAN:
            if (fputs(value->V.boolean_value ? "true" : "false", file) == EOF) return;
            break;
        case JSON_VALUE_TYPE_INTEGER:
            fprintf(file, "%lld", (long long)value->V.integer_value);
            break;
        case JSON_VALUE_TYPE_REAL:
            fprintf(file, "%f", value->V.real_value);
            break;
        case JSON_VALUE_TYPE_STRING:
            json_serialize_string_to_file(value->V.string_value, file);
            break;
        case JSON_VALUE_TYPE_OBJECT:
            json_serialize_object_to_file(file, value->V.object_value, indent);
            break;
        case JSON_VALUE_TYPE_ARRAY:
            json_serialize_array_to_file(file, value->V.array_value, indent);
            break;
    }
}

/**
 * @brief Serialize a JSON string to a file.
 * 
 * @param str Pointer to the JSON string to serialize.
 * @param file File pointer to write the serialized data.
 */
void json_serialize_string_to_file(const char* str, FILE* file) {
    if (fputc('"', file) == EOF) return;

    while (*str) {
        switch (*str) {
            case '\"':
                if (fputs("\\\"", file) == EOF) return;
                break;
            case '\\':
                if (fputs("\\\\", file) == EOF) return;
                break;
            case '\b':
                if (fputs("\\b", file) == EOF) return;
                break;
            case '\f':
                if (fputs("\\f", file) == EOF) return;
                break;
            case '\n':
                if (fputs("\\n", file) == EOF) return;
                break;
            case '\r':
                if (fputs("\\r", file) == EOF) return;
                break;
            case '\t':
                if (fputs("\\t", file) == EOF) return;
                break;
            default:
                if (fputc(*str, file) == EOF) return;
                break;
        }
        str++;
    }

    if (fputc('"', file) == EOF) return;
}