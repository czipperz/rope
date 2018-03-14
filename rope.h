#include <stddef.h>

typedef struct Rope Rope;

void Rope_Free(Rope*);
Rope* Rope_New(void);

Rope* Rope_From(const char* data, size_t len);
Rope* Rope_FromStr(const char* str);

int Rope_Insert(Rope* rope, size_t pos, const char* data, size_t len);
int Rope_InsertStr(Rope* rope, size_t pos, const char* data);
