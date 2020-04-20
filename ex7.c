/* 
 *
 * Copyright (C) Armand Mousavi
 * Email: amousavi@uw.edu
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <inttypes.h>

#define SIZE 128
/*
 * Read the text within the file to stdout
 */
void printFile(char* filePath) {
  if (filePath == NULL) {
    fprintf(stderr, "file path is NULL\n");
    exit(EXIT_FAILURE);
  }
  /* 1. Open the file, use O_RDONLY flag */
  int32_t fileDes = open(filePath, O_RDONLY);
  if (fileDes == -1) {
    fprintf(stderr, "file failed to open\n");
    free(filePath);
    exit(EXIT_FAILURE);
  }
  /* 2. Read from the file and write it to standard out.*/
  /* Section 3 solutions "cat"*/
  char buf[SIZE];
  ssize_t len;
  do {
    len = read(fileDes, buf, SIZE);
    if (len == -1) {
      if (errno != EINTR) {
        close(fileDes);
        free(filePath);
        perror(NULL);
        exit(EXIT_FAILURE);
      }
    continue;
    }
    ssize_t total = 0;
    ssize_t wlen;
    while (total < len) {
      wlen = write(1, buf + total, len - total);
      if (wlen == -1) {
        if (errno != EINTR) {
          close(fileDes);
          free(filePath);
          perror(NULL);
          exit(EXIT_FAILURE);
        }
        continue;
      }
      total += wlen;
    }
  } while (len != 0);
  /* 3. Clean up */
  close(fileDes);
}
/*
 * Get the full path to the file as a concatenated string
 */
void getFilePath(char* fileDirPath, char* fileName, char* arg1) {
  if (fileDirPath == NULL || fileName == NULL || arg1 == NULL) {
    fprintf(stderr, "file path or name is NULL\n");
    exit(EXIT_FAILURE);
  }
  // length of the path to file
  int32_t fileDirPathLen = strlen(fileDirPath);
  // make sure the file directory path ends with trailing slash
  int32_t maxlen = fileDirPathLen + strlen(fileName);
  if (fileDirPath[fileDirPathLen - 1] == '/') {
    snprintf(fileDirPath, maxlen + 1, "%s%s", fileDirPath, fileName);
    return;
  }
  snprintf(fileDirPath, maxlen + 2, "%s%s%s", arg1, "/", fileName);
}
/*
 * Go through each file in the directory passed as a parameter,
 * if the file is a .txt file get its full path and print it
 */
int main(int argc, char** argv) {
/* 1. Check to make sure we have a valid command line arguments */
  if (argc != 2) {
    fprintf(stderr, "Usage: ./ex7 <path>\n");
    return EXIT_FAILURE;
  }
  /* 2. Open the directory, look at opendir() */
  DIR* dirp = opendir(argv[1]);
  if (dirp == NULL) {
    fprintf(stderr, "Could not open directory\n");
    return EXIT_FAILURE;
  }

  /* 3. Read through/parse the directory and choose .txt files*/
  /* Look at readdir() and struct dirent */
  struct dirent *entry;
  entry = readdir(dirp);
  char* fileType = ".txt";
  size_t argLength = strlen(argv[1]);
  while (entry != NULL) {
    char* name = entry->d_name;
    int32_t length = strlen(entry->d_name);
    // if the file name could hold .txt and then if it ends with .txt
    if (length >= 4 && strcmp(fileType, (name + length) - 4) == 0) {
      // append the filename to the directory path
      char* filePath = malloc((argLength + length + 1) * sizeof(char));
      snprintf(filePath, argLength + 2, "%s", argv[1]);
      getFilePath(filePath, name, argv[1]);
      printFile(filePath);
      free(filePath);
    }
    entry = readdir(dirp);
  }
  closedir(dirp);
  return EXIT_SUCCESS;
}
