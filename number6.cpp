#include <iostream>
#include <ctime>
#include <cstdlib>
using namespace std;

// Только базовые функции, без STL

// -------------------- Строковые утилиты --------------------
inline int mystrlen(const char *s)
{
    int n = 0;
    while (s[n] != '\0')
        ++n;
    return n;
}

// -------------------- Математические --------------------
inline bool isPrime(int x)
{
    if (x < 2)
        return false;
    if (x % 2 == 0)
        return x == 2;
    for (int d = 3; d * d <= x; d += 2)
        if (x % d == 0)
            return false;
    return true;
}

inline int nextPrime(int x)
{
    if (x <= 2)
        return 2;
    if (x % 2 == 0)
        ++x;
    while (!isPrime(x))
        x += 2;
    return x;
}

inline int absInt(int x)
{
    return x < 0 ? -x : x;
}

// -------------------- Хеш-функция --------------------
// Мультипликативный хеш Кнута (32-бит)
inline unsigned int hash32(unsigned int x)
{
    return (x * 2654435761u);
}

// ========================== МЕТОД ЦЕПОЧЕК ==========================

struct ListNode
{
    int key;
    ListNode *next;
};

struct HashChaining
{
    ListNode **buckets;
    int capacity;
    int size;

    void init(int cap)
    {
        capacity = nextPrime(cap);
        size = 0;
        buckets = new ListNode *[capacity];
        for (int i = 0; i < capacity; ++i)
            buckets[i] = 0;
    }

    void clear()
    {
        if (!buckets)
            return;
        for (int i = 0; i < capacity; ++i)
        {
            ListNode *cur = buckets[i];
            while (cur)
            {
                ListNode *nx = cur->next;
                delete cur;
                cur = nx;
            }
        }
        delete[] buckets;
        buckets = 0;
        capacity = 0;
        size = 0;
    }

    int indexOf(int key)
    {
        unsigned int h = hash32((unsigned)key);
        return (int)(h % (unsigned)capacity);
    }

    bool find(int key)
    {
        int idx = indexOf(key);
        ListNode *cur = buckets[idx];
        while (cur)
        {
            if (cur->key == key)
                return true;
            cur = cur->next;
        }
        return false;
    }

    void insert(int key)
    {
        if (find(key))
            return;
        int idx = indexOf(key);
        ListNode *n = new ListNode;
        n->key = key;
        n->next = buckets[idx];
        buckets[idx] = n;
        ++size;
    }
};

// ========================== ОТКРЫТАЯ АДРЕСАЦИЯ ==========================

struct HashOpenAddress
{
    int *keys;
    unsigned char *state; // 0=пусто, 1=занято, 2=удалено
    int capacity;
    int size;

    void init(int cap)
    {
        capacity = nextPrime(cap);
        size = 0;
        keys = new int[capacity];
        state = new unsigned char[capacity];
        for (int i = 0; i < capacity; ++i)
            state[i] = 0;
    }

    void clear()
    {
        if (!keys)
            return;
        delete[] keys;
        delete[] state;
        keys = 0;
        state = 0;
        capacity = 0;
        size = 0;
    }

    int probeIndex(int key)
    {
        unsigned int h = hash32((unsigned)key) % (unsigned)capacity;
        int start = (int)h;
        for (int k = 0; k < capacity; ++k)
        {
            int i = (start + k) % capacity;
            if (state[i] == 0)
                return -1;
            if (state[i] == 1 && keys[i] == key)
                return i;
        }
        return -1;
    }

    bool find(int key) { return probeIndex(key) != -1; }

    void insert(int key)
    {
        if (find(key))
            return;
        unsigned int h = hash32((unsigned)key) % (unsigned)capacity;
        int start = (int)h;
        int firstDel = -1;
        for (int k = 0; k < capacity; ++k)
        {
            int i = (start + k) % capacity;
            if (state[i] == 1)
            {
                if (keys[i] == key)
                    return;
            }
            else if (state[i] == 2)
            {
                if (firstDel == -1)
                    firstDel = i;
            }
            else
            {
                if (firstDel != -1)
                    i = firstDel;
                keys[i] = key;
                state[i] = 1;
                ++size;
                return;
            }
        }
    }
};

