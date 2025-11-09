#include <iostream>
using namespace std;

// ====================== ВСПОМОГАТЕЛЬНОЕ ======================
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
inline unsigned int hash32(unsigned int x) { return x * 2654435761u; }

// ====================== ДВУСВЯЗНЫЙ СПИСОК (LRU порядок) ======================
struct Node
{
    int key, val;
    Node *prev;
    Node *next;
    Node(int k, int v) : key(k), val(v), prev(0), next(0) {}
};

struct LRUList
{
    Node *head = 0; // MRU
    Node *tail = 0; // LRU

    void push_front(Node *n)
    {
        n->prev = 0;
        n->next = head;
        if (head)
            head->prev = n;
        head = n;
        if (!tail)
            tail = n;
    }
    void remove(Node *n)
    {
        if (n->prev)
            n->prev->next = n->next;
        else
            head = n->next;
        if (n->next)
            n->next->prev = n->prev;
        else
            tail = n->prev;
        n->prev = n->next = 0;
    }
    void move_to_front(Node *n)
    {
        if (head == n)
            return;
        remove(n);
        push_front(n);
    }
    Node *pop_back()
    { // вернуть LRU
        Node *r = tail;
        if (!r)
            return 0;
        remove(r);
        return r;
    }
};

// ====================== ХЕШ-Таблица (откр. адресация) key -> Node* ======================
struct PtrHashMap
{
    int cap;
    int size;
    int *keys;
    Node **vals;
    unsigned char *state; // 0 пусто, 1 занято, 2 удалено

    void init(int expect)
    {
        cap = nextPrime(expect);
        size = 0;
        keys = new int[cap];
        vals = new Node *[cap];
        state = new unsigned char[cap];
        for (int i = 0; i < cap; ++i)
            state[i] = 0;
    }
    void clear()
    {
        if (!keys)
            return;
        delete[] keys;
        delete[] vals;
        delete[] state;
        keys = 0;
        vals = 0;
        state = 0;
        cap = 0;
        size = 0;
    }
    int findIndex(int key) const
    {
        unsigned int h = hash32((unsigned)key) % (unsigned)cap;
        int start = (int)h;
        for (int k = 0; k < cap; ++k)
        {
            int i = (start + k) % cap;
            if (state[i] == 0)
                return -1; // пустая — не найден
            if (state[i] == 1 && keys[i] == key)
                return i; // найден
        }
        return -1;
    }
    Node *get(int key) const
    {
        int i = findIndex(key);
        return (i == -1) ? 0 : vals[i];
    }
    void put(int key, Node *val)
    {
        // вставка/обновление
        unsigned int h = hash32((unsigned)key) % (unsigned)cap;
        int start = (int)h;
        int firstDel = -1;
        for (int k = 0; k < cap; ++k)
        {
            int i = (start + k) % cap;
            if (state[i] == 1)
            {
                if (keys[i] == key)
                {
                    vals[i] = val;
                    return;
                }
            }
            else if (state[i] == 2)
            {
                if (firstDel == -1)
                    firstDel = i;
            }
            else
            { // пусто
                if (firstDel != -1)
                    i = firstDel;
                keys[i] = key;
                vals[i] = val;
                state[i] = 1;
                ++size;
                return;
            }
        }
        // На учебной задаче можно не делать rehash; в проде тут расширение.
    }
    void erase(int key)
    {
        int i = findIndex(key);
        if (i != -1)
        {
            state[i] = 2;
            vals[i] = 0;
        }
    }
};

// ====================== LRU Cache ======================
struct LRUCache
{
    int capacity;
    int curSize;
    LRUList order;
    PtrHashMap map;

    void init(int cap)
    {
        capacity = (cap < 0) ? 0 : cap;
        curSize = 0;
        // таблицу делаем побольше, чтобы коллизий меньше (≈4*cap + запас)
        int mapCap = (capacity > 0) ? (capacity * 4 + 7) : 11;
        map.init(mapCap);
    }
    void clear()
    {
        // удалить все узлы списка
        Node *c = order.head;
        while (c)
        {
            Node *nx = c->next;
            delete c;
            c = nx;
        }
        order.head = order.tail = 0;
        map.clear();
        capacity = 0;
        curSize = 0;
    }

    int get(int key)
    {
        if (capacity <= 0)
            return -1;
        Node *n = map.get(key);
        if (!n)
            return -1;
        order.move_to_front(n);
        return n->val;
    }

    void set(int key, int value)
    {
        if (capacity <= 0)
            return;
        Node *n = map.get(key);
        if (n)
        {
            n->val = value;
            order.move_to_front(n);
            return;
        }
        // новый
        Node *m = new Node(key, value);
        order.push_front(m);
        map.put(key, m);
        ++curSize;

        if (curSize > capacity)
        {
            // вытеснение LRU (хвост)
            Node *old = order.pop_back();
            // pop_back уже удалил из списка; old == только что вынутый
            map.erase(old->key);
            delete old;
            --curSize;
        }
    }
};

// ====================== Разбор команд и выполнение ======================
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int cap, Q;
    if (!(cin >> cap >> Q))
        return 0;

    LRUCache cache;
    cache.init(cap);

    bool firstOut = true; // формат вывода: результаты GET в одну строку через пробел

    for (int i = 0; i < Q; ++i)
    {
        char cmd[8]; // "SET" или "GET"
        cin >> cmd;
        if (cmd[0] == 'S' && cmd[1] == 'E' && cmd[2] == 'T')
        {
            int x, y;
            cin >> x >> y;
            cache.set(x, y);
        }
        else if (cmd[0] == 'G' && cmd[1] == 'E' && cmd[2] == 'T')
        {
            int x;
            cin >> x;
            int res = cache.get(x);
            if (!firstOut)
                cout << ' ';
            cout << res;
            firstOut = false;
        }
        else
        {
            // неизвестная команда — проигнорируем её аргументы (на всякий случай)
            // но по условию приходит только SET/GET
        }
    }
    if (!firstOut)
        cout << "\n";

    cache.clear();
    return 0;
}
