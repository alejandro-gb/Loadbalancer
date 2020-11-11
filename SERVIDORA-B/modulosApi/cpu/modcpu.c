#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/module.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/tick.h>
#include <linux/cpumask.h>
#include <linux/interrupt.h>
#include <linux/kernel_stat.h>
#include <linux/time.h>
#include <linux/irqnr.h>
#include <sys/types.h>
#include <sys/param.h>

const double ticks = (double)sysconf( _SC_CLK_TCK);

#ifdef arch_idle_time

//IDLE - TIEMPO INACTIVO DEL CPU
static cputime64_t get_idle_time(int cpu)
{
	cputime64_t idle;
	idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
	if (cpu_online(cpu) && !nr_iowait_cpu(cpu))
		idle += arch_idle_time(cpu);
	return idle;
}

//IOWAIT - TIEMPO DE ESPERA PARA HACER UNA OPERACION DE IO
static cputime64_t get_iowait_time(int cpu)
{
	cputime64_t iowait;
	iowait = kcpustat_cpu(cpu).cpustat[CPUTIME_IOWAIT];
	if (cpu_online(cpu) && nr_iowait_cpu(cpu))
		iowait += arch_idle_time(cpu);
	return iowait;
}
#else

static u64 get_idle_time(int cpu)
{
	u64 idle, idle_time = -1ULL;
	if (cpu_online(cpu))
		idle_time = get_cpu_idle_time_us(cpu, NULL);
	if (idle_time == -1ULL)
		idle = kcpustat_cpu(cpu).cpustat[CPUTIME_IDLE];
	else
		idle = nsecs_to_jiffies64(idle_time);
	return idle;
}

static u64 get_iowait_time(int cpu)
{
	u64 iowait, iowait_time = -1ULL;
	if (cpu_online(cpu))
		iowait_time = get_cpu_iowait_time_us(cpu, NULL);

	if (iowait_time == -1ULL)
		iowait = kcpustat_cpu(cpu).cpustat[CPUTIME_IOWAIT];
	else
		iowait = nsecs_to_jiffies64(iowait_time);
	return iowait;
}

#endif

static int meminfo_proc_show(struct seq_file *m, void *v)
{
	//VARIABLES
	int i;
	unsigned long tiempo;
	u64 usercputime, usernicecputime, systemcputime, idle, iowait, irq, softwareirq, stealtime;
	u64 guestcputime, guestnicecpu;
	
	u64 total = 0;

	struct timespec boottime;

	usercputime = usernicecputime = systemcputime = idle = iowait = irq = softwareirq = stealtime = 0;
	guestcputime = guestnicecpu = 0;
	getboottime(&boottime);
	tiempo = boottime.tv_sec;

	for_each_possible_cpu(i)
	{
		usercputime += kcpustat_cpu(i).cpustat[CPUTIME_USER];//TIEMPO DE CPU GASTADO EN EL ESPACIO DE USUARIO
		usernicecputime += kcpustat_cpu(i).cpustat[CPUTIME_NICE];//TIEMPO GASTADO EN PROCESOS DE BAJA PRIORIDAD
		systemcputime += kcpustat_cpu(i).cpustat[CPUTIME_SYSTEM];//TIEMPO DE CPU GASTADO EN EL ESPACIO DEL KERNEL
		idle += get_idle_time(i);
		iowait += get_iowait_time(i);
		irq += kcpustat_cpu(i).cpustat[CPUTIME_IRQ];//TIEMPO MANEJANDO INTERRUPCIONES DE HARDWARE
		softwareirq += kcpustat_cpu(i).cpustat[CPUTIME_SOFTIRQ];//TIEMPO MANEJANDO INTERRUPCIONES DE SOFTWARE
		stealtime += kcpustat_cpu(i).cpustat[CPUTIME_STEAL];//TIEMPO ES ESPERA INVOLUNTARIO POR UNA MAQUINA VITUAL
		guestcputime += kcpustat_cpu(i).cpustat[CPUTIME_GUEST];//TIEMPO EN MODO INVITADO
		guestnicecpu += kcpustat_cpu(i).cpustat[CPUTIME_GUEST_NICE];//TIEMPO EN MODO INVITADO DE BAJA PRIORIDAD
	}

	total = total + usercputime + usernicecputime + systemcputime + idle + iowait + irq + softwareirq + stealtime + guestcputime + guestnicecpu;
	
	//JIFFIE INDICA EL TIEMPO QUE HA PASDO DESDE QUE SE INICIO EL SISTEMA
	seq_printf(m, "{");
	seq_printf(m, "\"cpu\":");
	seq_put_decimal_ull(m, " ", jiffies_64_to_clock_t(total));
	seq_printf(m, ",\"used\":");
	seq_put_decimal_ull(m, " ", jiffies_64_to_clock_t((idle / total) * 100));
	seq_printf(m, ",\"free\":");
	seq_put_decimal_ull(m, " ", ticks);
	seq_printf(m, "\"porcentaje\":");
	seq_put_decimal_ull(m, " ", jiffies_64_to_clock_t(((total - idle) * 100 / total)));
	seq_printf(m, "}");
	seq_printf(m,"\n");
	return 0;
}


//METODO QUE SE EJECUTA AL SALI DEL MODULO
static void __exit endcpu(void)
{
	printk(KERN_INFO "Cleaning Up.\n");
}

static int meminfo_proc_open(struct inode *inode, struct file *file)
{
	return single_open(file, meminfo_proc_show, NULL);
}

//VALORES
static const struct file_operations meminfo_proc_fops = {
	.open = meminfo_proc_open,
	.read = seq_read,
	.llseek = seq_lseek,
	.release = single_release,
};

//METODO QUE SE EJECUTA AL INCIAR EL METODO PARA ESCRIBIR EN EL PROC
static int __init startcpu(void)
{
	proc_create("modcpu", 0, NULL, &meminfo_proc_fops);
	return 0;
}


module_init(startcpu);
module_exit(endcpu);
MODULE_LICENSE("GPL");