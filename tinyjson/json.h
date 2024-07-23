/**
 * @file json.h
 * @brief A JSON parser and serializer in C.
 * 
 * This file provides the definitions and function prototypes for a simple JSON 
 * parser and serializer. The parser can handle JSON strings, numbers, booleans, 
 * null values, arrays, and objects. It uses a memory pool for efficient memory 
 * management during parsing and serialization.
 * 
 * The main features of this JSON library include:
 * - Parsing JSON strings into C structures.
 * - Serializing C structures back into JSON strings.
 * - Efficient memory management using a pool allocator.
 * - Support for basic JSON types: strings, numbers (integers and floats), 
 *   booleans, null, arrays, and objects.
 * - Simple API for easy integration into other C projects.
 * 
 * This library is designed to be lightweight and easy to use, making it suitable 
 * for embedded systems and applications where resources are constrained.
 * 
 * Usage:
 * - Initialize the memory pool manager using `json_pool_manager_init`.
 * - Parse JSON strings using `json_parse_value` or type-specific parsing functions.
 * - Serialize C structures to JSON using the appropriate `json_serialize_*` functions.
 * - Free allocated memory using `json_pool_manager_free_pools`.
 */

#ifndef JSON_H
#define JSON_H

#include <stddef.h>
#include <stdint.h>
#include <float.h>
#include <stdbool.h>

#ifdef JSON_DEBUG_MODE
/**
 * @brief Macro for debug printing.
 * 
 * When JSON_DEBUG_MODE is defined, this macro will print debug messages
 * using the `printf` function. Otherwise, it will do nothing.
 * 
 * @param _fmt Format string.
 * @param ... Additional arguments for the format string.
 */
#define _jdbg_print(_fmt, ...) printf(_fmt, __VA_ARGS__)
#else
#define _jdbg_print(_fmt, ...)
#endif

/**
 * @brief Maximum size of the memory pool.
 * 
 * This macro defines the maximum size (in bytes) of the memory pool used for
 * efficient memory allocation during JSON parsing and serialization.
 */
#define JSON_MAX_POOL_SIZE 16384

/**
 * @brief Maximum number of properties in a JSON object.
 * 
 * This macro defines the maximum number of key-value pairs (properties) that a
 * JSON object can contain.
 */
#define JSON_MAX_PROPERTIES 100

/**
 * @brief Maximum number of elements in a JSON array.
 * 
 * This macro defines the maximum number of elements that a JSON array can contain.
 */
#define JSON_MAX_ARRAY_ELEMENTS 100

/**
 * @brief Define the indentation level for JSON serialization.
 * 
 * This macro sets the number of spaces used for indentation in JSON serialization. 
 * The value of _Indent should be a positive integer representing the number of spaces.
 * 
 * @param _Indent The number of spaces for indentation.
 */
#define JSON_INDENT(_Indent) (int)_Indent

/**
 * @brief Define no indentation for JSON serialization.
 * 
 * This macro represents zero indentation level, meaning that JSON serialization will
 * be compact without additional spaces for indentation.
 */
#define JSON_INDENT_NULL JSON_INDENT(0)

/**
 * @brief Macro to access the value of a specific type from a JSON value.
 * 
 * This macro is used to access the value of a specific type from a JSON value 
 * structure. It simplifies accessing the value stored in the union.
 * 
 * @param _JVal The JSON value structure.
 * @param _Type The type of the value to access (string, boolean, integer, real, array, object).
 * @return The value of the specified type.
 */
#define JVALUE_GET(_JVal,_Type) _JVal##.V.##_Type##_value

/**
 * @brief Macro to access the value of a specific type from a pointer to a JSON value.
 * 
 * This macro is used to access the value of a specific type from a pointer to a
 * JSON value structure. It simplifies accessing the value stored in the union.
 * 
 * @param _JVal Pointer to the JSON value structure.
 * @param _Type The type of the value to access (string, boolean, integer, real, array, object).
 * @return The value of the specified type.
 */
#define JVALUE_REF_GET(_JVal,_Type) _JVal##->V.##_Type##_value

/**
 * @brief Memory pool for efficient allocation during JSON parsing.
 */
typedef struct _S_JMemoryPool {
    char data[JSON_MAX_POOL_SIZE]; /**< Pool data */
    size_t used; /**< Amount of memory used in the pool */
} JMemoryPool;

/**
 * @brief Manager for handling multiple memory pools.
 */
typedef struct _S_JPoolManager {
    JMemoryPool*    pools; /**< Array of memory pools */
    size_t          pool_count; /**< Total number of pools */
    size_t          current_pool; /**< Index of the current pool being used */
} JPoolManager;

/**
 * @brief Enumeration of JSON value types.
 */
