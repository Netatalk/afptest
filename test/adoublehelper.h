/* ---------------------------------------
*/
extern int delete_unix_adouble(char *path, char *name);
extern int delete_unix_dir(char *path, char *name);
extern int folder_with_ro_adouble(u_int16_t vol, int did, char *name, char *file);
extern int delete_ro_adouble(u_int16_t vol, int did, char *file);

extern int delete_unix_md(char *path, char *name, char *file);
extern int delete_unix_rf(char *path, char *name, char *file);
extern int delete_unix_file(char *path, char *name, char *file);

extern int unlink_unix_file(char *path, char *name, char *file);
extern int symlink_unix_file(char *target, char *path, char *source);

/* -------------------
*/
