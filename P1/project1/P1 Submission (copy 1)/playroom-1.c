#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "params.h"

#include "playroom.h"

// Initialization and destruction functions
int playroom_init(struct playroom *room)
{
   int rv;

   // Zero everything by default
   memset(room, 0, sizeof *room);

   // Initialize the mutex and conditionals
   pthread_mutex_init(&room->animalLock, NULL);
   pthread_cond_init(&room->noCats, NULL);
   pthread_cond_init(&room->noDogs, NULL);

   rv = pthread_mutex_init(&room->lock, NULL);
   if (rv)
	   return rv;

   return 0;
}

void playroom_destroy(struct playroom *room)
{
   while (pthread_mutex_destroy(&room->lock) == EBUSY);
   pthread_mutex_destroy(&room->animalLock);
   pthread_cond_destroy(&room->noDogs);
   pthread_cond_destroy(&room->noCats);
}

// Entry/exit functions
void cat_enter(struct playroom *room)
{
  pthread_mutex_lock(&room->animalLock);
  room->cats.wait++;
  while(room->dogs.n > 0) {
    pthread_cond_wait(&room->noDogs, &room->animalLock);
      }
  room->cats.wait--;
  room->cats.n++;
  room->cats.total++;
  pthread_mutex_unlock(&room->animalLock);
}

void cat_exit(struct playroom *room)
{
  pthread_mutex_lock(&room->animalLock);
  room->cats.n--;
  if (room->cats.n < 1)
    pthread_cond_broadcast(&room->noCats);
  pthread_mutex_unlock(&room->animalLock);
}

void dog_enter(struct playroom *room)
{
  pthread_mutex_lock(&room->animalLock);
  room->dogs.wait++;
  while(room->cats.n > 0) {
    pthread_cond_wait(&room->noCats, &room->animalLock);
      }
  room->dogs.wait--;
  room->dogs.n++;
  room->dogs.total++;
  pthread_mutex_unlock(&room->animalLock);
}

void dog_exit(struct playroom *room)
{
  pthread_mutex_lock(&room->animalLock);
  room->dogs.n--;
  if (room->dogs.n < 1)
    pthread_cond_broadcast(&room->noDogs);
  pthread_mutex_unlock(&room->animalLock);
}

// Output functions
void playroom_print(struct playroom *room)
{
  // Use this so we get a consistent snapshot
  pthread_mutex_lock(&room->lock);
  printf("play: %2dc %2dd     wait: %2dc %2dd     total: %2dc %2dd\n",
	 room->cats.n, room->dogs.n,
	 room->cats.wait, room->dogs.wait,
	 room->cats.total, room->dogs.total);
  pthread_mutex_unlock(&room->lock);
}

void playroom_visual(struct playroom *room)
{
  int i;

  // Print out a one-line "visual" view of the playroom state
  pthread_mutex_lock(&room->lock);
  for (i = NUM_CATS; i > room->cats.wait; i--)
    printf(" ");
  for (; i > 0; i--)
		printf("c");
  printf("|");
  for (i = 0; i < room->cats.n; i++)
    printf("c");
  for (; i < NUM_CATS; i++)
    printf(" ");

  for (i = NUM_DOGS; i > room->dogs.n; i--)
    printf(" ");
  for (; i > 0; i--)
    printf("d");
  printf("|");
  for (i = 0; i < room->dogs.wait; i++)
    printf("d");
  for (; i < NUM_DOGS; i++)
    printf(" ");

  printf("\n");
  pthread_mutex_unlock(&room->lock);
}
