set (MXML_DIR ${PROJECT_SOURCE_DIR}/deps/mxml)
set (MXML_CONFIG_DIR ${PROJECT_BINARY_DIR}/mxml_config)

add_library (mxml4 STATIC
	${MXML_DIR}/mxml-attr.c
	${MXML_DIR}/mxml-file.c
	${MXML_DIR}/mxml-get.c
	${MXML_DIR}/mxml-index.c
	${MXML_DIR}/mxml-node.c
	${MXML_DIR}/mxml-options.c
	${MXML_DIR}/mxml-private.c
	${MXML_DIR}/mxml-search.c
	${MXML_DIR}/mxml-set.c
)

file (MAKE_DIRECTORY ${MXML_CONFIG_DIR})
target_include_directories (mxml4 PRIVATE
	${MXML_DIR}
	${MXML_CONFIG_DIR}
)

if (NOT MSVC)
	target_compile_options (mxml4 PRIVATE -fPIC)
endif ()

set (MXML_VERSION_CUSTOM "Mini-XML v4.0.4 for ${PROJECT_NAME}")

# Check if we should use pthread or not, then fill in the config.h header file.
# See mxml_private.c for more details.
if (WIN32)    # On Windows, prefer Win32 API. MXML itself uses Win32 threading API if on Win32.
    set (PTHREAD_DEFINITION "undef HAVE_PTHREAD_H")
else ()    # Assume that non-Win32 uses pthread.
	set (PTHREAD_DEFINITION "define HAVE_PTHREAD_H 1")
endif ()

if (WIN32)	# Win32 has its own config file
	configure_file (${MXML_DIR}/vcnet/config.h ${MXML_CONFIG_DIR}/config.h @ONLY)
else ()
	configure_file (${PROJECT_SOURCE_DIR}/cmake/mxml4.config.h.in ${MXML_CONFIG_DIR}/config.h @ONLY)
endif ()
