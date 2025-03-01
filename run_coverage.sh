#!/bin/bash

# Цвета для вывода
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
YELLOW='\033[0;33m'
NC='\033[0m' # No Color

# Определяем количество ядер процессора для параллельной сборки
if [ -x "$(command -v sysctl)" ]; then
    # macOS
    NUM_CORES=$(sysctl -n hw.ncpu)
elif [ -x "$(command -v nproc)" ]; then
    # Linux
    NUM_CORES=$(nproc)
else
    NUM_CORES=2
fi

# Создаем структуру директории для покрытия если она отсутствует
echo -e "${BLUE}Создание директорий для хранения отчетов о покрытии...${NC}"
mkdir -p coverage/html
mkdir -p coverage/details

# Удаляем старые файлы покрытия
echo -e "${BLUE}Очистка старых файлов покрытия...${NC}"
rm -f coverage/*.profraw coverage/*.profdata
rm -rf coverage/html/* coverage/details/*

# Создаем build_cov директорию если её нет
mkdir -p build_cov

# Пытаемся сконфигурировать проект с нужными флагами
echo -e "${BLUE}Конфигурация проекта...${NC}"
cmake -DCODE_COVERAGE=ON -B build_cov

# Проверяем код возврата cmake
CMAKE_EXIT_CODE=$?

# Если конфигурация не удалась (например, из-за несоответствия флагов),
# удаляем build_cov директорию и пробуем заново
if [ $CMAKE_EXIT_CODE -ne 0 ]; then
    echo -e "${YELLOW}Конфигурация не удалась. Выполняем полную пересборку...${NC}"
    rm -rf build_cov
    mkdir -p build_cov
    cmake -DCMAKE_CXX_FLAGS="-fprofile-instr-generate -fcoverage-mapping" -B build_cov
    
    # Если и это не помогло - выходим
    if [ $? -ne 0 ]; then
        echo -e "${RED}Ошибка при конфигурации проекта${NC}"
        exit 1
    fi
fi

# Собираем проект
echo -e "${BLUE}Сборка проекта...${NC}"
cmake --build build_cov -- -j$NUM_CORES

# Проверяем успешность сборки
if [ $? -ne 0 ]; then
    echo -e "${RED}Ошибка при сборке проекта${NC}"
    exit 1
fi

# Запуск тестов и генерация профиля
echo -e "${BLUE}Запуск тестов и сбор данных о покрытии...${NC}"
cd build_cov
LLVM_PROFILE_FILE="${PWD}/../coverage/coverage.profraw" ./tests/json_test
TEST_EXIT_CODE=$?
cd ..

# Проверяем результаты тестов
if [ $TEST_EXIT_CODE -ne 0 ]; then
    echo -e "${YELLOW}Внимание: Некоторые тесты не прошли. Покрытие может быть неполным.${NC}"
fi

# Проверяем, что профиль создан
if [ ! -f coverage/coverage.profraw ]; then
    echo -e "${RED}Ошибка: файл профиля не создан${NC}"
    exit 1
fi

# Создание отчета о покрытии
echo -e "${BLUE}Создание отчета о покрытии...${NC}"
xcrun llvm-profdata merge -sparse coverage/coverage.profraw -o coverage/coverage.profdata

# Находим все исходные файлы в папке json
echo -e "${BLUE}Поиск всех исходных файлов в папке json...${NC}"
SRC_FILES=$(find json -type f -name "*.cpp" -o -name "*.h" | tr '\n' ' ')

# Создание текстового отчета о покрытии со всеми исходными файлами
echo -e "${BLUE}Создание текстового отчета для всех исходных файлов...${NC}"
xcrun llvm-cov show build_cov/tests/json_test -instr-profile=coverage/coverage.profdata $SRC_FILES -use-color=false > coverage/details/coverage.txt
xcrun llvm-cov report build_cov/tests/json_test -instr-profile=coverage/coverage.profdata $SRC_FILES > coverage/details/summary.txt

# Создание HTML отчета о покрытии
echo -e "${BLUE}Создание HTML отчета...${NC}"
xcrun llvm-cov show build_cov/tests/json_test -instr-profile=coverage/coverage.profdata $SRC_FILES -format=html -o coverage/html

# Вывод итогов
echo -e "${GREEN}Покрытие кода тестами выполнено успешно!${NC}"
echo -e "${GREEN}Отчеты доступны:${NC}"
echo -e "  - Текстовый отчет: ${BLUE}coverage/details/coverage.txt${NC}"
echo -e "  - Сводка покрытия: ${BLUE}coverage/details/summary.txt${NC}"
echo -e "  - HTML отчет: ${BLUE}coverage/html/index.html${NC}"

# Извлечение и вывод общего процента покрытия
COVERAGE=$(grep "TOTAL" coverage/details/summary.txt | awk '{print $NF}')
if [ -n "$COVERAGE" ]; then
    echo -e "${GREEN}Общее покрытие кода: ${BLUE}$COVERAGE${NC}"
fi

exit 0