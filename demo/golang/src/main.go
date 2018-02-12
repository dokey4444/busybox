package main

import "flag"
import "time"
import "github.com/golang/glog"

func main() {
	flag.Parse()

	for {
		glog.Infof("glog info %s", "abc")
		glog.Warning("glog warning")
		glog.Error("glog error")

		glog.V(1).Info("glog info v1")
		glog.V(2).Info("glog info v2")
		glog.V(3).Info("glog info v3")

		glog.Flush()
		time.Sleep(time.Second)
	}

	//	glog.Fatal("glog fatal")

	defer glog.Flush()
}
