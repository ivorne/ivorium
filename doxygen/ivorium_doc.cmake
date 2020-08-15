include( "doxygen/get_target_property_recursive.cmake" )

find_package( Doxygen OPTIONAL_COMPONENTS dot )
if( DOXYGEN_FOUND )
    # style
    set( DOXYGEN_HTML_HEADER "../doxygen/header.html" )
    set( DOXYGEN_HTML_FOOTER "../doxygen/footer.html" )
    set( DOXYGEN_HTML_EXTRA_STYLESHEET "../doxygen/customdoxygen.css" )
    set( DOXYGEN_HTML_EXTRA_FILES "../doxygen/doxy-boot.js" )
    set( DOXYGEN_HTML_DYNAMIC_MENUS "NO" )
    set( DOXYGEN_GENERATE_TREEVIEW "NO" )
    set( DOXYGEN_GENERATE_HTML "YES" )
    set( DOXYGEN_HTML_TIMESTAMP "YES" )
    
    # options
    #set( DOXYGEN_USE_MDFILE_AS_MAINPAGE "../doxygen/main.md" )
    #set( DOXYGEN_CLANG_ASSISTED_PARSING "YES" )
    #set( DOXYGEN_INTERACTIVE_SVG "YES" )
    set( DOXYGEN_PROJECT_NUMBER "0.0.0" )
    set( DOXYGEN_BUILTIN_STL_SUPPORT "YES" )
    set( DOXYGEN_SOURCE_BROWSER "YES" )
    set( DOXYGEN_COLS_IN_ALPHA_INDEX "2" )
    set( DOXYGEN_CLASS_DIAGRAMS "NO" )
    set( DOXYGEN_CLASS_GRAPH "NO" )
    set( DOXYGEN_COLLABORATION_GRAPH "NO" )
    set( DOXYGEN_INCLUDE_GRAPH "NO" )
    set( DOXYGEN_INCLUDED_BY_GRAPH "NO" )
    set( DOXYGEN_DOT_IMAGE_FORMAT "svg" )
    set( DOXYGEN_EXTRACT_ALL "YES" )
    set( DOXYGEN_SORT_MEMBER_DOCS "NO" )
    set( DOXYGEN_GENERATE_TAGFILE "${CMAKE_CURRENT_BINARY_DIR}/html/ivorium.tag" )
    set( DOXYGEN_SHOW_NAMESPACES "NO" )
    
    # bugs
    set( DOXYGEN_SOURCE_TOOLTIPS "NO" )  # does not work, causes errors in console and disrupts bootstrap scripts
    
    # warnings
    set( DOXYGEN_WARN_IF_UNDOCUMENTED "NO" )
    
    # include directories
    get_target_property_recursive( DOX_INCLUDE_DIRECTORIES ivorium INCLUDE_DIRECTORIES )
    set( DOXYGEN_INCLUDE_PATH ${DOX_INCLUDE_DIRECTORIES} )
    
    # documentation target
    doxygen_add_docs( ivorium_doc "." WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src" )
endif()
