#include "books.h"

#include <dialog.h>
#include <sqlite3.h>

#include <stdlib.h>
#include <string.h>

/* defined in main.c */
extern sqlite3 *db;

/* called by fill_library() */
static int sqlite_query_library(void *library, int argc, char **argv, char **col_name)
{
	enum {ID = 0, TITLE, AUTHOR, VOLUME, PAGES, PATH};
	struct library *lib = library;
	lib->count += 1;
	lib->books = reallocarray(lib->books, lib->count, sizeof(struct book));

	struct book *book = &lib->books[lib->count - 1];
	book->id = strdup(argv[ID]);
	book->title = strdup(argv[TITLE]);
	book->author = strdup(argv[AUTHOR]);
	book->volume = strdup(argv[VOLUME]);
	book->pages = strdup(argv[PAGES]);
	book->path = strdup(argv[PATH]);
	return 0;
}

/* fill a library with all the books from the database DB_TMP
 * be sure to free the manually allocated books with empty_library() */
int fill_library(struct library *lib)
{
	lib->count = 0;
	lib->books = calloc(0, sizeof(struct book));

	char *errmsg;
	int err = sqlite3_exec(db, "SELECT * FROM books ORDER BY title ASC",
			sqlite_query_library, lib, &errmsg);

	if (err == SQLITE_OK) return 0;
	fprintf(stderr, "%s\n", errmsg);
	dialog_msgbox("SQL Error", errmsg, 0, 0, 1);
	sqlite3_free(errmsg);
	return 1;
}

/* must be called after fill_library(), free all the books, dynamic
 * array library.books and sets library.count to 0 */
void empty_library(struct library *lib)
{
	while (lib->count--) {
		free(lib->books[lib->count].id);
		free(lib->books[lib->count].title);
		free(lib->books[lib->count].author);
		free(lib->books[lib->count].volume);
		free(lib->books[lib->count].pages);
		free(lib->books[lib->count].path);
	};
	free(lib->books);
}

