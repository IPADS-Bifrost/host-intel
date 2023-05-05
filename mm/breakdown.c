#include <linux/kernel.h>
#include <linux/module.h>

static int cmd = 0;
static int arg = 0;
MODULE_SOFTDEP("post: kvm_intel");
module_param(cmd, int, S_IRUGO);
module_param(arg, int, S_IRUGO);
#define VCPU_NR (48)
extern unsigned long before_cycles;
extern unsigned long after_cycles;
extern atomic64_t vmexit_cnt[];
extern atomic64_t vmexit_cycle[];
extern bool vmexit_record_en;
extern uint64_t breakdown_st;
extern atomic_t vmexit_first[];

static int __init breakdown_init(void)
{

        //pr_info("%s", __func__);
        int i;
        uint64_t en;
        switch (cmd)
        {
        case 0:
                pr_info("vmexit_record_en set to %s\n", 
                            arg == 1?"true":"false");
                breakdown_st = rdtsc_ordered();
                vmexit_record_en = (arg == 1);
                break;
        case 1:
                before_cycles = arg >> 1;
                after_cycles = arg >> 1;
                pr_info("total_cycles set to %lld\n", arg);
                break;
        //case 2:
        //        after_cycles = arg;
        //        pr_info("after_cycles set to %lld", after_cycles);
        //        break;
        case 2:
                vmexit_record_en = 0;
                for (i = 0; i < 100; i++)
                atomic64_set(&vmexit_cnt[i], 0);
                for (i = 0; i < 100; i++)
                atomic64_set(&vmexit_cycle[i], 0);
                for (i = 0; i < VCPU_NR; i++) {
                    atomic_set(&vmexit_first[i], 1);
                }
                // atomic_set(&vmexit_first, 1);
                break;
        case 3:
                if (vmexit_record_en)
                    en = rdtsc_ordered();
                vmexit_record_en = 0;
                pr_info("Total %lld cycles\n", en - breakdown_st);
                for (i = 0; i < 100; i++) {
                    uint64_t cnt, cycle;
                    cnt = atomic64_read(&vmexit_cnt[i]);
                    cycle = atomic64_read(&vmexit_cycle[i]);
                    if (cnt) {
                        pr_info("vmexit reason %d cnt %lld cycle %lld\n", i, cnt, cycle);
                    }
                }
                break;
        default:
                pr_err("unexpected cmd:%d", cmd);
        }
        return 0;
}

static void __exit breakdown_exit(void)
{
        //pr_info("%s", __func__);
}

module_init(breakdown_init)
module_exit(breakdown_exit)
MODULE_LICENSE("GPL");

