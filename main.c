// To compile: gcc -o main main.c
// To run: ./main input_example.txt output.txt

#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

typedef struct node {
  char key[50];
  char type[3];
  uint32_t counter;
  uint32_t size;
  mode_t permission;
  struct node *left;
  struct node *right;
} node;

node *search(node *root, const char name[50]) {
  if (root == NULL || strcmp(root->key, name) == 0) {
    return root;
  }

  if (strcmp(root->key, name) < 0) {
    return search(root->right, name);
  } else {
    return search(root->left, name);
  }
}

static uint32_t insertion_counter = 1;

node *insert(node *root, const char name[50], const char type[3], uint32_t size,
             mode_t permission, uint32_t count) {
  if (root == NULL) {
    node *new_node = (node *)malloc(sizeof(node));
    if (new_node == NULL) {
      return root;
    }

    strcpy(new_node->key, name);
    strcpy(new_node->type, type);
    new_node->size = size;
    new_node->permission = permission;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->counter = insertion_counter++;
    return new_node;
  }

  if (strcmp(name, root->key) < 0) {
    root->left = insert(root->left, name, type, size, permission, count);
  } else if (strcmp(name, root->key) > 0) {
    root->right = insert(root->right, name, type, size, permission, count);
  }
  if (strcmp(name, root->key) == 0) {

    strcpy(root->type, type);
    root->size = size;
    root->permission = permission;
  }

  return root;
}

void in_order(node *root, FILE *output) {
  if (root != NULL) {
    in_order(root->left, output);
    if (root->size > 1) {
      fprintf(output, "%u %s %s %u bytes\n", root->counter, root->key,
              root->type, root->size);
    } else {
      fprintf(output, "%u %s %s %u byte\n", root->counter, root->key,
              root->type, root->size);
    }
    in_order(root->right, output);
  }
}

void pre_order(node *root, FILE *output) {
  if (root != NULL) {
    if (root->size > 1) {
      fprintf(output, "%u %s %s %u bytes\n", root->counter, root->key,
              root->type, root->size);
    } else {
      fprintf(output, "%u %s %s %u byte\n", root->counter, root->key,
              root->type, root->size);
    }
    pre_order(root->left, output);
    pre_order(root->right, output);
  }
}

void post_order(node *root, FILE *output) {
  if (root != NULL) {
    post_order(root->left, output);
    post_order(root->right, output);
    if (root->size > 1) {
      fprintf(output, "%u %s %s %u bytes\n", root->counter, root->key,
              root->type, root->size);
    } else {
      fprintf(output, "%u %s %s %u byte\n", root->counter, root->key,
              root->type, root->size);
    }
  }
}

int main(int argc, char *argv[]) {
  FILE *input = fopen(argv[1], "r");
  FILE *output = fopen(argv[2], "w");
  node *root = NULL;
  uint32_t number;
  uint32_t i;
  char line[100];
  char name[51];
  char type[3];
  uint32_t size;
  uint32_t count = 1;

  fscanf(input, "%u", &number);
  fgetc(input);

  for (i = 0; i < number; i++) {
    if (fgets(line, sizeof(line), input)) {
      uint32_t n = sscanf(line, "%49s %2s %u", name, type, &size);
      if (n == 3) {
        mode_t permission = 0644;
        if (strcmp(type, "ro") == 0) {
          permission = S_IRUSR | S_IRGRP | S_IROTH; // Read permissions
        } else if (strcmp(type, "rw") == 0) {
          permission =
              S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
          ; // Read and write permissions
        }
        count++;
        root = insert(root, name, type, size, permission, count);
      }
    }
  }

  fprintf(output, "IN ORDER:\n");
  in_order(root, output);
  fprintf(output, "PRE ORDER:\n");
  pre_order(root, output);
  fprintf(output, "POST ORDER:\n");
  post_order(root, output);

  fclose(input);
  fclose(output);

  return 0;
}