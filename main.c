#include "books.h"

#include <dialog.h>
#include <sqlite3.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define RESET_INPUT() do { *get_input() = '\0'; } while (0)
#define INPUT_EMPTY() (!*get_input())
#define DLG_X 60
#define DLG_Y 30
#define DB_TMP "/tmp/db.tmp"
#define EXIT(exit_status)					\
	do {							\
		fprintf(stderr, "%s\n", sqlite3_errmsg(db));	\
		dialog_msgbox("SQL Error",			\
			sqlite3_errmsg(db), 0, 0, 1);		\
		sqlite3_close(db);				\
		end_dialog();					\
		exit(exit_status);				\
	} while (0)

sqlite3 *db;

/* alias for dynamic pointer dialog_vars.input_result */
char *get_input(void) { return dialog_vars.input_result; }

void open_read(void)
{
	RESET_INPUT();
	struct library lib;
	fill_library(&lib);

	if (!lib.count) {
		dialog_msgbox("Warning", "You have no books to read", 0, 0, 1);
		return;
	}

	/* generate seventh argument of dialog_menu */
	char *items[lib.count * 3];
	for (int i = 0; i < lib.count; i++) {
		/* each book is displayed as 'Title, Author vVolume' */
		int size = strlen(lib.books[i].title)
			+ strlen(lib.books[i].author)
			+ strlen(lib.books[i].volume)
			+ 5;  /* ', ' ' v' and '\0' */
		char (*descr)[size] = malloc(sizeof(char[size]));
		sprintf((char*)descr, "%s, %s v%s",
			lib.books[i].title, lib.books[i].author,
			lib.books[i].volume);

		items[i * 3] = lib.books[i].id;
		items[i * 3 + 1] = (char*)descr;
		items[i * 3 + 2] = "";
	}

	dialog_menu("Read", "Choose a book you want to read.",
		    DLG_Y, DLG_X, 0, lib.count, items);

	/* free item description */
	for (int i = 1; i < lib.count; i += 3) {
		free(items[i]);
        }

	char *title, *path;
	for (int i = 0; i < lib.count; i++) {
		if (strcmp(lib.books[i].id, get_input())) continue;
		title = lib.books[i].title;
		path = lib.books[i].path;
	}
	dlg_clear();
	dialog_textbox(title, path, 0, 0);
	empty_library(&lib);
}

int open_add(void)
{
	dlg_clear();
	dialog_vars.default_button = -1;
get_form:
	RESET_INPUT();
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
		dialog_msgbox("Error", "Please fill in all the fields.",
			0, 0, 1);
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
	dialog_fselect("Find Book", fhome, DLG_Y, DLG_X * 2);

	/* ask user to verify imput */
	char yesno_msg[5120] = {0};
	sprintf(yesno_msg, "Does this look okay?\n\nTitle: %s\nAuthor: %s\n\
Volume: %d\nPages: %d\nPath: %s",
		book.title, book.author, atoi(book.volume), atoi(book.pages),
		get_input());
	dlg_clear();
	char no = dialog_yesno("Verification", yesno_msg, DLG_Y, DLG_X);
	if (no) return 1; /* loop */

	/* format SQL query */
	char *sql_query = yesno_msg;
        sprintf(sql_query,
		"INSERT INTO books (title, author, volume, pages, path) \
VALUES(\"%s\", \"%s\", \"%d\", \"%d\", \"%s\");",
		book.title, book.author, atoi(book.volume), atoi(book.pages), get_input());

	/* run SQL query */
	char *errmsg;
	int err = sqlite3_exec(db, sql_query, NULL, NULL, &errmsg);
	if (err != SQLITE_OK) {
		dialog_msgbox("SQL Error", errmsg, 0, 0, 1);
		sqlite3_free(errmsg);
		return 1;
	}

	free(book.title);
	free(book.author);
	free(book.volume);
	free(book.pages);
	dialog_msgbox("Success!", "Your book has been added to the library.",
		0, 0, 1);
	return 0;
}

void open_delete(void)
{
	RESET_INPUT();
	struct library lib;
	fill_library(&lib);

	if (!lib.count) {
		dialog_msgbox("Warning", "You have no books to delete.",
			0, 0, 1);
		return;
	}

	/* format the items for dialog_checklist */
	char *items[lib.count * 3];
	for (int i = 0; i < lib.count; i++) {
		/* the item's formatting is 'Title, Author vVolume' */
		int size = strlen(lib.books[i].title)
			+ strlen(lib.books[i].author)
			+ strlen(lib.books[i].volume)
			+ 5;  /* ', ' ' v' and '\0' */
		char (*descr)[size] = malloc(sizeof(char[size]));
		sprintf((char*)descr, "%s, %s v%s",
			lib.books[i].title, lib.books[i].author,
			lib.books[i].volume);

		items[i * 3] = lib.books[i].id;
		items[i * 3 + 1] = (char*)descr;
		items[i * 3 + 2] = "";
	}

	dialog_checklist("Delete",
			"Selects the books you want gone from your library.",
			 DLG_Y, DLG_X, 0, lib.count, items, FLAG_CHECK);
	/* free item description */
	for (int i = 1; i < lib.count; i += 3) {
		free(items[i]);
	}
	empty_library(&lib);

	if (INPUT_EMPTY()) return;

	/* format SQL query */
	int size = 25; /* length of DELETE query */
	char *query = calloc(size, sizeof(char));
	strcpy(query, "DELETE FROM books WHERE ");
	for (char *str = strtok(get_input(), " "); str; str = strtok(NULL, " ")) {
		size += strlen("ID IS ") + strlen(str) + strlen(" OR ");
		query = reallocarray(query, size, sizeof(char));
		if (!query) {
			perror("SQL query formatting failed\n");
			exit(EXIT_FAILURE);
		}
		sprintf(query, "%s%s%s%s", query, "ID IS ", str, " OR ");
	}
	query[size - 5] = '\0';

	/* SQL DELETE query */
	char *errmsg;
	int err = sqlite3_exec(db, query, NULL, NULL, &errmsg);
	if (err != SQLITE_OK) {
		dialog_msgbox("SQL Error", errmsg, 0, 0, 1);
		sqlite3_free(errmsg);
	}
	free(query);
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
			"Are you sure you want to quit?",
			0, 0);
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
			RESET_INPUT();
		}
		dialog_menu("Main Menu", "Welcome to BookApp!",
			    7, 0, 0, 4,
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
