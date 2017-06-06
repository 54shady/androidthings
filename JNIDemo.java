public class JNIDemo {
	/* static code will be called when new class object */
	static {
		/* 1. load */
		System.loadLibrary("native"); /* libnative.so */
	}
	public native static void hello();
	public static void main(String args[]) {

		/* 3. call the native function */
		hello();
	}
}
