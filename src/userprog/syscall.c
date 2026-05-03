#include <stdio.h>
#include <syscall-nr.h>
#include "userprog/syscall.h"
#include "threads/interrupt.h"
#include "threads/thread.h"
/* header files you probably need, they are not used yet */
#include <string.h>
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/vaddr.h"
#include "threads/init.h"
#include "userprog/pagedir.h"
#include "userprog/process.h"
#include "devices/input.h"
#include "userprog/plist.h"
#include "../devices/timer.h"

static void syscall_handler(struct intr_frame *);

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

/* This array defined the number of arguments each syscall expects.
   For example, if you want to find out the number of arguments for
   the read system call you shall write:

   int sys_read_arg_count = argc[ SYS_READ ];

   All system calls have a name such as SYS_READ defined as an enum
   type, see `lib/syscall-nr.h'. Use them instead of numbers.
 */
const int argc[] = {
    /* basic calls */
    0, 1, 1, 1, 2, 1, 1, 1, 3, 3, 2, 1, 1,
    /* not implemented */
    2, 1, 1, 1, 2, 1, 1,
    /* extended, you may need to change the order of these two (plist, sleep) */
    1, 0};

static void
syscall_handler(struct intr_frame *f)
{
  int32_t *esp = (int32_t *)f->esp;

  switch (*esp)
  {
  default:
  {
    printf("Executed an unknown system call!\n");

    printf("Stack top + 0: %d\n", esp[0]);
    printf("Stack top + 1: %d\n", esp[1]);

    thread_exit();
  };
  case SYS_HALT:
  {
    power_off();
    break;
  };

  case SYS_EXIT:
  {

    process_exit(thread_current()->tid);
    f->eax = esp[1];
    thread_exit();
    break;
  };

  case SYS_READ:
  {

    int fd = esp[1];
    char *buf = (char *)esp[2]; // Have to cast void* to char* fml how is this legal?
    int size = esp[3];
    if (fd == STDIN_FILENO)
    {
      for (size_t i = 0; i < size; i++)
      {
        char c = input_getc();

        if (c == '\r')
        {
          buf[i] = '\n';

          // break;
        }
        else
        {
          buf[i] = c;
        }
        putchar(c);
      }
      f->eax = size;
      break;
    }
    else
    {
      struct thread *curr = thread_current();
      struct file *file = (struct file *)map_find(&curr->opened_files, fd);
      if (file == NULL)
      {
        f->eax = -1;
        break;
      }

      f->eax = file_read(file, buf, size);
    }

    break;
  }

  case SYS_WRITE:
  {
    int fd = esp[1];
    if (fd == STDOUT_FILENO)
    {
      putbuf((char *)esp[2], esp[3]);
    }
    else // we write to file probs
    {
      struct thread *curr = thread_current();
      struct file *file = (struct file *)map_find(&curr->opened_files, fd);
      if (file == NULL)
      {
        f->eax = -1;
        break;
      }
      f->eax = file_write(file, (void *)esp[2], esp[3]);
    }

    break;
  }

  case SYS_CREATE:
  {
    f->eax = filesys_create((char *)esp[1], esp[2]);
    break;
  };

  case SYS_REMOVE:
  {
    if (is_user_vaddr(esp[1]))
    {
      int fd = esp[1];
      // struct thread *curr = thread_current();
      // struct file *file = (struct file *)map_find(&curr->opened_files, fd);
      //  map_remove(&curr->opened_files, fd);

      f->eax = filesys_remove((char *)esp[1]);
      break;
    }
  }

  case SYS_OPEN:
  {
    char *filename = (char *)esp[1];

    struct file *file = filesys_open(filename);

    if (file != NULL)
    {
      struct thread *curr = thread_current(); // Need to be ptr to access live data

      int fd = map_insert(&curr->opened_files, file);

      if (fd == -1)
      {
        f->eax = -1;
        filesys_close(file);
        break;
      }

      f->eax = fd;
      break;
    }
    else
    {
      f->eax = -1;
    }

    break;
  }

  case SYS_CLOSE:
  {
    int fd = esp[1];
    struct thread *curr = thread_current();
    struct file *file = (struct file *)map_remove(&curr->opened_files, fd);
    if (file == NULL)
    {
      break;
    }

    filesys_close(file);
    break;
  }

  case SYS_SEEK:
  {
    int fd = esp[1];
    struct thread *curr = thread_current();
    struct file *file = (struct file *)map_find(&curr->opened_files, fd);

    if (file_length(file) > esp[2])
    {
      file_seek(file, esp[2]);
    }
    else
    {
      file_seek(file, file_length(file));
    }
    break;
  }

  case SYS_TELL:
  {
    int fd = esp[1];
    struct thread *curr = thread_current();
    struct file *file = (struct file *)map_find(&curr->opened_files, fd);

    if (file == NULL)
    {
      f->eax = -1;
      break;
    }
    else
    {
      f->eax = file_tell(file);
    }
    break;
  }

  case SYS_FILESIZE:
  {
    int fd = esp[1];
    struct thread *curr = thread_current();
    struct file *file = map_find(&curr->opened_files, fd);

    if (file == NULL)
    {
      f->eax = -1;
      break;
    }

    f->eax = file_length(file);
    break;
  }
    // lab 3c2
  case SYS_EXEC:
  {
    if (esp[1] == NULL)
    {
      thread_exit();
    }

    f->eax = process_execute(esp[1]);

    break;
  }

  case SYS_PLIST:
  {
    p_map_for_each(p_print, 0);
    break;
  }

  case SYS_SLEEP:
  {
    timer_msleep((int)esp[1]);
    break;
  }
  }
}
