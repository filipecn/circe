set(STB_INCLUDES "${CMAKE_CURRENT_SOURCE_DIR}/ext/stb")

install(FILES
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/stb/stb_image.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/stb/stb_image_write.h
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/stb/stb_truetype.h
        DESTINATION ${INSTALL_PATH}/include)
