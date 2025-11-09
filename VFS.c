#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLOCKSIZE 512
#define NUMBLOCKS 1024
#define MAXBLOCKPTRS 100

typedef enum { FILETYPE, DIRTYPE } NodeType;

struct FileNode;

typedef struct FreeBlock {
    int index;
    struct FreeBlock* next;
    struct FreeBlock* prev;
} FreeBlock;

typedef struct FileNode {
    char name[50];
    NodeType type;
    struct FileNode* parent;
    struct FileNode* child;
    struct FileNode* next;
    int contentSize;
    int blockPointers[MAXBLOCKPTRS];
    int blocksUsed;
} FileNode;

char disk[NUMBLOCKS][BLOCKSIZE];
FreeBlock* freeHead = NULL;
FreeBlock* freeTail = NULL;
FileNode* root = NULL;
FileNode* cwd = NULL;

void removeNewline(char* s) { if (s) s[strcspn(s, "\n")] = 0; }

void initNode(FileNode* n, const char* name, NodeType t) {
    strcpy(n->name, name);
    n->type = t;
    n->parent = NULL;
    n->child = NULL;
    n->next = NULL;
    n->contentSize = 0;
    n->blocksUsed = 0;
    for (int i = 0; i < MAXBLOCKPTRS; i++) n->blockPointers[i] = -1;
}

FileNode* findChild(FileNode* dir, const char* name) {
    if (!dir || dir->type != DIRTYPE || !dir->child) return NULL;
    FileNode* cur = dir->child;
    do {
        if (strcmp(cur->name, name) == 0) return cur;
        cur = cur->next;
    } while (cur != dir->child);
    return NULL;
}

void addChild(FileNode* dir, FileNode* child) {
    child->parent = dir;
    if (!dir->child) {
        dir->child = child;
        child->next = child;
    } else {
        FileNode* head = dir->child;
        FileNode* tail = head;
        while (tail->next != head) tail = tail->next;
        tail->next = child;
        child->next = head;
    }
}

void removeChild(FileNode* dir, FileNode* child) {
    if (!dir || !dir->child || !child) return;
    FileNode* head = dir->child;
    if (head == child && head->next == head) {
        dir->child = NULL;
        child->next = NULL;
        return;
    }
    FileNode* prev = head;
    while (prev->next != child && prev->next != head)
        prev = prev->next;
    if (prev->next == child) {
        prev->next = child->next;
        if (dir->child == child) dir->child = child->next;
        child->next = NULL;
    }
}

void pushFreeTail(int idx) {
    FreeBlock* n = malloc(sizeof(FreeBlock));
    if (!n) return;
    n->index = idx;
    n->next = NULL;
    n->prev = freeTail;
    if (!freeHead) freeHead = n;
    else freeTail->next = n;
    freeTail = n;
}

int popFreeHead() {
    if (!freeHead) return -1;
    FreeBlock* n = freeHead;
    int idx = n->index;
    freeHead = n->next;
    if (freeHead) freeHead->prev = NULL;
    else freeTail = NULL;
    free(n);
    return idx;
}

int countFreeBlocks() {
    int c = 0;
    for (FreeBlock* p = freeHead; p; p = p->next) c++;
    return c;
}

void freeFileBlocks(FileNode* f) {
    for (int i = 0; i < f->blocksUsed; i++) {
        if (f->blockPointers[i] >= 0) pushFreeTail(f->blockPointers[i]);
        f->blockPointers[i] = -1;
    }
    f->blocksUsed = 0;
    f->contentSize = 0;
}

int allocFileBlocks(FileNode* f, int size) {
    int need = (size + BLOCKSIZE - 1) / BLOCKSIZE;
    if (need > MAXBLOCKPTRS || need > countFreeBlocks()) return -1;
    for (int i = 0; i < need; i++) {
        int b = popFreeHead();
        if (b < 0) return -1;
        f->blockPointers[i] = b;
    }
    f->blocksUsed = need;
    f->contentSize = size;
    return 0;
}

void initVFS() {
    for (int i = 0; i < NUMBLOCKS; i++) pushFreeTail(i);
    root = malloc(sizeof(FileNode));
    if (!root) { printf("Error initializing root\n"); exit(1); }
    initNode(root, "/", DIRTYPE);
    cwd = root;
    printf("Compact VFS ready. Type 'exit' to quit.\n");
}

void makeDir(const char* name) {
    if (!name || !*name) return;
    if (findChild(cwd, name)) { printf("Directory already exists\n"); return; }
    FileNode* d = malloc(sizeof(FileNode));
    if (!d) return;
    initNode(d, name, DIRTYPE);
    addChild(cwd, d);
    printf("Directory '%s' created\n", name);
}

void list() {
    if (!cwd->child) { printf("(empty)\n"); return; }
    FileNode* cur = cwd->child;
    do {
        printf("%s%s\n", cur->name, cur->type == DIRTYPE ? "/" : "");
        cur = cur->next;
    } while (cur != cwd->child);
}

void changeDir(const char* name) {
    if (!name || !*name) return;
    if (strcmp(name, "/") == 0) { cwd = root; return; }
    if (strcmp(name, "..") == 0) { if (cwd->parent) cwd = cwd->parent; return; }
    FileNode* d = findChild(cwd, name);
    if (!d || d->type != DIRTYPE) { printf("No such directory\n"); return; }
    cwd = d;
    printf("Moved to /%s\n", cwd == root ? "" : cwd->name);
}

