package main

import "net/http"
import "log"
import "fmt"

func myHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Println(r)
	fmt.Fprintf(w, "Hello there!\n")
}

func main() {
	http.HandleFunc("/", myHandler)
	log.Fatal(http.ListenAndServe(":65535", nil))
}
