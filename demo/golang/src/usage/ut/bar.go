package ut

func Bar(n int) int {
	if n > 0 {
		return n + 1
	}
	if n < 0 {
		return n - 1
	}
	if n == 0 {
		return 0
	}
	return 0
}
