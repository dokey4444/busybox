package ut

import "testing"

func TestFoo(t *testing.T) {
	ret := Foo(2)
	if ret != 1 {
		t.Error("Expect 1, got ", ret)
	}
}

func BenchmarkFoo(b *testing.B) {
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		Foo(i)
	}
}
