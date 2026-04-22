/* IPC large message test: exercise near IPC_REPLY_SIZE_DEFAULT */

#include <testlib.h>
#include <mach/mach.h>
#include <mach/message.h>

/* Simple round-trip using mach_port_allocate and mach_msg to self */
int main(int argc, char *argv[], int envc, char *envp[])
{
  kern_return_t kr;
  mach_port_t recv;
  mach_port_t send;
  const size_t payload = 8000; /* close to default reply size (8192) */

  /* Create a receive right */
  recv = mach_reply_port();
  ASSERT_RET(recv != MACH_PORT_NULL ? KERN_SUCCESS : KERN_INVALID_NAME, "mach_reply_port");

  /* Insert a send right to the same port */
  kr = mach_port_insert_right(mach_task_self(), recv, recv, MACH_MSG_TYPE_MAKE_SEND);
  ASSERT_RET(kr, "mach_port_insert_right");
  send = recv;

  typedef struct {
    mach_msg_header_t head;
    char bytes[payload];
  } msg_t;

  msg_t sendmsg = {0};
  msg_t rcvmsg = {0};

  sendmsg.head.msgh_bits = MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, MACH_MSG_TYPE_MAKE_SEND_ONCE);
  sendmsg.head.msgh_remote_port = send;
  sendmsg.head.msgh_local_port = recv;
  sendmsg.head.msgh_id = 0x4242;
  for (size_t i = 0; i < sizeof(sendmsg.bytes); i++) sendmsg.bytes[i] = (char)(i & 0xFF);

  kr = mach_msg(&sendmsg.head, MACH_SEND_MSG, sizeof(sendmsg), 0, MACH_PORT_NULL, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
  ASSERT_RET(kr, "mach_msg send");

  kr = mach_msg(&rcvmsg.head, MACH_RCV_MSG, 0, sizeof(rcvmsg), recv, MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
  ASSERT_RET(kr, "mach_msg receive");

  ASSERT_RET(rcvmsg.head.msgh_id == 0x4242 ? KERN_SUCCESS : KERN_INVALID_ARGUMENT, "msgh_id mismatch");
  for (size_t i = 0; i < sizeof(sendmsg.bytes); i++) {
    if (rcvmsg.bytes[i] != (char)(i & 0xFF)) {
      ASSERT_RET(KERN_INVALID_ADDRESS, "payload mismatch");
    }
  }
  return 0;
}


