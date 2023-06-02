#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/string.h>
#include <net/sock.h>

#include "utils/Netlink.h"

#define NETLINK_TEST_PROTOCOL 31

static struct sock* nl_sk = NULL;

static void netlink_recv_msg_fn (struct sk_buff* skb_in)
{
}

static struct netlink_kernel_cfg cfg = {
  .input = netlink_recv_msg_fn,
};

static int __init GreetNetlinkLKM_init (void)
{
  printk (KERN_INFO "Hello kernel, I'm the module GreetNetlinkLKM.ko\n");

  nl_sk = netlink_kernel_create (&init_net, NETLINK_TEST_PROTOCOL, &cfg);

  if (!nl_sk) {
    printk (KERN_INFO "Kernel netlink socket for Netlink protocol %u failed.\n",
            NETLINK_TEST_PROTOCOL);
    return -ENOMEM;
  } else {
    printk (KERN_INFO "Netlink socket created successfully.\n");
  }
  return 0;
}

static void __exit GreetNetlinkLKM_exit (void)
{
  printk (KERN_INFO "Bye. Exiting kernel module GreetNetlinkLKM.ko\n");
  netlink_kernel_release (nl_sk);
  nl_sk = NULL;
}

module_init (GreetNetlinkLKM_init);
module_exit (GreetNetlinkLKM_exit);

MODULE_LICENSE ("GPL");
