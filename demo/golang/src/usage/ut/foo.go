package ut

func Foo(n int) int {
	if n > 0 {
		return 1
	}
	if n < 0 {
		return -1
	}
	if n == 0 {
		return 0
	}
	return 0
}