void printPath() {
    const char* parts[256]; int n = 0;
    FileNode* p = cwd;
    while (p && p != root && n < 256) { parts[n++] = p->name; p = p->parent; }
    printf("/");
    for (int i = n - 1; i >= 0; i--) {
        printf("%s", parts[i]);
        if (i) printf("/");
    }
    printf("\n");
}

void createFile(const char* name) {
    if (!name || !*name) return;
    if (findChild(cwd, name)) { printf("File already exists\n"); return; }
    FileNode* f = malloc(sizeof(FileNode));
    if (!f) return;
    initNode(f, name, FILETYPE);
    addChild(cwd, f);
    printf("File '%s' created\n", name);
}

void writeFile(const char* name, const char* line) {
    if (!name || !*name) return;
    FileNode* f = findChild(cwd, name);
    if (!f || f->type != FILETYPE) { printf("File not found\n"); return; }

    const char* text = strchr(line, '"');
    if (text) {
        text++;
        const char* end = strrchr(text, '"');
        if (end) {
            static char buf[4096];
            size_t len = end - text;
            if (len >= sizeof(buf)) len = sizeof(buf) - 1;
            memcpy(buf, text, len);
            buf[len] = 0;
            text = buf;
        }
    } else {
        const char* p = line;
        while (*p && *p != ' ') p++;
        while (*p == ' ') p++;
        while (*p && *p != ' ') p++;
        while (*p == ' ') p++;
        text = p;
    }

    int len = strlen(text);
    freeFileBlocks(f);
    if (len == 0) { printf("Written 0 bytes\n"); return; }
    if (allocFileBlocks(f, len) != 0) { printf("Disk full\n"); return; }

    int remain = len, off = 0;
    for (int i = 0; i < f->blocksUsed; i++) {
        int b = f->blockPointers[i];
        int chunk = remain > BLOCKSIZE ? BLOCKSIZE : remain;
        memset(disk[b], 0, BLOCKSIZE);
        memcpy(disk[b], text + off, chunk);
        off += chunk;
        remain -= chunk;
    }
    printf("Written %d bytes\n", f->contentSize);
}

void readFile(const char* name) {
    if (!name || !*name) return;
    FileNode* f = findChild(cwd, name);
    if (!f || f->type != FILETYPE) { printf("File not found\n"); return; }
    if (f->contentSize == 0) { printf("\n"); return; }
    char* buf = malloc(f->contentSize + 1);
    if (!buf) return;
    int remain = f->contentSize, off = 0;
    for (int i = 0; i < f->blocksUsed && remain > 0; i++) {
        int b = f->blockPointers[i];
        int chunk = remain > BLOCKSIZE ? BLOCKSIZE : remain;
        memcpy(buf + off, disk[b], chunk);
        off += chunk;
        remain -= chunk;
    }
    buf[f->contentSize] = 0;
    printf("%s\n", buf);
    free(buf);
}

void deleteFile(const char* name) {
    if (!name || !*name) return;
    FileNode* f = findChild(cwd, name);
    if (!f || f->type != FILETYPE) { printf("File not found\n"); return; }
    freeFileBlocks(f);
    removeChild(cwd, f);
    free(f);
    printf("File deleted\n");
}

void removeDir(const char* name) {
    if (!name || !*name) return;
    FileNode* d = findChild(cwd, name);
    if (!d || d->type != DIRTYPE) { printf("Directory not found\n"); return; }
    if (d->child) { printf("Directory not empty\n"); return; }
    removeChild(cwd, d);
    free(d);
    printf("Directory removed\n");
}

void diskInfo() {
    int freec = countFreeBlocks();
    int used = NUMBLOCKS - freec;
    double pct = (NUMBLOCKS ? (100.0 * used / NUMBLOCKS) : 0.0);
    printf("Total Blocks: %d\n", NUMBLOCKS);
    printf("Used Blocks: %d\n", used);
    printf("Free Blocks: %d\n", freec);
    printf("Disk Usage: %.2f%%\n", pct);
}

int main() {
    initVFS();
    char line[512], cmd[50], arg[50];
    while (1) {
        printf("/ > ");
        if (!fgets(line, sizeof(line), stdin)) break;
        removeNewline(line);
        cmd[0] = arg[0] = '\0';
        int n = sscanf(line, "%49s %49s", cmd, arg);
        if (n <= 0) continue;

        if (strcmp(cmd, "exit") == 0) {
            printf("Exiting...\n");
            while (freeHead) popFreeHead();
            break;
        } else if (strcmp(cmd, "mkdir") == 0) makeDir(arg);
        else if (strcmp(cmd, "ls") == 0) list();
        else if (strcmp(cmd, "cd") == 0) changeDir(arg);
        else if (strcmp(cmd, "pwd") == 0) printPath();
        else if (strcmp(cmd, "df") == 0) diskInfo();
        else if (strcmp(cmd, "create") == 0) createFile(arg);
        else if (strcmp(cmd, "write") == 0) writeFile(arg, line);
        else if (strcmp(cmd, "read") == 0) readFile(arg);
        else if (strcmp(cmd, "delete") == 0) deleteFile(arg);
        else if (strcmp(cmd, "rmdir") == 0) removeDir(arg);
        else printf("Unknown command\n");
    }
    return 0;
}

