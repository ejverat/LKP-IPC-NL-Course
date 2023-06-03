#include <linux/module.h>
#include <linux/netlink.h>
#include <linux/string.h>
#include <net/sock.h>

#include "utils/Netlink.h"

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

static struct sock* nl_sk = NULL;

static void netlink_recv_msg_fn (struct sk_buff* skb_in)
{
  struct nlmsghdr *recv, *reply;
  int uspace_pid;
  char* uspace_data;
  int uspace_data_len;
  char kernel_reply[256];
  struct sk_buff* skb_out;
  int res;

  printk (KERN_INFO "%s() invoked", __FUNCTION__);

  recv = (struct nlmsghdr*)(skb_in->data);

  nlmsg_dump (recv);

  uspace_pid = recv->nlmsg_pid;

  printk (KERN_INFO "%s(%d) : port id of the sending user space process = %u\n",
          __FUNCTION__, __LINE__, uspace_pid);

  uspace_data     = (char*)nlmsg_data (recv);
  uspace_data_len = skb_in->len;

  printk (KERN_INFO "%s(%d) : msg recvd from user space = %s, skb_in->len = "
                    "%d, nlh->nlmsg_len = %d\n",
          __FUNCTION__, __LINE__, uspace_data, uspace_data_len, recv->nlmsg_len);

  if (recv->nlmsg_flags & NLM_F_ACK) {
    memset (kernel_reply, 0, sizeof (kernel_reply));
    snprintf (kernel_reply, sizeof (kernel_reply),
              "Msg from Process %d has been processed by kernel", recv->nlmsg_pid);

    skb_out = nlmsg_new (sizeof (kernel_reply), 0);
    reply =
    nlmsg_put (skb_out, 0, recv->nlmsg_seq, NLMSG_DONE, sizeof (kernel_reply), 0);

    strncpy (nlmsg_data (reply), kernel_reply, sizeof (kernel_reply));

    res = nlmsg_unicast (nl_sk, skb_out, uspace_pid);

    if (res < 0) {
      printk (KERN_INFO "Error while sending the data back to user space\n");
      kfree_skb (skb_out);
    }
  }
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

MODULE_AUTHOR ("Emmanuel Vera");
MODULE_DESCRIPTION ("Greeter example of the netlink sockets");
MODULE_LICENSE ("GPL");
