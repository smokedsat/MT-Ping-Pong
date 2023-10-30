cmake_minimum_required(VERSION 3.0)
project(Game)

add_executable(Game src/main.cpp)

# Указываем путь к заголовочным файлам
target_include_directories(Game PRIVATE
    include  # Путь к папке с заголовочными файлами
)

# Устанавливаем стандарт C++
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Путь к исходному файлу data.json
set(SOURCE_DATA_FILE "${CMAKE_CURRENT_SOURCE_DIR}/data.json")

# Копируем data.json
set(DEST_DATA_DIR "${CMAKE_BINARY_DIR}")

# Имя для скопированного файла то же.
set(DEST_DATA_FILE "${DEST_DATA_DIR}/data.json")

# Копирование файла
add_custom_command(
    OUTPUT "${DEST_DATA_FILE}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${SOURCE_DATA_FILE}" "${DEST_DATA_DIR}"
    DEPENDS "${SOURCE_DATA_FILE}"
    COMMENT "Copying data.json to build directory"
)

# Добавляем копирование файла в процесс сборки
add_custom_target(CopyDataFile ALL DEPENDS "${DEST_DATA_FILE}")

# Добавляем зависимость от копирования файла к исполняемому файлу
add_dependencies(Game CopyDataFile)

# Устанавливаем цель для исполняемого файла
install(TARGETS Game DESTINATION bin)
