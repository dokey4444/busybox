package main

import "fmt"
import "flag"

func main() {
	var a *string = flag.String("a", "default-a", "usage of a")
	var b string
	flag.StringVar(&b, "b", "default-b", "usage of b")

	flag.Parse()

	fmt.Println(*a)
	fmt.Println(b)
}
