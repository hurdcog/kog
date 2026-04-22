/* Hurd unionfs
   Copyright (C) 2005 Free Software Foundation, Inc.
   Written by Gianluca Guida <glguida@gmail.com>.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or * (at your option) any later version.
 
   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.
 
   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
   USA.  */

/* Update thread: A clean way to solve locking issues of 
   root node update.  */

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "ncache.h"
#include "node.h"
#include "ulfs.h"

/* Reader lock is used by threads that are going to
   add/remove an ulfs; writer lock is hold by the 
   update thread.  */
static pthread_rwlock_t update_rwlock;
static pthread_cond_t update_wakeup;
static pthread_mutex_t update_lock;

static void *
_root_update_thread (void *arg)
{
  error_t err;
  
  while (1)
    {
      if (pthread_hurd_cond_wait_np (&update_wakeup, &update_lock))
	pthread_mutex_unlock (&update_lock);

      pthread_rwlock_wrlock (&update_rwlock);

      do 
	{
	  ulfs_check();
	  err = node_init_root (netfs_root_node);
	}
      while (err == ENOENT);

      if (err)
	{
	  fprintf (stderr, "update thread: got a %s\n", strerror (err));
	}

      ncache_reset ();

      pthread_rwlock_unlock (&update_rwlock);
    }

  return NULL;
}

void
root_update_schedule ()
{
  pthread_cond_signal (&update_wakeup);
}

void
root_update_disable ()
{
  pthread_rwlock_rdlock (&update_rwlock);
}

void
root_update_enable ()
{
  pthread_rwlock_unlock (&update_rwlock);
}

void
root_update_init()
{
  pthread_t thread;
  error_t err;

  pthread_mutex_init (&update_lock, NULL);
  pthread_rwlock_init (&update_rwlock, NULL);
  pthread_cond_init (&update_wakeup, NULL);

  err = pthread_create (&thread, NULL, _root_update_thread, NULL);
  if (!err)
    pthread_detach (thread);
  else
    perror ("root_update_init/pthread_create");
}
