import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import org.apache.zookeeper.AsyncCallback.StatCallback;
import org.apache.zookeeper.CreateMode;
import org.apache.zookeeper.KeeperException;
import org.apache.zookeeper.KeeperException.Code;
import org.apache.zookeeper.WatchedEvent;
import org.apache.zookeeper.Watcher;
import org.apache.zookeeper.ZooDefs;
import org.apache.zookeeper.ZooDefs.Ids;
import org.apache.zookeeper.ZooKeeper;
import org.apache.zookeeper.data.ACL;
import org.apache.zookeeper.data.Id;
import org.apache.zookeeper.data.Stat;

public class main {
	public static void main(String args[]){
		try {
		// 创建一个与服务器的连接
		ZooKeeper zk = new ZooKeeper("localhost:2181", 1000, new Watcher() {
					// 监控所有被触发的事件
					public void process(WatchedEvent event) {
						System.out.println("已经触发了" + event.getType() + "事件！");
					}
				});
		System.out.println("new zookeeper");

//		// 创建一个目录节点
//		zk.create("/testRootPath", "testRootData".getBytes(), Ids.OPEN_ACL_UNSAFE,
//				CreateMode.PERSISTENT);
//		System.out.println("create: /testRootPath");
//
//		// 创建一个子目录节点
//		zk.create("/testRootPath/testChildPathOne", "testChildDataOne".getBytes(),
//				Ids.OPEN_ACL_UNSAFE,CreateMode.PERSISTENT);
//		System.out.println("create: /testRootPath/testChildPathOne");
//		System.out.println(new String(zk.getData("/testRootPath",false,null)));
//		System.out.println("get: /testRootPath");
//
//		// 取出子目录节点列表
//		System.out.println(zk.getChildren("/testRootPath",true));
//		System.out.println("get children: /testRootPath");
//
//		// 修改子目录节点数据
//		zk.setData("/testRootPath/testChildPathOne","modifyChildDataOne".getBytes(),-1);
//		System.out.println("目录节点状态：["+zk.exists("/testRootPath",true)+"]");
//		System.out.println("set: /testRootPath/testChildPathOne");
//
//		// 创建另外一个子目录节点
//		zk.create("/testRootPath/testChildPathTwo", "testChildDataTwo".getBytes(),
//				Ids.OPEN_ACL_UNSAFE,CreateMode.PERSISTENT);
//		System.out.println("create: /testRootPath/testChildPathTwo");
//		System.out.println(new String(zk.getData("/testRootPath/testChildPathTwo",true,null)));
//		System.out.println("get: /testRootPath/testChildPathTwo");
//
//		// 删除子目录节点
//		zk.delete("/testRootPath/testChildPathTwo",-1);
//		System.out.println("delete: /testRootPath/testChildPathTwo");
//		zk.delete("/testRootPath/testChildPathOne",-1);
//		System.out.println("delete: /testRootPath/testChildPathOne");
//
//		// 删除父目录节点
//		zk.delete("/testRootPath",-1);
//		System.out.println("delete: /testRootPath");

		// 关闭连接
		zk.close();
		System.out.println("close zookeeper");

		} catch(Exception e) {
			System.out.println(e.getMessage());
		}

		System.out.println("Hello World!");
	}
}

