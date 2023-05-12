set(LLVM_LINK_COMPONENTS
    FrontendOpenMP
    Support
)

add_clang_tool(qt-obfuscator
    command_line_parser.cpp
    pr_generator.cpp
    qml_refactor.cpp
    general_ast_actions.cpp
    qt_obfuscator.cpp

)


target_link_libraries(qt-obfuscator
    PRIVATE
    clangTooling
    clangToolingRefactoring
)