#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>

char currentDir[256];

void cd();
void redirectOut();
void pwd();
void clear();
void ls();
void listFiles();
void resolveCommand(char* command);
void listFilesRecursive(const char *basePath);
void doFile(char *basePath, void (*funcFileName)(), void (*funcPath)());
void doFileRecursively(char *basePath, void (*funcFileName)(), void (*funcPath)());
void printFile(char* name);
void listDir();
void listDirRS();
void removeFileAndDir(const char *fpath);
void removeOnlyFiles(const char *fpath);
void removeDirRS();
void removeDir();
void makeDir();
void getCurrentDir();
void setCurrentDir();
void updateCurrentDir();
void redirectIn();
void redirectOut();

int main() {
   char command[256];
   char param[256];
   while(1) {
      updateCurrentDir();
      printf("%s $", currentDir);
      scanf("%s", command);
      resolveCommand(command);
   }
   return 0;
}

void doFile(char *basePath, void (*funcFileName)(), void (*funcPath)())
{
    char path[1000];
    struct dirent *dp;
    DIR *dir = opendir(basePath);

    if (!dir)
        return;

    while ((dp = readdir(dir)) != NULL)
    {
        if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
        {
            strcpy(path, basePath);
            strcat(path, "/");
            strcat(path, dp->d_name);
            if (funcFileName) {
               (*funcFileName)(dp->d_name);
            }
            if (funcPath) {
               (*funcPath)(path);
            }
        }
    }

    closedir(dir);
}

void doFileRecursively(char *basePath, void (*funcFileName)(), void (*funcPath)()) {
   char path[1000];
   struct dirent *dp;
   DIR *dir = opendir(basePath);

   if (!dir)
      return;

   while ((dp = readdir(dir)) != NULL)
   {
      if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
      {
         strcpy(path, basePath);
         strcat(path, "/");
         strcat(path, dp->d_name);

         doFileRecursively(path, &*funcFileName, &*funcPath);

         if (funcFileName) {
            (*funcFileName)(dp->d_name);
         }
         if (funcPath) {
            (*funcPath)(path);
         }
      }
   }

   closedir(dir);
}

void printFile(char* name) {
   printf("%s\n", name, NULL);
}

void printFileInfo(char* filepath) {

    int returnval = 0;
    returnval = access (filepath, R_OK);

    if (returnval == 0)
        printf ("r");
    else if (errno == EACCES)
        printf ("-");

    returnval = 0;
    returnval = access (filepath, W_OK);
    if (returnval == 0)
        printf ("w");
    else if (errno == EACCES)
        printf ("-");

    returnval = 0;
    returnval = access (filepath, X_OK);
    if (returnval == 0)
        printf ("x ");
    else if (errno == EACCES)
        printf ("- ");
}

void ls() {
    char str[256] = "";
    scanf("%s", str);

    if(!strcmp(str, "-R"))
        listDirRS(str);
    else if(!strcmp(str, "-l"))
        listDir(str);
    else if(!strcmp(str, "-lR"))
        listDirRS(str);
    else
        listDir(str);
}

void listDir(char str[256]) {
    if(!strcmp(str, "-l")) {
        char dir[256] = "";
        scanf("%s", dir);
        doFile(dir, &printFile, &printFileInfo);
    } else
        doFile(str, &printFile, NULL);
}

void listDirRS(char str[256]) {
    char dir[256] = "";
    scanf("%s", dir);

    if(!strcmp(str, "-lR")) {
        doFileRecursively(dir, &printFile, &printFileInfo);
    } else if(!strcmp(str, "-R")) {
        doFileRecursively(dir, &printFile, NULL);
    }
}

void removeFileAndDir(const char *fpath) {
   struct stat statbuf;
   if (!stat(fpath, &statbuf)) {
      if (S_ISDIR(statbuf.st_mode)) {
         if (rmdir(fpath) < 0) {
            printf("Error - Ao excluir diretorio %s\n", fpath);
         }
      } else if (remove(fpath) < 0) {
         printf("Error - Ao excluir arquivo %s\n", fpath);
      }
   } else {
      printf("Error - Na exclusao %s\n", fpath);
   }
}

void removeOnlyFiles(const char *fpath) {
   struct stat statbuf;
   if (!stat(fpath, &statbuf)) {
      if (!S_ISDIR(statbuf.st_mode)) {
         if (remove(fpath) < 0) {
            printf("Error - Ao excluir arquivo %s\n", fpath);
         }
      }
   } else {
      printf("Error - Na exclusao %s\n", fpath);
   }
}

void removeDirRS() {
   char dir[256] = "";
   scanf("%s", dir);
   if(!strcmp(dir, ".")) {
      doFileRecursively(currentDir, NULL, &removeFileAndDir);
   } else {
      doFileRecursively(dir, NULL, &removeFileAndDir);
   }
   removeFileAndDir(dir);
}

void removeDir() {
   char dir[256] = "";
   scanf("%s", dir);
   doFile(dir, NULL, &removeOnlyFiles);
}

void makeDir() {
   char dir[256] = "";
   scanf("%s", dir);
   if (mkdir(dir, 0777) < 0) {
      printf("Error - Ao criar pasta\n");
   }
}

void getCurrentDir() {
   if (getcwd(currentDir, sizeof currentDir) < 0) {
      printf("Error - Ao pegar o diretorio corrente\n");
   }
}

void setCurrentDir() {
   if (chdir(currentDir) < 0) {
      printf("Error - Ao setar a pasta corrente\n");
   }
}

void updateCurrentDir() {
   getCurrentDir();
   setCurrentDir();
}

void cd() {
   char dir[256] = "", newDir[256] = "";
   scanf("%s", dir);
   strcat(newDir, currentDir);
   strcat(newDir, "\/");
   strcat(newDir, dir);
   if (chdir(newDir) < 0) {
      printf("Error - Mudar diretorio\n");
   }
}

void pwd() {
   printf("%s\n", currentDir);
}

void clear() {
   system("clear");
}

void redirectIn() {
   char dir[256] = "";
   scanf("%s", dir);
   int input_fds = open(dir, O_RDONLY); // ex: "./input.txt"
   if(dup2(input_fds, STDIN_FILENO) < 0) {
      printf("Unable to duplicate file descriptor.");
   }
}

void redirectOut() {
   char dir[256] = "";
   scanf("%s", dir);
   int input_fds = open(dir, O_WRONLY);
   if(dup2(input_fds, STDOUT_FILENO) < 0) {
      printf("Unable to duplicate file descriptor.");
   }
}

void resolveCommand(char* command) {


   if(!strcmp(command, "..")) {
      cd();
      return;
   }

   if(!strcmp(command, "cd")) {
      cd();
      return;
   }

   if(!strcmp(command, "pwd")) {
      pwd();
      return;
   }

   if(!strcmp(command, "rm")) {
      removeDir();
      return;
   }

   if(!strcmp(command, "rmr")) {
      removeDirRS();
      return;
   }

   if(!strcmp(command, "mkdir")) {
      makeDir();
      return;
   }

   if(!strcmp(command, "c") || !strcmp(command, "cls") || !strcmp(command, "clear")) {
      clear();
      return;
   }

   if(!strcmp(command, "<")) {
      redirectIn();
      return;
   }

   if(!strcmp(command, ">")) {
      redirectOut();
      return;
   }

   if(!strcmp(command, "ls")) {
      ls();
      return;
   }

   printf("%s command not found!\n", command);
}

