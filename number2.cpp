#include <iostream>
#include <string>
#include <fstream>
using namespace std;

const int TABLE_SIZE = 100003; // Простое число для хеш-таблицы

struct HashSet
{
    enum State
    {
        EMPTY,
        FILLED,
        DELETED
    };

    struct Bucket
    {
        int key;
        State state;
        Bucket() : key(0), state(EMPTY) {}
    };

    Bucket table[TABLE_SIZE];

    int hash(int x) const
    {
        return (x % TABLE_SIZE + TABLE_SIZE) % TABLE_SIZE;
    }

    bool contains(int x) const
    {
        int h = hash(x);
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            int idx = (h + i) % TABLE_SIZE;
            if (table[idx].state == EMPTY)
                return false;
            if (table[idx].state == FILLED && table[idx].key == x)
                return true;
        }
        return false;
    }

    void add(int x)
    {
        if (contains(x))
            return;
        int h = hash(x);
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            int idx = (h + i) % TABLE_SIZE;
            if (table[idx].state != FILLED)
            {
                table[idx].key = x;
                table[idx].state = FILLED;
                return;
            }
        }
        cerr << "Ошибка: таблица переполнена\n";
    }

    void remove(int x)
    {
        int h = hash(x);
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            int idx = (h + i) % TABLE_SIZE;
            if (table[idx].state == EMPTY)
                return;
            if (table[idx].state == FILLED && table[idx].key == x)
            {
                table[idx].state = DELETED;
                return;
            }
        }
    }
};

// Загрузка из файла
void loadFromFile(HashSet &s, const string &filename)
{
    ifstream file(filename);
    if (!file.is_open())
    {
        cerr << "Ошибка: не удалось открыть файл " << filename << endl;
        return;
    }
    int value;
    while (file >> value)
    {
        s.add(value);
    }
    file.close();
}

// Сохранение в файл
void saveToFile(const HashSet &s, const string &filename)
{
    ofstream file(filename, ios::trunc);
    if (!file.is_open())
    {
        cerr << "Ошибка: не удалось открыть файл для записи " << filename << endl;
        return;
    }
    for (int i = 0; i < TABLE_SIZE; ++i)
    {
        if (s.table[i].state == HashSet::FILLED)
        {
            file << s.table[i].key << "\n";
        }
    }
    file.close();
}

int main(int argc, char *argv[])
{
    if (argc < 5)
    {
        cerr << "Использование: ./program --file <путь> --query <операция> [значение]\n";
        cerr << "Операции: SETADD <x>, SETDEL <x>, SET_AT <x>\n";
        return 1;
    }

    string fileFlag = argv[1];
    string filename = argv[2];
    string queryFlag = argv[3];
    string query = argv[4];

    HashSet mySet;
    loadFromFile(mySet, filename);

    if (query == "SETADD")
    {
        if (argc < 6)
        {
            cerr << "Ошибка: не указано значение для SETADD\n";
            return 1;
        }
        int value = stoi(argv[5]);
        mySet.add(value);
        saveToFile(mySet, filename);
        cout << "Добавлено: " << value << endl;
    }
    else if (query == "SETDEL")
    {
        if (argc < 6)
        {
            cerr << "Ошибка: не указано значение для SETDEL\n";
            return 1;
        }
        int value = stoi(argv[5]);
        mySet.remove(value);
        saveToFile(mySet, filename);
        cout << "Удалено: " << value << endl;
    }
    else if (query == "SET_AT")
    {
        if (argc < 6)
        {
            cerr << "Ошибка: не указано значение для SET_AT\n";
            return 1;
        }
        int value = stoi(argv[5]);
        if (mySet.contains(value))
            cout << "YES\n";
        else
            cout << "NO\n";
    }
    else
    {
        cerr << "Неизвестная операция: " << query << endl;
        return 1;
    }

    return 0;
}
