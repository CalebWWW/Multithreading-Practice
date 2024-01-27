#ifndef PLAYROOM_H
#define PLAYROOM_H

#include <pthread.h>

struct pet_counter {
  // Number currently in the room
  int n;
  // Number currently waiting outside
  int wait;

  // Number of useless wakeups
  int wakeup;
  // Number of successful entries
  int total;
};

struct playroom {
  pthread_mutex_t lock;

  pthread_mutex_t animalLock;
  
  pthread_cond_t allowWolves;
  pthread_cond_t allowCats;

  pthread_cond_t allowDogs;
  pthread_cond_t allowBirbs;
  pthread_cond_t allowMice;

  // Animal counters
  struct pet_counter cats, dogs, birds, wolves, mice;
  
  // Flag that room is shutting down
  int ending;
};

int playroom_init(struct playroom *room);
void playroom_destroy(struct playroom *room);
void controller(struct playroom *room);

void cat_enter(struct playroom *room);
void cat_exit(struct playroom *room);

void dog_enter(struct playroom *room);
void dog_exit(struct playroom *room);

void bird_exit(struct playroom *room);
void bird_enter(struct playroom *room);

void mouse_enter(struct playroom *room);
void mouse_exit(struct playroom *room);

void wolf_exit(struct playroom *room);
void wolf_enter(struct playroom *room);

void playroom_print(struct playroom *room);
void playroom_visual(struct playroom *room);
void print_uselessWakeups(struct playroom *room);

#endif
