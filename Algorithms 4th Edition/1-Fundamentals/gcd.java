// gcd: greatest common divisor
// lcm: least common multiple

public class gcd {
	private static int gcd(int x, int y) {
		if (y == 0) return x;
		return gcd(y, x % y);
	}

	private static int lcm(int x, int y) {
		return x * y / gcd(x, y);
	}

	public static void main(String[] args) {
		System.out.println(String.valueOf(gcd(319, 377)));
		System.out.println(String.valueOf(lcm(18, 20)));
		System.out.println("Hello World");
	}
}

