ez_cmake_init()



# Get the name of this folder as the project name
get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME_WE)

ez_create_target(APPLICATION ${PROJECT_NAME})

if(EZ_CMAKE_PLATFORM_ANDROID)
  #TODO: Add actual packaging code. This is done in PRE_BUILD so that it happens before the
  #apk gen steps that happen in POST_BUILD and which are already done via ez_create_target. 
  add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${EZ_ROOT}/Data/UnitTests/FoundationTest/ ${CMAKE_CURRENT_BINARY_DIR}/package/Assets/Data/UnitTests/FoundationTest/)
endif()

target_link_libraries(${PROJECT_NAME}
  PUBLIC
  TestFramework
  Texture
)

add_dependencies(${PROJECT_NAME}
  FoundationTest_Plugin1
  FoundationTest_Plugin2
)

ez_ci_add_test(${PROJECT_NAME})

if(EZ_CMAKE_COMPILER_MSVC)
  target_compile_options(${PROJECT_NAME} PRIVATE /wd4244 /wd4018)
endif()
