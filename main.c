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

char quit = 0;
sqlite3 *db;

void open_read()
{

}

int open_add()
{
	dlg_clear();
	*dialog_vars.input_result = '\0';
	dialog_vars.default_button = -1;
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

	char title[TITLE_MAX] = {0};
	char author[AUTHOR_MAX] = {0};
	char volume[VOLUME_MAX] = {0};
	char pages[PAGES_MAX] = {0};

	int offset = 0;
	for (int i = 0; i < 4; i++) {
		char *end = strchr(dialog_vars.input_result + offset, '\n');
		int length = end - (dialog_vars.input_result + offset);

		char *data;
		int max = 0;
		switch (i) {
		case 0: data = title; max = TITLE_MAX; break;
		case 1: data = author; max = AUTHOR_MAX; break;
		case 2: data = volume; max = VOLUME_MAX; break;
		case 3: data = pages; max = PAGES_MAX; break;
		}
		
		strncpy(data,
			dialog_vars.input_result + offset,
			(length < max) ? length : max);
		offset += length + 1;
	}

	/* Append '/' at the end of $HOME */
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
		title, author, volume, pages,
		dialog_vars.input_result);
	char no = dialog_yesno("Verification", yesno_msg, 30, 60);
	if (no) return 1;

	/* add book to database  */
	char *sql_query = yesno_msg;
	sprintf(sql_query, "INSERT INTO books (title, author, volume, pages, path) VALUES(\"%s\", \"%s\", \"%s\", \"%s\", \"%s\");", title, author, volume, pages, dialog_vars.input_result);
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

void open_delete()
{

}

void setup_db()
{
	fclose(fopen(DB_TMP, "w"));
	
	int err = sqlite3_open(DB_TMP, &db);
	if (err) {
		EXIT(EXIT_FAILURE);
	}
	
	char *message;
        err = sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS books  \
(id INTEGER PRIMARY KEY, title TEXT, author TEXT, volume INTEGER, \
pages INTEGER, path TEXT);", NULL, NULL, &message);
}

char open_quit()
{
	char defaultno = dialog_vars.defaultno;
	dialog_vars.defaultno = 1;
	char answer = !dialog_yesno("Quitting",
			"Are you sure you want to quit?", 6, 30);
	dialog_vars.defaultno = defaultno;
	return answer;
}

int main(int argc, char **argv)
{
	if (argc != 1) {
		fprintf(stderr, "Usage: %s\n", argv[0]);
	}

	setup_db();

	dialog_vars.erase_on_exit = 1;
	dialog_vars.nocancel = 1;
	dialog_vars.input_length = 0;
        init_dialog(stdin, stdout);

	do {
		dlg_clear();
		if (dialog_vars.input_result) {
			*dialog_vars.input_result = 0;
		}
		dialog_menu("Main Menu", "Welcome to BookApp!", 7, 50, 0, 4,
			    (char*[]){
				    "READ", "Read a book from your library",
				    "ADD", "Add a book to your library",
				    "DELETE", "Remove a book to your library",
				    "QUIT", "Close the application"
			    });
	
		switch (*dialog_vars.input_result) {
		case 'R': open_read(); break;
		case 'A': while(open_add()); break;
		case 'D': open_delete(); break;
		case 'Q': quit = open_quit();
		}
	} while (!quit);

	EXIT(EXIT_SUCCESS);
}
