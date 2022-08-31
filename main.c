#include <dialog.h>
#include <sqlite3.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define TITLE_MAX 256
#define AUTHOR_MAX 128
#define VOLUME_MAX 8
#define PAGES_MAX 8
#define DB_TMP "/tmp/db.tmp"
#define EXIT(exit_status)					\
	do {							\
		fprintf(stderr, "%s\n", sqlite3_errmsg(db));	\
		dialog_msgbox("SQL Error",			\
				sqlite3_errmsg(db), 30, 60, 1);	\
		sqlite3_close(db);				\
		end_dialog();					\
		exit(exit_status);				\
	} while (0)

struct book {
	char *id;
	char *title;
	char *author;
	char *volume;
	char *pages;
	char *path;
};

struct library {
	struct book *books;
        size_t count;
};

sqlite3 *db;

/* alias for dynamic pointer dialog_vars.input_result */
char *get_input(void) { return dialog_vars.input_result; }

void open_read()
{

}

int open_add(void)
{
	dlg_clear();
	dialog_vars.default_button = -1;
get_form:
	*get_input() = '\0';
        dialog_form("New Book", "Enter the details of the new book\
, so that it can be added to your library.\n\n\
Use the arrow keys to change field.",
		    0, 0, 0, 4,
		    (char*[]){
			    "Title:", "1", "1",
			    "", "1", "9",
			    "30", "64",

			    "Author:", "2", "1",
			    "", "2", "9",
			    "30", "64",

			    "Volume:", "3", "1",
			    "", "3", "9",
			    "5", "16",

			    "Pages:", "4", "1",
			    "", "4", "9",
			    "5", "16"
		    });
	if (*get_input() == '\n' || strstr(get_input(), "\n\n")) {
		dialog_msgbox("Error", "Please fill in all the fields.", 6, 20, 1);
		goto get_form;
	}

	/* write input to `book` */
	struct book book;
	char **data[4] = {&book.title, &book.author, &book.volume, &book.pages};
	char i = 0;
	for (char *str = strtok(get_input(), "\n"); str; str = strtok(NULL, "\n")) {
		*data[i] = calloc(strlen(str) + 1, sizeof(char));
		strcpy(*data[i], str);
		i++;
	}

	/* append '/' at the end of $HOME */
	const char *home = getenv("HOME");
	char fhome[strlen(home) + 1];
	strcpy(fhome, home);
	fhome[strlen(home)] = '/';

	dialog_vars.default_button = -3;
	dialog_fselect("Find Book", fhome, 15, 60);

	/* TODO, check for duplicate and ask user if it's okay */
	/* ask user to verify imput */
	char yesno_msg[5120] = {0};
	sprintf(yesno_msg, "Does this look okay?\n\nTitle: %s\nAuthor: %s\n\
Volume: %s\nPages: %s\nPath: %s",
		book.title, book.author, book.volume, book.pages,
		get_input());
	char no = dialog_yesno("Verification", yesno_msg, 30, 60);
	if (no) return 1;

	/* add book to database  */
	char *sql_query = yesno_msg;
	sprintf(sql_query, "INSERT INTO books (title, author, volume, pages, path) VALUES(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\");", book.title, book.author, book.volume, book.pages, get_input());
	char *errmsg;
	int err = sqlite3_exec(db, sql_query, NULL, NULL, &errmsg);
	if (err != SQLITE_OK) {
		dialog_msgbox("SQL Error", errmsg, 6, 30, 1);
		sqlite3_free(errmsg);
		return 1;
	}

	dialog_msgbox("Success!", "Your book has been added to the library.",
		      6, 30, 1);
	return 0;
}

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

int fill_library(struct library *lib)
{
	lib->count = 0;
	lib->books = calloc(0, sizeof(struct book));

	char *errmsg;
	int err = sqlite3_exec(db, "SELECT id, title, author, volume, pages, \
 path FROM books ORDER BY title ASC", sqlite_query_library, lib, &errmsg);

	if (err == SQLITE_OK) return 0;
	fprintf(stderr, "%s\n", errmsg);
	dialog_msgbox("SQL Error", errmsg, 30, 60, 1);
	sqlite3_free(errmsg);
	return 1;
}

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

void open_delete(void)
{
	*get_input() = '\0';
	struct library lib;
	fill_library(&lib);

	/* format the items for dialog_checklist */
	char *items[lib.count * 3];
	for (int i = 0; i < lib.count; i++) {
		/* the item's formatting is 'Title, Author' */
		int size = strlen(lib.books[i].title)
			+ strlen(lib.books[i].author)
			+ 3;  /* ', ' and '\0' */
		char (*title)[size] = malloc(sizeof(char[size]));
		sprintf((char*)title, "%s, %s",
			lib.books[i].title, lib.books[i].author);

		items[i * 3] = lib.books[i].id;
		items[i * 3 + 1] = (char*)title;
		items[i * 3 + 2] = "";
	}


	dialog_checklist("Delete", "Selects the books you want gone from your library.",
			30, 60, 0, lib.count, items, FLAG_CHECK);
	for (int i = 1; i < lib.count; i += 3) {
		free(items[i]);
	}
	empty_library(&lib);
}

void setup_db(void)
{
	fclose(fopen(DB_TMP, "a"));
	
	int err = sqlite3_open(DB_TMP, &db);
	if (err) {
		EXIT(EXIT_FAILURE);
	}
	
	char *message;
        err = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS books  \
(id INTEGER PRIMARY KEY, title TEXT, author TEXT, volume INTEGER, \
pages INTEGER, path TEXT);", NULL, NULL, &message);
}

char open_quit(void)
{
	char defaultno = dialog_vars.defaultno;
	dialog_vars.defaultno = 1;
	char answer = !dialog_yesno("Quitting",
			"Are you sure you want to quit?", 6, 30);
	dialog_vars.defaultno = defaultno;
	return answer;
}

int main(void)
{
	setup_db();

        init_dialog(stdin, stdout);
	dialog_vars.erase_on_exit = 1;
	dialog_vars.nocancel = 1;
	dialog_vars.input_length = 0;

	char quit = 0;
	do {
		dlg_clear();
		if (get_input()) {
			*get_input() = '\0';
		}
		dialog_menu("Main Menu", "Welcome to BookApp!", 7, 50, 0, 4,
			    (char*[]){
				    "READ", "Read a book from your library",
				    "ADD", "Add a book to your library",
				    "DELETE", "Remove a book to your library",
				    "QUIT", "Close the application"
			    });
		switch (*get_input()) {
		case 'R': open_read(); break;
		case 'A': while(open_add()); break;
		case 'D': open_delete(); break;
		case 'Q': quit = open_quit();
		}
	} while (!quit);

	sqlite3_close(db);
	end_dialog();
	exit(EXIT_SUCCESS);
}
