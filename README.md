# tinyjson
Tiny lightweight json parser

## A JSON parser and serializer in C

This library provides function definitions and prototypes for a simple JSON parser and serializer. The parser can process JSON strings, numbers, booleans, NULLs, arrays, and objects. It uses a memory pool to efficiently manage memory during parsing and serialization.

### Main Features

- Parsing JSON strings into C structures.
- Serializing C structures back into JSON strings.
- Efficient memory management using a pool allocator.
- Support for basic JSON types: strings, numbers (integers and floats), booleans, null, arrays, and objects.
- Simple API for easy integration into other C projects.

This library is designed to be lightweight and easy to use, making it suitable for embedded systems and applications where resources are constrained.

### Usage

- Initialize the memory pool manager using `json_pool_manager_init`.
- Parse JSON strings using `json_parse_value` or type-specific parsing functions.
- Serialize C structures to JSON using the appropriate `json_serialize_*` functions.
- Free allocated memory using `json_pool_manager_free_pools`.

## Constants

### JSON_MAX_POOL_SIZE

Defines the maximum size (in bytes) of the memory pool used for efficient memory allocation during JSON parsing and serialization.

### JSON_MAX_PROPERTIES

Defines the maximum number of key-value pairs (properties) that a JSON object can contain.

### JSON_MAX_ARRAY_ELEMENTS

Defines the maximum number of elements that a JSON array can contain.

## Macros

### _jdbg_print

When `JSON_DEBUG_MODE` is defined, this macro will print debug messages using the `printf` function. Otherwise, it will do nothing.

```c
#define _jdbg_print(_fmt, ...) printf(_fmt, __VA_ARGS__)
```

### JVALUE_GET

Access the value of a specific type from a JSON value structure.

```c
#define JVALUE_GET(_JVal, _Type) _JVal##.V.##_Type##_value
```

### JVALUE_REF_GET

Access the value of a specific type from a pointer to a JSON value structure.

```c
#define JVALUE_REF_GET(_JVal, _Type) _JVal##->V.##_Type##_value
```

## Data Structures

### JMemoryPool

Memory pool for efficient allocation during JSON parsing.

```c
typedef struct _S_JMemoryPool {
    char data[JSON_MAX_POOL_SIZE];
    size_t used;
} JMemoryPool;
```

### JPoolManager

Manager for handling multiple memory pools.

```c
typedef struct _S_JPoolManager {
    JMemoryPool* pools;
    size_t pool_count;
    size_t current_pool;
} JPoolManager;
```

### JValueType

Enumeration of JSON value types.

```c
typedef enum _E_JValueType {
    JSON_VALUE_TYPE_STRING,
    JSON_VALUE_TYPE_NULL,
    JSON_VALUE_TYPE_BOOLEAN,
    JSON_VALUE_TYPE_INTEGER,
    JSON_VALUE_TYPE_REAL,
    JSON_VALUE_TYPE_ARRAY,
    JSON_VALUE_TYPE_OBJECT
} JValueType;
```

### JValue

JSON value structure.

```c
typedef struct _S_JValue {
    JValueType T;
    union {
        char* string_value;
        bool boolean_value;
        int64_t integer_value;
        double real_value;
        struct _S_JArray* array_value;
        struct _S_JObject* object_value;
    } V;
} JValue;
```

### JProperty

JSON property structure.

```c
typedef struct _S_JProperty {
    char* key;
    JValue value;
} JProperty;
```

### JObject

JSON object structure.

```c
typedef struct _S_JObject {
    JProperty properties[JSON_MAX_PROPERTIES];
    size_t property_count;
} JObject;
```

### JArray

JSON array structure.

```c
typedef struct _S_JArray {
    JValue elements[JSON_MAX_ARRAY_ELEMENTS];
    size_t element_count;
} JArray;
```

## Function Prototypes

### json_pool_manager_init

Initialize the memory pool manager.

```c
void json_pool_manager_init(JPoolManager* manager, size_t pool_count);
```

### json_pool_alloc

Allocate memory from the pool.

```c
void* json_pool_alloc(JPoolManager* manager, size_t size);
```

### json_pool_manager_free_pool

Free a specific pool.

```c
int json_pool_manager_free_pool(JPoolManager* manager, size_t index);
```

### json_pool_manager_free_pools

Free all pools managed by the pool manager.

```c
void json_pool_manager_free_pools(JPoolManager* manager);
```

### json_skip_whitespace

Skip whitespace characters in the JSON string.

```c
void json_skip_whitespace(const char** str);
```

### json_parse_string

Parse a JSON string value.

```c
char* json_parse_string(JPoolManager* manager, const char** str);
```

### json_parse_null

Parse a JSON null value.

```c
int json_parse_null(const char** str);
```

### json_parse_bool

Parse a JSON boolean value.

```c
int json_parse_bool(const char** str, bool* value);
```

### json_parse_int

Parse a JSON integer value.

```c
int json_parse_int(const char** str, int64_t* value);
```

### json_parse_float

Parse a JSON floating-point value.

```c
int json_parse_float(const char** str, double* value);
```

### json_parse_property

Parse a JSON property (key-value pair).

```c
int json_parse_property(JPoolManager* manager, JObject* obj, const char** str);
```

### json_parse_object

Parse a JSON object.

```c
int json_parse_object(JPoolManager* manager, JObject* obj, const char** str);
```

### json_parse_array

Parse a JSON array.

```c
int json_parse_array(JPoolManager* manager, JArray* array, const char** str);
```

### json_parse_value

Parse a generic JSON value.

```c
int json_parse_value(JPoolManager* manager, JValue* value, const char** str);
```

### json_serialize_object

Serialize a JSON object to a file.

```c
void json_serialize_object(FILE* file, JObject* obj);
```

### json_serialize_array

Serialize a JSON array to a file.

```c
void json_serialize_array(FILE* file, JArray* array);
```

### json_serialize_value

Serialize a generic JSON value to a file.

```c
void json_serialize_value(FILE* file, JValue* value);
```
