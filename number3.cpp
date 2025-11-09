#include <iostream>
using namespace std;

// -------------------------
// Узел списка (множество)
// -------------------------
struct Node
{
    int value;
    Node *next;
};

// Создание нового узла
Node *createNode(int v)
{
    Node *n = new Node;
    n->value = v;
    n->next = 0;
    return n;
}

// Добавление в конец списка
void appendNode(Node *&head, int v)
{
    Node *n = createNode(v);
    if (!head)
    {
        head = n;
        return;
    }
    Node *cur = head;
    while (cur->next)
        cur = cur->next;
    cur->next = n;
}

// Вывод множества
void printSet(Node *head)
{
    cout << "{";
    Node *cur = head;
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

// Освобождение памяти
void freeList(Node *&head)
{
    while (head)
    {
        Node *tmp = head;
        head = head->next;
        delete tmp;
    }
}

// -------------------------
// Простая сортировка (по убыванию)
// -------------------------
void sortDesc(int arr[], int n)
{
    for (int i = 0; i < n - 1; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            if (arr[j] > arr[i])
            {
                int t = arr[i];
                arr[i] = arr[j];
                arr[j] = t;
            }
        }
    }
}

// -------------------------
// Backtracking: раскладываем числа по "корзинам" суммой K
// -------------------------
bool dfs(int idx, int n, int nums[], int m, int K, int *bucketSum, int **buckets, int *bucketCount)
{
    if (idx == n)
        return 1;

    int x = nums[idx];
    for (int i = 0; i < m; ++i)
    {
        if (bucketSum[i] + x <= K)
        {
            bucketSum[i] += x;
            buckets[i][bucketCount[i]++] = x;

            if (dfs(idx + 1, n, nums, m, K, bucketSum, buckets, bucketCount))
                return 1;

            bucketSum[i] -= x;
            bucketCount[i]--;

            if (bucketSum[i] == 0)
                break; // чтобы не плодить одинаковые пустые корзины
        }
    }
    return 0;
}

// -------------------------
// Главная программа
// -------------------------
int main()
{
    int n;
    cout << "Введите количество элементов множества: ";
    if (!(cin >> n) || n <= 0)
    {
        cout << "Ошибка ввода.\n";
        return 0;
    }

    Node *head = 0;
    int nums[100]; // максимум 100 элементов
    if (n > 100)
    {
        cout << "Слишком много элементов (максимум 100).\n";
        return 0;
    }

    cout << "Введите элементы множества: ";
    for (int i = 0; i < n; ++i)
    {
        int v;
        cin >> v;
        if (v <= 0)
        {
            cout << "Ошибка: только натуральные числа.\n";
            freeList(head);
            return 0;
        }
        nums[i] = v;
        appendNode(head, v);
    }

    int K;
    cout << "Введите целевую сумму K: ";
    cin >> K;
    if (K <= 0)
    {
        cout << "Ошибка: K должно быть ббб.\n";
        freeList(head);
        return 0;
    }

    cout << "Исходное множество S = ";
    printSet(head);
    cout << "\n";

    // Проверки
    long long total = 0;
    for (int i = 0; i < n; ++i)
        total += nums[i];

    if (total % K != 0)
    {
        cout << "Ошибка: сумма элементов " << total << " не кратна " << K << ".\n";
        freeList(head);
        return 0;
    }

    int m = total / K;
    if (m > n)
    {
        cout << "Ошибка: слишком много подмножеств.\n";
        freeList(head);
        return 0;
    }

    for (int i = 0; i < n; ++i)
        if (nums[i] > K)
        {
            cout << "Ошибка: элемент " << nums[i] << " больше K=" << K << ".\n";
            freeList(head);
            return 0;
        }

    sortDesc(nums, n); // упорядочим числа по убыванию

    int bucketSum[100] = {0};
    int *buckets[100];
    int bucketCount[100] = {0};
    for (int i = 0; i < m; ++i)
        buckets[i] = new int[n]; // запас

    bool ok = dfs(0, n, nums, m, K, bucketSum, buckets, bucketCount);

    if (!ok)
    {
        cout << "Разбиение невозможно.\n";
    }
    else
    {
        cout << "Разбиение найдено (" << m << " подмножеств):\n";
        for (int i = 0; i < m; ++i)
        {
            cout << i + 1 << ") {";
            int sum = 0;
            for (int j = 0; j < bucketCount[i]; ++j)
            {
                if (j)
                    cout << ", ";
                cout << buckets[i][j];
                sum += buckets[i][j];
            }
            cout << "}  (сумма = " << sum << ")\n";
        }
    }

    for (int i = 0; i < m; ++i)
        delete[] buckets[i];

    freeList(head);
    return 0;
}
