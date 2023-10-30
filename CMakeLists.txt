cmake_minimum_required(VERSION 3.0)
project(Game)

add_executable(Game
    src/main.cpp
)

target_include_directories(Game PRIVATE
    include
)

find_package(nlohmann_json 3.2.0 REQUIRED)
target_link_libraries(Game PRIVATE nlohmann_json::nlohmann_json)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# путь к исходному файлу data.json
set(SOURCE_DATA_FILE "${CMAKE_CURRENT_SOURCE_DIR}/data.json")

# копируем data.json
set(DEST_DATA_DIR "${CMAKE_BINARY_DIR}")

# имя для скопированного файла то же.
set(DEST_DATA_FILE "${DEST_DATA_DIR}/data.json")

# копирование файла
add_custom_command(
    OUTPUT "${DEST_DATA_FILE}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SOURCE_DATA_FILE}" "${DEST_DATA_DIR}"
    DEPENDS "${SOURCE_DATA_FILE}"
    COMMENT "Copying data.json to build directory"
)

# ддобавляем копирование файла в процесс сборки
add_custom_target(CopyDataFile ALL DEPENDS "${DEST_DATA_FILE}")

# добавляем зависимость от копирования файла к исполняемому файлу
add_dependencies(Game CopyDataFile)

install(TARGETS Game DESTINATION bin)
