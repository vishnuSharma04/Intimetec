#include <stdio.h>
#include <ctype.h>
#include <string.h>

const char* p; 
int err;       

void skip() {
while (*p && isspace(*p)) p++;
}

int num() {
    skip();
    if (!isdigit(*p)) {
err = 2; return 0;
}
    int v = 0;
    while (isdigit(*p)) {
      v = v * 10 + (*p - '0'); p++; 
    }
    return v;
}

int fact() {
  return num(); 
}

int term() {
    int v = fact();
    while (!err) {
        skip();
        if (*p == '*') { p++; v *= fact(); }
        else if (*p == '/') { 
            p++; 
            int n = fact();
            if (n == 0) {
              err = 1;
              return 0; 
        }
            v /= n; 
        } else break;
    }
    return v;
}

int expr() {
    int v = term();
    while (!err) {
        skip();
        if (*p == '+') {
          p++; v += term();
        }
        else if (*p == '-') {
          p++; v -= term();
        }
        else break;
    }
    return v;
}

int main() {
    char buf[256];
    if (!fgets(buf, sizeof(buf), stdin)) return 0;
    buf[strcspn(buf, "\n")] = 0;

    p = buf; err = 0;
    int v = expr();
    skip();
    if (*p) err = 2;

    if (err == 1) printf("Error: Division by zero.\n");
    else if (err == 2 || buf[0] == '\0') printf("Error: Invalid expression.\n");
    else printf("%d\n", v);
    return 0;
}