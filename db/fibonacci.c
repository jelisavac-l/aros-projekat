#include <stdio.h>

int fibonacci(int n) {
    if (n <= 1) {
        return n;
    } else {
        return fibonacci(n-1) + fibonacci(n-2);
    }
}

int main() {
    int n;

    printf("Enter the position in the sequence: ");
    scanf("%d", &n);
    printf("Fibonacci number at position %d: %d\n", n, fibonacci(n));

    return 0;
}