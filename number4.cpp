#include <iostream>
using namespace std;

// -------------------------
// Узел связного списка (универсальный тип)
// -------------------------
template <typename T>
struct Node
{
    T value;
    Node *next;
};

// Создание нового узла
template <typename T>
Node<T> *createNode(T v)
{
    Node<T> *n = new Node<T>;
    n->value = v;
    n->next = 0;
    return n;
}

// Добавление элемента в конец списка
template <typename T>
void appendNode(Node<T> *&head, T v)
{
    Node<T> *n = createNode(v);
    if (!head)
    {
        head = n;
        return;
    }
    Node<T> *cur = head;
    while (cur->next)
        cur = cur->next;
    cur->next = n;
}

// Освобождение памяти
template <typename T>
void freeList(Node<T> *&head)
{
    while (head)
    {
        Node<T> *tmp = head;
        head = head->next;
        delete tmp;
    }
}

// Печать списка в виде {a, b, c}
template <typename T>
void printList(Node<T> *head)
{
    cout << "{";
    Node<T> *cur = head;
    int first = 1;
    while (cur)
    {
        if (!first)
            cout << ", ";
        cout << cur->value;
        first = 0;
        cur = cur->next;
    }
    cout << "}";
}

// -------------------------
// Печать подмножества по битовой маске
// -------------------------
template <typename T>
void printSubset(T arr[], int n, int mask)
{
    cout << "{";
    int first = 1;
    for (int i = 0; i < n; ++i)
    {
        if (mask & (1 << i))
        {
            if (!first)
                cout << ", ";
            cout << arr[i];
            first = 0;
        }
    }
    cout << "}";
}

// -------------------------
// Главная программа
// -------------------------
int main()
{
    int n;
    cout << "Введите количество элементов массива: ";
    cin >> n;
    if (n <= 0 || n > 20)
    {
        cout << "Ошибка: некорректное количество (1–20).\n";
        return 0;
    }

    cout << "Выберите тип элементов (1 — числа, 2 — строки): ";
    int type;
    cin >> type;

    if (type == 1)
    {
        // ----- ЦЕЛЫЕ ЧИСЛА -----
        Node<int> *head = 0;
        int arr[20];

        cout << "Введите элементы массива: ";
        for (int i = 0; i < n; ++i)
        {
            int v;
            cin >> v;
            arr[i] = v;
            appendNode(head, v);
        }

        cout << "\nИсходный массив S = ";
        printList(head);
        cout << "\n\nВсе различные подмассивы (подмножества):\n";

        int total = 1 << n;
        for (int mask = 0; mask < total; ++mask)
        {
            printSubset(arr, n, mask);
            cout << "\n";
        }

        freeList(head);
    }
    else if (type == 2)
    {
        // ----- СТРОКИ -----
        Node<string> *head = 0;
        string arr[20];

        cout << "Введите элементы массива (строки): ";
        for (int i = 0; i < n; ++i)
        {
            string v;
            cin >> v;
            arr[i] = v;
            appendNode(head, v);
        }

        cout << "\nИсходный массив S = ";
        printList(head);
        cout << "\n\nВсе различные подмассивы (подмножества):\n";

        int total = 1 << n;
        for (int mask = 0; mask < total; ++mask)
        {
            printSubset(arr, n, mask);
            cout << "\n";
        }

        freeList(head);
    }
    else
    {
        cout << "Ошибка: неверный тип.\n";
    }

    return 0;
}
