#include <stdio.h>
#include <ctype.h>
#include <string.h>

typedef enum {
    ERRNONE = 0,
    ERRDIVZERO,
    ERRINVALID
} ErrorCode;

static const char* ptr;
static ErrorCode err;

static void skipSpace(void);
static int readNumber(void);
static int readPrimary(void);
static int readTerm(void);
static int readExpr(void);

static void skipSpace(void) {
    while (*ptr && isspace((unsigned char)*ptr)) ptr++;
}

static int readNumber(void) {
    skipSpace();
    if (!isdigit((unsigned char)*ptr)) {
        err = ERRINVALID;
        return 0;
    }
    int val = 0;
    while (isdigit((unsigned char)*ptr)) {
        val = val * 10 + (*ptr - '0');
        ptr++;
    }
    return val;
}

static int readPrimary(void) {
    skipSpace();
    if (*ptr == '(') {
        ptr++;
        int v = readExpr();
        skipSpace();
        if (*ptr == ')') {
            ptr++;
            return v;
        } else {
            err = ERRINVALID;
            return 0;
        }
    }
    if (*ptr == '+' || *ptr == '-') {
        char sign = *ptr++;
        int v = readPrimary();
        return (sign == '-') ? -v : v;
    }
    return readNumber();
}

static int readTerm(void) {
    int v = readPrimary();
    while (err == ERRNONE) {
        skipSpace();
        if (*ptr == '*') {
            ptr++;
            v *= readPrimary();
        } else if (*ptr == '/') {
            ptr++;
            int n = readPrimary();
            if (n == 0) {
                err = ERRDIVZERO;
                return 0;
            }
            v /= n;
        } else break;
    }
    return v;
}

static int readExpr(void) {
    int v = readTerm();
    while (err == ERRNONE) {
        skipSpace();
        if (*ptr == '+') {
            ptr++;
            v += readTerm();
        } else if (*ptr == '-') {
            ptr++;
            v -= readTerm();
        } else break;
    }
    return v;
}

int main(void) {
    char buf[256];
    if (!fgets(buf, sizeof(buf), stdin)) return 0;
    buf[strcspn(buf, "\n")] = '\0';
    ptr = buf;
    err = ERRNONE;
    int v = readExpr();
    skipSpace();
    if (*ptr != '\0' || buf[0] == '\0') err = ERRINVALID;
    if (err == ERRDIVZERO) printf("Error: Division by zero.\n");
    else if (err == ERRINVALID) printf("Error: Invalid expression.\n");
    else printf("%d\n", v);
    return 0;
}
