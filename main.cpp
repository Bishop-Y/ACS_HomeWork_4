#include <pthread.h>
#include <iostream>
#include <unistd.h>
#include <vector>
#include <string>
#include <cstdio>

using namespace std;

// Мьютекс для критических секций
pthread_mutex_t busy;

// Количество ингридиентов у посредника
vector<int> ingredients(3);

// Названия ингридиентов
vector<string> name_of_ingredients = {"tobacco", "paper", "match"};

// Флаг, который показывает, какой поток должен совершить действия
int who_print = 0;

// Результирующая строка для вывода в файл
string res;


// Вывод сообщений
void show_message(const string &message) {
    res += message;
    cout << message;
}

// Остановка отработки всех потоков
void pause_all() {
    sleep(2);
}

// Проверка на то, может ли брокер выдать нужное количество ингридиентов для скрутки сигареты
bool check() {
    if (ingredients[0] + ingredients[1] >= 2 || ingredients[1] + ingredients[2] >= 2 ||
        ingredients[0] + ingredients[2] >= 2) {
        return true;
    }
    return false;
}

// Отработка брокера
void *broker_working(void *params) {

    // Число для генерации рандомных чисел
    size_t seed = 0;
    while (!ingredients.empty()) {
        if (who_print == 0) {

            // Отработка только брокера
            pthread_mutex_lock(&busy);

            // Проверка на количество ингридиентов
            if (check()) {
                show_message("- - - - - - - - - - - - - - - -\n");
                show_message("Broker watch for ingredients.\n");
                bool flag = false;
                pause_all();
                show_message("Broker puts: ");
                int index1, index2;
                while (!flag) {
                    srandom(seed);
                    ++seed;

                    // Индексы для выбора ингридиентов
                    index1 = random() % 3;
                    index2 = random() % 3;
                    if (index1 != index2 && ingredients[index1] != 0 && ingredients[index2] != 0) {
                        show_message(name_of_ingredients[index1] + " and " + name_of_ingredients[index2] + ".\n\n");
                        who_print = index1 + index2;
                        flag = true;
                    }
                }

                // Окончание отработки брокера
                pthread_mutex_unlock(&busy);
            } else {

                // Если ингридиентов не хватает, то заканчиваем работу всех потоков
                ingredients.clear();
            }
        }
    }
    show_message("- - - - - - - - - - - - - - - -\n");
    
    // Флаг для вывода сообщения
    int index = -1;
    if (ingredients[0] != 0) {
        index = 0;
    }
    if (ingredients[1] != 0) {
        index = 0;
    }
    if (ingredients[2] != 0) {
        index = 3;
    }
    if (index != -1) {
        show_message("Not enough ingredients. It remains only: " + to_string(ingredients[index]) + " " +
                     name_of_ingredients[index] + ".\n");
    } else {
        show_message("All resources left.\n");
    }
    return nullptr;
}

// Функция курильищка с табаком
void *tobacco_smoker(void *params) {
    while (!ingredients.empty()) {
        if (who_print == 3) {

            // Отнимаем ингридиенты у брокера, которые нужны нашему курильщику
            --ingredients[1];
            --ingredients[2];

            // Отработка только курильщика с табаком
            pthread_mutex_lock(&busy);
            show_message("Smoker with tobacco is smoking.\n");
            pause_all();
            who_print = 0;

            // Окончание отработки курильщика с табаком
            pthread_mutex_unlock(&busy);
        }
    }
    return nullptr;
}

// Функция курильищка с бумагой
void *paper_smoker(void *params) {
    while (!ingredients.empty()) {
        if (who_print == 2) {
            // Отнимаем ингридиенты у брокера, которые нужны нашему курильщику
            --ingredients[0];
            --ingredients[2];

            // Отработка только курильщика с бумагой
            pthread_mutex_lock(&busy);
            show_message("Smoker with paper smokes is smoking.\n");
            pause_all();
            who_print = 0;
            // Окончание отработки курильщика с бумагой
            pthread_mutex_unlock(&busy);
        }
    }
    return nullptr;
}

// Функция курильищка со спичками
void *match_smoker(void *params) {
    while (!ingredients.empty()) {
        if (who_print == 1) {
            // Отнимаем ингридиенты у брокера, которые нужны нашему курильщику
            --ingredients[0];
            --ingredients[1];

            // Отработка только курильщика со спичками
            pthread_mutex_lock(&busy);
            show_message("Smoker with match is smoking.\n");
            pause_all();
            who_print = 0;

            // Окончание отработки курильщика со спичками
            pthread_mutex_unlock(&busy);
        }
    }
    return nullptr;
}


int main(int argc, char *argv[]) {

    // Ввод с консоли
    if (argc == 1) {
        cout << "Type count of tobacco (from 0 to 10): ";
        cin >> ingredients[0];
        cout << "Type count of paper (from 0 to 10): ";
        cin >> ingredients[1];
        cout << "Type count of match (from 0 to 10): ";
        cin >> ingredients[2];
    }

    // Ввод с файла
    if (argc == 3) {
        FILE *input = fopen(argv[1], "r");
        if (input == nullptr) {
            cout << "File does not exist\n";
            return 0;
        } else {
            fscanf(input, "%d", &ingredients[0]);
            fscanf(input, "%d", &ingredients[1]);
            fscanf(input, "%d", &ingredients[2]);
        }
    }

    // Ввод количества ингридиентов с командной строки
    if (argc == 4) {
        ingredients[0] = stoi(argv[1]);
        ingredients[1] = stoi(argv[2]);
        ingredients[2] = stoi(argv[3]);
    }

    cout << '\n';

    // Объявление потоков
    pthread_t match, paper, tobacco, broker;

    // Создание потоков
    pthread_create(&broker, nullptr, broker_working, nullptr);
    pthread_create(&match, nullptr, match_smoker, nullptr);
    pthread_create(&paper, nullptr, paper_smoker, nullptr);
    pthread_create(&tobacco, nullptr, tobacco_smoker, nullptr);

    // Завершение отработки потоков
    pthread_join(broker, nullptr);
    pthread_join(match, nullptr);
    pthread_join(paper, nullptr);
    pthread_join(tobacco, nullptr);

    // Вывод в файл
    if (argc == 3) {
        FILE *output = fopen(argv[2], "w");
        fprintf(output, "%s", res.data());
    }
}
