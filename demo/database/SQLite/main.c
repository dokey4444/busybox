/*
 * =====================================================================================
 *
 *       Filename:  main.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/26/2016 06:49:04 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Elwin.Gao (elwin), elwin.gao4444@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#include <stdlib.h>
#include <stdio.h>

#include "sqlite3.h"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  callback
 *  Description:  
 * =====================================================================================
 */
static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
	int i;
	for(i=0; i<argc; i++){
		printf("%s = %s\t", azColName[i], argv[i] ? argv[i] : "NULL");
	}
	printf("\n");
	return 0;
}		/* -----  end of static function callback  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main(int argc, char *argv[])
{
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;

	if (argc!=3) {
		fprintf(stderr, "Usage: %s DATABASE SQL-STATEMENT\n", argv[0]);
		return EXIT_FAILURE;
	}
	rc = sqlite3_open(argv[1], &db);
	if (rc) {
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		sqlite3_close(db);
		return EXIT_FAILURE;
	}
	rc = sqlite3_exec(db, argv[2], callback, 0, &zErrMsg);
	if(rc!=SQLITE_OK){
		fprintf(stderr, "SQL error: %s\n", zErrMsg);
		sqlite3_free(zErrMsg);
	}
	sqlite3_close(db);
	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

