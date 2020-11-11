#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/hugetlb.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>

static int getram(struct seq_file *m, void *v)
{
    struct sysinfo i;
    unsigned long pages[NR_LRU_LISTS];
    int lru;
#define K(x) ((x) << (PAGE_SHIFT - 10))
    si_meminfo(&i);
    for (lru = LRU_BASE; lru < NR_LRU_LISTS; lru++)
        pages[lru] = global_numa_state(NR_LRU_BASE + lru);        
    seq_printf(m, "{");
    seq_printf(m, "\"ram\": %8lu,",K(i.totalram)/1024);
    seq_printf(m, "\"libre\": %8lu,",K(i.freeram)/1024);
    seq_printf(m, "\"buffer\": %8lu,",K(i.bufferram)/1024);
    seq_printf(m, "\"uso\": %8lu,",(K(i.totalram)-K(i.freeram)-K(i.bufferram))/1024);
    seq_printf(m, "\"porcentaje\": %8lu",(((K(i.totalram)-K(i.freeram)-K(i.bufferram))/1024)*100)/(K(i.totalram)/1024));
    seq_printf(m, "}\n");
#undef K
    return 0;
}

static int ram_proc_open(struct inode *inode, struct file *file)
{
    return single_open(file, getram, NULL);
}

//STRUCT OPERACIONES
static const struct file_operations ram_proc_fops = {
    .owner = THIS_MODULE,
    .open = ram_proc_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

//MODULO PARA CREAR EL PROCESO
static int __init ram_start(void)
{
    proc_create("modram", 0, NULL, &ram_proc_fops);
    return 0;
}

//MODULO PARA SALIR DEL PROCESO
static void __exit ram_end(void) //Salida de modulo
{
    remove_proc_entry("modram", NULL);
    printk(KERN_INFO "Cleaning Up.\n");
}

module_init(ram_start);
module_exit(ram_end);
MODULE_LICENSE("GPL");

