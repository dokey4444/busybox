package ut

import "testing"

func TestBar(t *testing.T) {
	ret := Bar(2)
	if ret != 3 {
		t.Error("Expect 3, got ", ret)
	}
}

func BenchmarkBar(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		Bar(i)
	}
}