// ========================== ЭМПИРИЧЕСКИЙ АНАЛИЗ ==========================

void benchmarkSearches(int *data, int N, int *queries, int M)
{
    int capChain = nextPrime(N * 2 + 1);
    int capOpen = nextPrime(N * 3 + 1);

    HashChaining Hc;
    Hc.init(capChain);
    HashOpenAddress Ho;
    Ho.init(capOpen);

    for (int i = 0; i < N; ++i)
    {
        Hc.insert(data[i]);
        Ho.insert(data[i]);
    }

    clock_t t1 = clock();
    int hitsC = 0;
    for (int i = 0; i < M; ++i)
        if (Hc.find(queries[i]))
            ++hitsC;
    clock_t t2 = clock();

    clock_t t3 = clock();
    int hitsO = 0;
    for (int i = 0; i < M; ++i)
        if (Ho.find(queries[i]))
            ++hitsO;
    clock_t t4 = clock();

    double timeC = double(t2 - t1) / CLOCKS_PER_SEC;
    double timeO = double(t4 - t3) / CLOCKS_PER_SEC;

    cout << "\n=== Эмпирический анализ поиска (" << M << " запросов) ===\n";
    cout << "Таблица (цепочки):   найдено " << hitsC << ", время " << timeC << " c\n";
    cout << "Таблица (откр.адр.): найдено " << hitsO << ", время " << timeO << " c\n";

    Hc.clear();
    Ho.clear();
}

// ========================== ИЗОМОРФНЫЕ СТРОКИ ==========================

bool isomorphic(const char *a, const char *b)
{
    int n1 = mystrlen(a), n2 = mystrlen(b);
    if (n1 != n2)
        return false;

    int mapAB[256], mapBA[256];
    for (int i = 0; i < 256; ++i)
    {
        mapAB[i] = -1;
        mapBA[i] = -1;
    }

    for (int i = 0; i < n1; ++i)
    {
        unsigned char x = (unsigned char)a[i];
        unsigned char y = (unsigned char)b[i];
        if (mapAB[x] == -1 && mapBA[y] == -1)
        {
            mapAB[x] = y;
            mapBA[y] = x;
        }
        else
        {
            if (mapAB[x] != y || mapBA[y] != x)
                return false;
        }
    }
    return true;
}

// ========================== MAIN ==========================

int main()
{
    cout << "Выберите режим:\n";
    cout << "1) Эмпирический анализ хеш-таблиц (поиск)\n";
    cout << "2) Проверка изоморфности двух строк\n";
    cout << "Ваш выбор: ";
    int mode;
    if (!(cin >> mode))
        return 0;

    if (mode == 1)
    {
        int N;
        cout << "Введите N (число элементов): ";
        cin >> N;
        if (N <= 0 || N > 100000)
        {
            cout << "Некорректный N (1..100000).\n";
            return 0;
        }

        int *data = new int[N];
        cout << "Введите " << N << " целых элементов:\n";
        for (int i = 0; i < N; ++i)
            cin >> data[i];

        int M;
        cout << "Введите M (число поисковых запросов): ";
        cin >> M;
        if (M <= 0 || M > 200000)
        {
            cout << "Некорректный M (1..200000).\n";
            delete[] data;
            return 0;
        }

        int *q = new int[M];
        cout << "Введите " << M << " значений для поиска:\n";
        for (int i = 0; i < M; ++i)
            cin >> q[i];

        benchmarkSearches(data, N, q, M);

        delete[] q;
        delete[] data;
    }
    else if (mode == 2)
    {
        static char A[100005], B[100005];
        cout << "Введите строку A: ";
        cin >> A;
        cout << "Введите строку B: ";
        cin >> B;
        bool ok = isomorphic(A, B);
        cout << (ok ? "Изоморфны\n" : "Не изоморфны\n");
    }
    else
    {
        cout << "Неизвестный режим.\n";
    }
    return 0;
}
