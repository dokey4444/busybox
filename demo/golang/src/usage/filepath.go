package main

import "fmt"
import "os"
import "path/filepath"

func main() {
	var path = "./"
	err := filepath.Walk(path, func(path string, f os.FileInfo, err error) error {
		if f == nil {
			return err
		}
		fmt.Printf("%s\t%t\t%s\n", f.ModTime(), f.Mode().IsRegular(), path)
		return nil
	})
	if err != nil {
		fmt.Println("filepath.Walk: ", err)
	}
}
