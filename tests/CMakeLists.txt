set(SOURCES
        main.cpp
        vk_tests.cpp
        )

add_executable(circe_tests ${SOURCES})
target_include_directories(circe_tests PUBLIC ${CATCH2_INCLUDES})
target_link_options(circe_tests INTERFACE --coverage)

if (UNIX AND NOT APPLE)
    target_link_libraries(circe_tests circe gcov ${VULKAN_LIBRARIES})

    add_custom_target(test_circe
            COMMAND lcov --directory . --zerocounters
            COMMAND circe_tests DEPENDS circe_tests
            COMMAND lcov --directory . --capture -output-file cov.info
            COMMAND lcov --remove cov.info '/usr/*' --output-file cov.info
            COMMAND lcov --remove cov.info '*/ext/*' --output-file cov.info
            COMMAND lcov --remove cov.info '*/tests/*' --output-file cov.info
            COMMAND lcov --remove cov.info '*/spdlog/*' --output-file cov.info
            COMMAND genhtml cov.info --output-directory out
            )
else (UNIX AND NOT APPLE)

    target_link_libraries(circe_tests circe)

endif (UNIX AND NOT APPLE)
