#ifndef __NETLINK_UTILS__

#include <linux/netlink.h>

static inline void nlmsg_dump (struct nlmsghdr* nlh)
{
  printk (KERN_INFO "=== NETLINK MESSAGE HEADER ===");
  printk (KERN_INFO "Length: %d\n", nlh->nlmsg_len);
  printk (KERN_INFO "Type: %x\n", nlh->nlmsg_type);
  printk (KERN_INFO "Flags: %x\n", nlh->nlmsg_flags);
  printk (KERN_INFO "Seq: %d\n", nlh->nlmsg_seq);
  printk (KERN_INFO "PID: %d\n", nlh->nlmsg_pid);
  printk (KERN_INFO "==============================");
}

#endif // !__NETLINK_UTILS__
