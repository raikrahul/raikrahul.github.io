#include <linux/module.h>
#include <linux/kprobes.h>
#include <linux/sched.h>
#include <linux/ptrace.h>

static struct kprobe kp = {
    .symbol_name = "force_sig_fault",
};

static int handler_pre(struct kprobe *p, struct pt_regs *regs)
{
    /* Capture only our target program 'buggy' */
    if (strcmp(current->comm, "buggy") == 0) {
        int sig = (int)regs->di;
        int code = (int)regs->si;
        unsigned long addr = (unsigned long)regs->dx;

        pr_info("KPROBE CRASH: force_sig_fault triggered by '%s' (pid: %d)!\n",
                current->comm, current->pid);
        pr_info("  Signal: %d, Code: %d, Faulting Address: 0x%lx\n",
                sig, code, addr);
    }
    return 0;
}

static int __init kprobe_init(void)
{
    int ret;
    kp.pre_handler = handler_pre;
    ret = register_kprobe(&kp);
    if (ret < 0) {
        pr_err("kprobe_crash: registration failed, returned %d\n", ret);
        return ret;
    }
    pr_info("kprobe_crash: planted kprobe at force_sig_fault\n");
    return 0;
}

static void __exit kprobe_exit(void)
{
    unregister_kprobe(&kp);
    pr_info("kprobe_crash: unregistered kprobe\n");
}

module_init(kprobe_init)
module_exit(kprobe_exit)
MODULE_LICENSE("GPL");
