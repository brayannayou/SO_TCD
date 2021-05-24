#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char currentDir[256];

void removeDir() {
   char dir[256] = "";
   scanf("%s", dir);
   if (rmdir(dir) < 0) {
      printf("Error - Ao excluir diretorio\n");
   }
}

void makeDir() {
   char dir[256] = "";
   scanf("%s", dir);
   if (mkdir(dir) < 0) {
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
   strcat(newDir, "\\");
   strcat(newDir, dir);
   if (chdir(newDir) < 0) {
      printf("Error - changing the directory\n");
   }
}

void redirectOut() {
   printf("redirecionar saida\n");
}

void pwd() {
   printf("Path\n----\n%s\n\n", currentDir);
}

void clear() {
   system("clear");
   system("cls");
}

void resolveCommand(char* command) {
   if(!strcmp(command, "cd")) {
      cd();
      return;
   }
   if(!strcmp(command, "pwd")) {
      pwd();
      return;
   }
   if(!strcmp(command, "rmdir")) {
      removeDir();
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
   if(!strcmp(command, ">")) {
      redirectOut();
      return;
   }
}

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
