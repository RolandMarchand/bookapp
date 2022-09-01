#pragma once

#include <stdlib.h>

struct book {
	char *id;
	char *title;
	char *author;
	char *volume;
	char *pages;
	char *path;
};

/* dynamic array of books
 * use fill_library() and empty_library() */
struct library {
	struct book *books;
        size_t count;
};

/* fill a library with all the books from the database DB_TMP
 * be sure to free the manually allocated books with empty_library() */
int fill_library(struct library *lib);
/* must be called after fill_library(), free all the books, dynamic
 * array library.books and sets library.count to 0 */
void empty_library(struct library *lib);

