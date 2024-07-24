/**
 * @file _export.h
 * @brief Defines the JSON_API macro for symbol export and import in shared libraries.
 */

#ifndef EXPORT_H
#define EXPORT_H

/**
 * @def JSON_API
 * @brief Macro for exporting and importing symbols in a shared library.
 *
 * This macro handles the platform-specific directives for exporting and importing
 * symbols in a shared library. When building the library, symbols are exported,
 * and when using the library, symbols are imported.
 *
 * On Windows, __declspec(dllexport) is used for exporting symbols when building the
 * library, and __declspec(dllimport) is used for importing symbols when using the library.
 *
 * On other platforms, __attribute__((visibility("default"))) is used for exporting symbols
 * when building the library.
 */

#if defined(_WIN32) || defined(_WIN64)
#ifdef JSON_LIBRARY_BUILD
#define JSON_API __declspec(dllexport)
#else
#define JSON_API __declspec(dllimport)
#endif
#else
#ifdef JSON_LIBRARY_BUILD
#define JSON_API __attribute__((visibility("default")))
#else
#define JSON_API
#endif
#endif

#endif // EXPORT_H