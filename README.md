# Terracraft Valrising Bot

Бот для автоматического прохождения подземелья в игре Terracraft Valrising. Реализация тестового задания.

## Архитектура

```text
Game (симулятор)
├── Map          - полная карта подземелья, загрузка из файла
├── Player       - игрок
├── Logger       - запись действий в result.txt
└── IStrategyBot - интерфейс бота
└── AliceBot     - реализация алгоритма Алисы
```

## Сборка

```bash
mkdir build && cd build
cmake ..
make
Запуск
bash
./game <in_file>
cat result.txt
```

## Тесты
Тесты лежат в tests/. 

Формат:
  .in       - входные данные
  .expected - ожидаемый вывод

```bash
bash
cd tests
sh run_tests.sh
```

## Улучшения алгоритма
Предложения по улучшению бота Алисы описаны в improvements.md