typedef enum _E_JValueType {
    JSON_VALUE_TYPE_STRING,
    JSON_VALUE_TYPE_NULL,
    JSON_VALUE_TYPE_BOOLEAN,
    JSON_VALUE_TYPE_INTEGER,
    JSON_VALUE_TYPE_REAL,
    JSON_VALUE_TYPE_ARRAY,
    JSON_VALUE_TYPE_OBJECT
} JValueType;

/**
 * @brief JSON value structure.
 */
typedef struct _S_JValue {
    JValueType T; /**< Type of the JSON value */
    union {
        char* string_value; /**< String value */
        bool boolean_value; /**< Boolean value */
        int64_t integer_value; /**< Integer value */
        double real_value; /**< Real (floating point) value */
        struct _S_JArray* array_value; /**< Array value */
        struct _S_JObject* object_value; /**< Object value */
    } V; /**< Union holding the value */
} JValue;

/**
 * @brief JSON property structure.
 */
typedef struct _S_JProperty {
    char* key; /**< Key of the property */
    JValue value; /**< Value of the property */
} JProperty;

/**
 * @brief JSON object structure.
 */
typedef struct _S_JObject {
    JProperty properties[JSON_MAX_PROPERTIES]; /**< Array of properties */
    size_t property_count; /**< Number of properties */
} JObject;

/**
 * @brief JSON array structure.
 */
typedef struct _S_JArray {
    JValue elements[JSON_MAX_ARRAY_ELEMENTS]; /**< Array of elements */
    size_t element_count; /**< Number of elements */
} JArray;

/**
 * @brief Initialize the memory pool manager.
 * 
 * @param manager Pointer to the pool manager.
 * @param pool_count Number of pools to be managed.
 */
void json_pool_manager_init(JPoolManager* manager, size_t pool_count);

/**
 * @brief Allocate memory from the pool.
 * 
 * @param manager Pointer to the pool manager.
 * @param size Size of memory to allocate.
 * @return Pointer to the allocated memory.
 */
void* json_pool_alloc(JPoolManager* manager, size_t size);

/**
 * @brief Free a specific pool.
 * 
 * @param manager Pointer to the pool manager.
 * @param index Index of the pool to free.
 * @return Status code (1 on success, 0 on failure).
 */
int json_pool_manager_free_pool(JPoolManager* manager, size_t index);

/**
 * @brief Free all pools managed by the pool manager.
 * 
 * @param manager Pointer to the pool manager.
 */
void json_pool_manager_free_pools(JPoolManager* manager);

/**
 * @brief Skip whitespace characters in the JSON string.
 * 
 * @param str Pointer to the JSON string pointer.
 */
void json_skip_whitespace(const char** str);

/**
 * @brief Parse a JSON string value.
 * 
 * @param manager Pointer to the pool manager.
 * @param str Pointer to the JSON string pointer.
 * @return Pointer to the parsed string.
 */
char* json_parse_string(JPoolManager* manager, const char** str);

/**
 * @brief Parse a JSON null value.
 * 
 * @param str Pointer to the JSON string pointer.
 * @return Status code (1 on success, 0 on failure).
 */
int json_parse_null(const char** str);

/**
 * @brief Parse a JSON boolean value.
 * 
 * @param str Pointer to the JSON string pointer.
 * @param value Pointer to the boolean value to store the result.
 * @return Status code (1 on success, 0 on failure).
 */
int json_parse_bool(const char** str, bool* value);

/**
 * @brief Parse a JSON integer value.
 * 
 * @param str Pointer to the JSON string pointer.
 * @param value Pointer to the integer value to store the result.
 * @return Status code (1 on success, 0 on failure).
 */
int json_parse_int(const char** str, int64_t* value);

/**
 * @brief Parse a JSON floating-point value.
 * 
 * @param str Pointer to the JSON string pointer.
 * @param value Pointer to the double value to store the result.
 * @return Status code (1 on success, 0 on failure).
 */
int json_parse_float(const char** str, double* value);

/**
 * @brief Parse a JSON property (key-value pair).
 * 
 * @param manager Pointer to the pool manager.
 * @param obj Pointer to the JSON object to store the property.
 * @param str Pointer to the JSON string pointer.
 * @return Status code (1 on success, 0 on failure).
 */
int json_parse_property(JPoolManager* manager, JObject* obj, const char** str);

/**
 * @brief Parse a JSON object.
 * 
 * @param manager Pointer to the pool manager.
 * @param obj Pointer to the JSON object to store the parsed data.
 * @param str Pointer to the JSON string pointer.
 * @return Status code (1 on success, 0 on failure).
 */
int json_parse_object(JPoolManager* manager, JObject* obj, const char** str);

/**
 * @brief Parse a JSON array.
 * 
 * @param manager Pointer to the pool manager.
 * @param array Pointer to the JSON array to store the parsed data.
 * @param str Pointer to the JSON string pointer.
 * @return Status code (1 on success, 0 on failure).
 */
