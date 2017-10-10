#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/proc_fs.h>
int procfile_read(char *buffer, char **buffer_location, off_t offset, int buffer_length, int *eof, void *data)
{
	if (offset > 0)
	{
		return 0;
	}
	else
	{
		return sprintf(buffer, "Hello world\n");
	}
}

int init_module(void)
{
	struct proc_dir_entry* dir = create_proc_entry("myproc", 0644, NULL);
	dir->read_proc = procfile_read;
	return 0;
}

void cleanup_module(void)
{
	remove_proc_entry("myproc", NULL);
}



