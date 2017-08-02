# Binder

## AB两个进程通信命令

只有下面四个命令涉及两个进程通信

	BC_TRANSACTION
	BR_TRANSACTION
	BC_REPLY
	BR_REPLY

A向B发送消息简单流程图

	A(BC_TRANSACTION) --send--> B(BR_TRANSACTION)
	A(BR_REPLY) <--reply-- B(BC_REPLY)

## 核心数据结构和代码分析

- 在binder驱动中用struct binder_proc描述每一个进程
- 用struct binder_thread描述进程中的线程

### binder_open

binder_open函数有如下设置,current表示当前进程

	struct binder_proc *proc;
	proc->tsk = current;

### binder_call(中数据转换)

binder_call会发起ioctrl调用,将用户程序数据发送到驱动

用户程序中使用的数据结构是binder_io,但是ioctrl中使用的数据结构是binder_rw,两者间如何转换

	binder_io->binder_transaction_data->binder_write_read

用binder_io构造binder_transaction_data

    writebuf.txn.data_size = msg->data - msg->data0;
    writebuf.txn.offsets_size = ((char*) msg->offs) - ((char*) msg->offs0);
    writebuf.txn.data.ptr.buffer = (uintptr_t)msg->data0;
    writebuf.txn.data.ptr.offsets = (uintptr_t)msg->offs0;

binder_transaction_data放入到binder_write_read

    bwr.write_size = sizeof(writebuf);
    bwr.write_buffer = (uintptr_t) &writebuf;

### ref,node,proc,thread

在client中通过svcmgr_lookup获得一个服务的handle,其中这个handle是client对server提供服务的引用

- 在binder系统中用struct binder_node描述server里提供的服务
- 在binder系统中用struct binder_ref描述这个引用(对某一个服务的引用)
- 在binder系统中用struct binder_proc描述相应的进程
- 如果handle等于binder_ref里的desc的话,就表示找到相应的引用
- 通过binder_ref里的node成员找到相应的binder_node
- 通过binder_node里的proc成员找到提供这个服务的进程

也就解释了binder_call里通过handle能够直到数据要发送给哪个进程的原因

在实际情况中,可能会有多个client调用到server里的服务,所以在server里会创建线程来处理相应的请求,也就是在binder_proc里有threads成员的原因(这些线程都是放入到红黑树中管理的,其中每一个线程用struct binder_thread描述)

## 注册服务到获取服务流程

1. 在server内核态为每一个服务创建binder_node,设置binder_node.proc = server进程
2. 在service_manager内核态创建binder_ref引用binder_node,binder_ref.desc = 1,2,3...,且service_manager在用户态创建服务链表
3. client通过服务名向service_manager查询服务
4. service_manager返回handle给binder驱动
5. binder驱动在service_manager的binder_ref里的红黑树中根据handle找到binder_ref,进而找到相应的binder_node,给client创建新的binder_ref,该binder_ref指向找到的binder_node,并将binder_ref.desc返回给client,这就是client里获得的handle
6. client使用handle发送数据时,驱动通过handle找到相应的binder_ref,进而找到相应的binder_node,再用binder_node找到相应的server进程,即完成client和server之间的通信

## C测试程序

Usage(在非android系统中测试,但是需要binder驱动支持)

	./service_manager &
	./test_server &
	./test_client hello
	./test_client hello testname
