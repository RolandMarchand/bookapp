#include <stdio.h>
#include <sqlite3.h>

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}

int main(int argc, char **argv){
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

	if( argc!=3 ){
		fprintf(stderr, "Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
		return(1);
	}
	rc = sqlite3_open(argv[1], &db);
	if( rc ){
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return(1);
	}
	rc = sqlite3_exec(db, argv[2], callback, 0, &zErrMsg);
	if( rc!=SQLITE_OK ){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
	return 0;
}
/* #include <dialog.h> */
/* #include <sqlite3.h> */

/* #include <stdlib.h> */
/* #include <string.h> */
/* #include <unistd.h> */

/* char quit = 0; */

/* void open_read() */
/* { */

/* } */

/* void open_add() */
/* { */
/* 	dlg_clear(); */
/* 	dialog_vars.default_button = -1; */
/* 	dialog_form("New Book", */
/* 		    "Enter the details of the new book\ */
/* , so that it can be added to your library.\n\nUse the arrow keys to change field.", */
/* 		    0, 0, 0, 4, */
/* 		    (char*[]){ */
/* 			    "Name:", "1", "1", */
/* 			    "", "1", "9", */
/* 			    "30", "64", */

/* 			    "Author:", "2", "1", */
/* 			    "", "2", "9", */
/* 			    "30", "64", */

/* 			    "Volume:", "3", "1", */
/* 			    "", "3", "9", */
/* 			    "5", "16", */

/* 			    "Pages:", "4", "1", */
/* 			    "", "4", "9", */
/* 			    "5", "16" */
/* 		    }); */
	
/* 	/\* Append '/' at the end of $HOME *\/ */
/* 	const char *home = getenv("HOME"); */
/* 	char fhome[strlen(home) + 1]; */
/* 	strcpy(fhome, home); */
/* 	fhome[strlen(home)] = '/'; */

/* 	dialog_vars.default_button = -3; */
/* 	dialog_fselect("Find Book", fhome, 15, 60); */
/* 	dialog_msgbox("Success!", "Your book has been added to the library.", 6, 30, 1); */
/* } */

/* void open_delete() */
/* { */

/* } */

/* int main(void) */
/* { */
/* 	sqlite3 *db; */
/* 	dialog_vars.erase_on_exit = 1; */
/* 	dialog_vars.nocancel = 1; */
/*         init_dialog(stdin, stdout); */

/* 	do { */
/* 		dlg_clr_result(); */
/* 		dlg_clear(); */
/* 		dialog_menu("Main Menu", "Welcome to BookApp!", 7, 50, 0, 4, */
/* 			    (char*[]){ */
/* 				    "READ", "Read a book from your library", */
/* 				    "ADD", "Add a book to your library", */
/* 				    "DELETE", "Remove a book to your library", */
/* 				    "QUIT", "Close the application" */
/* 			    }); */
	
/* 		switch (*dialog_vars.input_result) { */
/* 		case 'R': open_read(); break; */
/* 		case 'A': open_add(); break; */
/* 		case 'D': open_delete(); break; */
/* 		case 'Q': */
/* 		default: */
/* 			quit = 1; */
/* 		} */
/* 	} while (!quit); */

/* 	end_dialog(); */
/* } */
