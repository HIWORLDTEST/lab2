#include <iostream>
#include <string>
using namespace std;

// === Стек для строк ===
struct StackNode
{
    string value;
    StackNode *next;
};

struct StringStack
{
    StackNode *head;

    StringStack() : head(nullptr) {}

    ~StringStack()
    {
        clear();
    }

    bool empty() const
    {
        return head == nullptr;
    }

    void push(const string &val)
    {
        StackNode *node = new StackNode;
        node->value = val;
        node->next = head;
        head = node;
    }

    const string &top() const
    {
        return head->value; // вызывается, если стек не пуст
    }

    void pop()
    {
        if (!head)
            return;
        StackNode *tmp = head;
        head = head->next;
        delete tmp;
    }

    void clear()
    {
        while (head)
        {
            pop();
        }
    }
};

// === Проверка корректности XML ===
bool isValidXML(const string &s)
{
    int n = (int)s.size();

    if (n == 0)
        return true;
    if (s[0] != '<' || s[n - 1] != '>')
        return false;

    StringStack st;
    int i = 0;

    while (i < n)
    {
        if (s[i] != '<')
            return false;

        int j = i + 1;
        bool closing = false;

        if (j < n && s[j] == '/')
        {
            closing = true;
            ++j;
        }

        int nameStart = j;

        while (j < n && s[j] >= 'a' && s[j] <= 'z')
        {
            ++j;
        }

        if (nameStart == j)
            return false; // пустое имя
        if (j >= n || s[j] != '>')
            return false; // нет '>'

        string tag = s.substr(nameStart, j - nameStart);

        if (closing)
        {
            if (st.empty() || st.top() != tag)
            {
                return false;
            }
            st.pop();
        }
        else
        {
            st.push(tag);
        }

        i = j + 1;
    }

    return st.empty();
}

// === Основная программа ===
int main()
{
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string input;
    if (!getline(cin, input))
    {
        return 0;
    }

    const string symbols = "abcdefghijklmnopqrstuvwxyz<>/";
    string best;
    bool found = false;

    for (size_t i = 0; i < input.size(); ++i)
    {
        char original = input[i];

        for (size_t j = 0; j < symbols.size(); ++j)
        {
            char c = symbols[j];

            if (c == original)
            {
                continue;
            }

            string candidate = input;
            candidate[i] = c;

            if (isValidXML(candidate))
            {
                if (!found || candidate < best)
                {
                    best = candidate;
                    found = true;
                }
            }
        }
    }

    if (found)
    {
        cout << best << '\n';
    }
    else
    {
        cout << "NO SOLUTION\n";
    }

    return 0;
}
//<a><b></b><d></c></a>