int json_parse_array(JPoolManager* manager, JArray* array, const char** str);

/**
 * @brief Parse a generic JSON value.
 * 
 * @param manager Pointer to the pool manager.
 * @param value Pointer to the JSON value to store the parsed data.
 * @param str Pointer to the JSON string pointer.
 * @return Status code (1 on success, 0 on failure).
 */
int json_parse_value(JPoolManager* manager, JValue* value, const char** str);

/**
 * @brief Serialize a JSON object to a string buffer with indentation.
 * 
 * @param buffer Pointer to the buffer to store the serialized string.
 * @param size Size of the buffer.
 * @param obj Pointer to the JSON object to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 * @return The length of the serialized string, or -1 if the buffer is too small.
 */
int json_serialize_object_to_string(char* buffer, size_t size, JObject* obj, int indent);

/**
 * @brief Serialize a JSON array to a string buffer with indentation.
 * 
 * @param buffer Pointer to the buffer to store the serialized string.
 * @param size Size of the buffer.
 * @param array Pointer to the JSON array to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 * @return The length of the serialized string, or -1 if the buffer is too small.
 */
int json_serialize_array_to_string(char* buffer, size_t size, JArray* array, int indent);

/**
 * @brief Serialize a generic JSON value to a string buffer with indentation.
 * 
 * @param buffer Pointer to the buffer to store the serialized string.
 * @param size Size of the buffer.
 * @param value Pointer to the JSON value to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 * @return The length of the serialized string, or -1 if the buffer is too small.
 */
int json_serialize_value_to_string(char* buffer, size_t size, JValue* value, int indent);

/**
 * @brief Serialize a JSON string value to a string buffer.
 * 
 * @param str Pointer to the JSON string to serialize.
 * @param buffer Pointer to the buffer to store the serialized string.
 * @param size Size of the buffer.
 * @return The length of the serialized string, or -1 if the buffer is too small.
 */
int json_serialize_string_to_buffer(const char* str, char* buffer, size_t size);

/**
 * @brief Serialize a JSON object to a file with indentation.
 * 
 * @param file File pointer to write the serialized data.
 * @param obj Pointer to the JSON object to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 */
void json_serialize_object_to_file(FILE* file, JObject* obj, int indent);

/**
 * @brief Serialize a JSON array to a file with indentation.
 * 
 * @param file File pointer to write the serialized data.
 * @param array Pointer to the JSON array to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 */
void json_serialize_array_to_file(FILE* file, JArray* array, int indent);

/**
 * @brief Serialize a generic JSON value to a file with indentation.
 * 
 * @param file File pointer to write the serialized data.
 * @param value Pointer to the JSON value to serialize.
 * @param indent The number of spaces for indentation. Use 0 for no indentation.
 */
void json_serialize_value_to_file(FILE* file, JValue* value, int indent);

/**
 * @brief Serialize a JSON string to a file.
 * 
 * @param str Pointer to the JSON string to serialize.
 * @param file File pointer to write the serialized data.
 */
void json_serialize_string_to_file(const char* str, FILE* file);

/**
 * @brief Add a property to a JSON object.
 * 
 * @param obj Pointer to the JSON object.
 * @param key Pointer to the key string.
 * @param value Pointer to the value.
 * @return Status code (1 on success, 0 on failure).
 */
int json_object_add_property(JObject* obj, const char* key, JValue* value);

/**
 * @brief Add an element to a JSON array.
 * 
 * @param array Pointer to the JSON array.
 * @param value Pointer to the value.
 * @return Status code (1 on success, 0 on failure).
 */
int json_array_add_element(JArray* array, JValue* value);

/**
 * @brief Get an element from a JSON array by index.
 * 
 * @param array Pointer to the JSON array.
 * @param index Index of the element to retrieve.
 * @param value Pointer to the value to store the retrieved element.
 * @return Status code (1 on success, 0 on failure).
 */
int json_array_get_element(const JArray* array, size_t index, JValue** value);

/**
 * @brief Get a property from a JSON object by index.
 * 
 * @param obj Pointer to the JSON object.
 * @param index Index of the property to retrieve.
 * @param property Pointer to the property to store the retrieved property.
 * @return Status code (1 on success, 0 on failure).
 */
int json_object_get_property_by_index(const JObject* obj, size_t index, JProperty** property);

/**
 * @brief Get a property from a JSON object by key.
 * 
 * @param obj Pointer to the JSON object.
 * @param key Pointer to the key string.
 * @param property Pointer to the property to store the retrieved property.
 * @return Status code (1 on success, 0 on failure).
 */
int json_object_get_property(const JObject* obj, const char* key, JProperty** property);

#endif // JSON_H
