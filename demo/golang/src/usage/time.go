package main

import "fmt"
import "time"

func main() {
	// time create
	var t_now = time.Now()
	fmt.Println("time.Now: ", t_now)
	var t_from_unix = time.Unix(1487780010, 0)
	fmt.Println("time.Unix: ", t_from_unix)
	var t_diy = time.Date(2018, time.January, 1, 0, 0, 0, 0, time.Local)
	fmt.Println("time.Unix: ", t_diy)
	fmt.Println()

	// time format
	fmt.Println("time.Format with RFC822: ", t_now.Format(time.RFC822))
	fmt.Println("unix timestamp: ", t_now.Unix())
	fmt.Println()

	// time serialize & deserialize
	var t_byte []byte
	var err error
	t_byte, err = t_now.MarshalBinary()
	fmt.Println("time_now MarshalBinary: ", t_byte, err)
	err = t_now.UnmarshalBinary(t_byte)
	fmt.Println("time_now UnmarshalBinary: ", t_now, err)
	t_byte, err = t_now.MarshalJSON()
	fmt.Println("time_now MarshalJSON: ", t_byte, err)
	err = t_now.UnmarshalJSON(t_byte)
	fmt.Println("time_now UnmarshalJSON: ", t_now, err)
	t_byte, err = t_now.MarshalText()
	fmt.Println("time_now MarshalText: ", t_byte, err)
	err = t_now.UnmarshalText(t_byte)
	fmt.Println("time_now UnmarshalText: ", t_now, err)
	t_byte, err = t_now.GobEncode()
	fmt.Println("time_now GobEncode: ", t_byte, err)
	err = t_now.GobDecode(t_byte)
	fmt.Println("time_now GobDecode: ", t_now, err)
	fmt.Println()

	// time duration
	var d = time.Duration(10000000000000)
	fmt.Println("duration string: ", d.String())
	d = time.Duration(time.Second)
	fmt.Println("duration time.Second: ", d.String())
	fmt.Println()

	// time calculate
	var t_tomorrow = t_now.Add(24 * time.Hour)
	var t_yestoday = t_now.Add(-24 * time.Hour)
	fmt.Println("time tomorrow: ", t_tomorrow)
	fmt.Println("time yestoday: ", t_yestoday)
	fmt.Println("check now is after than yestoday: ", t_now.After(t_yestoday))
	fmt.Println("check now is before than tomorrow: ", t_now.Before(t_tomorrow))
	fmt.Println("check now is equal to tomorrow: ", t_now.Equal(t_tomorrow))
	fmt.Println("time duration from yestoday to torommow: ", t_tomorrow.Sub(t_yestoday))
	fmt.Println()

	// time sleep
	fmt.Println(time.Now())
	fmt.Println("sleep 1 second")
	time.Sleep(time.Second)
	fmt.Println(time.Now())

	// time after click
	var c = make(chan int)
	var m int
	select {
	case m = <-c:
		fmt.Println("never get message: ", m)
	case <-time.After(2 * time.Second):
		fmt.Println("time out after 2 seconds")
	}
	close(c)

	// time ticker stop but not close
	var ticker1 = time.NewTicker(time.Second)
	go func() {
		for time := range ticker1.C {
			fmt.Println("go time tick1: ", time)
		}
		fmt.Println("go timer ticker1 finished")
	}()
	time.Sleep(5 * time.Second)
	ticker1.Stop()
	time.Sleep(1 * time.Second)
	fmt.Println("ticker1 stoped")
	fmt.Println()

	// time ticker stop and close
	var done = make(chan bool, 1)
	var ticker2 = time.NewTicker(time.Second)
	go func() {
		for {
			select {
			case time := <-ticker2.C:
				fmt.Println("go time tick2: ", time)
			case is_done := <-done:
				fmt.Println("go time check down: ", is_done)
				close(done)
				return
			}
		}
	}()
	time.Sleep(5 * time.Second)
	ticker2.Stop()
	done <- true
	time.Sleep(1 * time.Second)
	fmt.Println("ticker2 stoped")
	fmt.Println()

	// time ticker dead loop
	var t <-chan time.Time = time.Tick(time.Second)
	var time time.Time
	for time = range t {
		fmt.Println("time tick: ", time)
	}
}
