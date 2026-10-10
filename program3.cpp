#include <iostream>
#include <cstdlib>

using namespace std;

void SetValue(int *a, int *b)
{
    int sum = *a + *b;
    int diff = abs(*a - *b);

    *a = sum;
    *b = diff;
}

int main()
{
    int a = 10;
    int b = 6;

    SetValue(&a, &b);

    cout << "First value (Sum): " << a << endl;
    cout << "Second value (Difference): " << b << endl;

    return 0;
}